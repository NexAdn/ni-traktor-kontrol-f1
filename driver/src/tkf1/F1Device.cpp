#include <cassert>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <string>

#include "F1Device.hpp"
#include "io/HidDevice.hpp"

namespace
{
namespace out_offsets
{
constexpr std::size_t MATRIX{0x19};
constexpr std::size_t SPECIAL{0x10};
constexpr std::size_t STOP1{0x49};
constexpr std::size_t STOP2{0x4a};
constexpr std::size_t SEGMENT_RIGHT{0x01};
constexpr std::size_t SEGMENT_LEFT{0x09};
} // namespace out_offsets

F1Device::SegmentChar digit_to_segment(std::uint8_t digit);
} // namespace

class F1Device::Impl
{
public:
	explicit Impl(HidDevice&& dev) : dev(std::move(dev))
	{
		for (std::size_t i = 0; i < OUTPUT_REPORT_SIZE; ++i) {
			out_report.at(i) = 0x00U;
		}
		update_out_report();
	}

	void update_in_state()
	{
		old_input_state = input_state;
		if (in_report.at(0) != INPUT_REPORT_ID)
			return;

		input_state.report_id = in_report.at(0);

		for (std::size_t i = 0; i < 2; ++i) {
			for (std::size_t j = 0; j < 8; ++j) {
				input_state.matrix_btns[j + 8 * i] =
					(((1U << (7U - j)) & in_report.at(i + 1)
					 ) != 0U);
			}
		}

		for (std::size_t i = 2; i < 8; ++i) {
			input_state.special_btns[i - 2] =
				(((1U << i) & in_report.at(3)) != 0U);
		}

		for (std::size_t i = 1; i < 4; ++i) {
			input_state.special_btns[i + 5] =
				(((1U << i) & in_report.at(4)) != 0U);
		}

		for (std::size_t i = 0; i < 4; ++i) {
			input_state.stop_btns[i] =
				(((1U << (4 + (3 - i))) & in_report.at(4)) != 0U
				);
		}

		input_state.wheel = in_report.at(5);

		for (std::size_t i = 0; i < 4; ++i) {
			input_state.knobs.at(i
			) = static_cast<std::uint16_t>(in_report.at(6 + i * 2)
			    ) |
			    static_cast<std::uint16_t>(
				    in_report.at(7 + i * 2) << 8U
			    );
		}

		for (std::size_t i = 0; i < 4; ++i) {
			input_state.faders.at(i
			) = static_cast<std::uint16_t>(in_report.at(14 + i * 2)
			    ) |
			    (static_cast<std::uint16_t>(
				     in_report.at(15 + i * 2) & 0xffU
			     )
			     << 8U);
		}
	}

	void update_out_report()
	{
		out_report.at(0) = output_state.report_id;

		for (std::size_t i = 0; i < MATRIX_BUTTONS_NUM; i++) {
			// F1 uses 7-bit BRG color
			const auto& [r, g, b] = output_state.matrix_btns.at(i);
			assert(r < MAX_BRIGHTNESS);
			assert(g < MAX_BRIGHTNESS);
			assert(b < MAX_BRIGHTNESS);
			out_report.at(i * 3 + out_offsets::MATRIX) = b;
			out_report.at(i * 3 + 1 + out_offsets::MATRIX) = r;
			out_report.at(i * 3 + 2 + out_offsets::MATRIX) = g;
		}

		for (std::size_t i = 0; i < STOP_BUTTONS_NUM; ++i) {
			const auto& brightness = output_state.stop_btns.at(i);
			assert(brightness < MAX_BRIGHTNESS);
			out_report.at(i * 2 + out_offsets::STOP1) = brightness;
			out_report.at(i * 2 + out_offsets::STOP2) = brightness;
		}

		// ignore first button (has no LED)
		for (std::size_t i = 1; i < SPECIAL_BUTTONS_NUM; ++i) {
			const auto& brightness =
				output_state.special_btns.at(i);
			assert(brightness < MAX_BRIGHTNESS);
			out_report.at(i + out_offsets::SPECIAL) = brightness;
		}

		const auto left_segment =
			static_cast<std::uint8_t>(output_state.segment_left_char
			);
		const auto right_segment = static_cast<std::uint8_t>(
			output_state.segment_right_char
		);
		assert(output_state.segment_left_brightness < MAX_BRIGHTNESS);
		assert(output_state.segment_right_brightness < MAX_BRIGHTNESS);
		// ignore first bit (dot segment)
		for (std::uint8_t i = 1; i < SEGMENTS_PER_DISPLAY; ++i) {
			const Brightness left =
				static_cast<std::uint8_t>(
					(left_segment & (1U << i)) >> i
				) *
				output_state.segment_left_brightness;
			const Brightness right =
				static_cast<std::uint8_t>(
					(right_segment & (1U << i)) >> i
				) *
				output_state.segment_right_brightness;

			out_report.at(out_offsets::SEGMENT_LEFT + i) = left;
			out_report.at(out_offsets::SEGMENT_RIGHT + i) = right;
		}

		assert(output_state.segment_left_dot < MAX_BRIGHTNESS);
		assert(output_state.segment_right_dot < MAX_BRIGHTNESS);
		out_report.at(out_offsets::SEGMENT_LEFT) =
			output_state.segment_left_dot;
		out_report.at(out_offsets::SEGMENT_RIGHT) =
			output_state.segment_right_dot;
	}

	void generate_input_events()
	{
		input_events.clear();

		using etype = InputEvent::EventType;
		using itype = InputEvent::InputType;
		using bevent = InputEvent::ButtonEvent;
		using eevent = InputEvent::EncoderEvent;
		using wevent = InputEvent::WheelEvent;

		for (std::uint8_t i = 0; i < MATRIX_BUTTONS_NUM; ++i) {
			if (input_state.matrix_btns[i] !=
			    old_input_state.matrix_btns[i]) {
				input_events.emplace_back(
					etype::BUTTON,
					itype::MATRIX,
					bevent{i, input_state.matrix_btns[i]}
				);
			}
		}

		for (std::uint8_t i = 0; i < SPECIAL_BUTTONS_NUM; ++i) {
			if (input_state.special_btns[i] !=
			    old_input_state.special_btns[i]) {
				input_events.emplace_back(
					etype::BUTTON,
					itype::SPECIAL,
					bevent{i, input_state.special_btns[i]}
				);
			}
		}

		for (std::uint8_t i = 0; i < STOP_BUTTONS_NUM; ++i) {
			if (input_state.stop_btns[i] !=
			    old_input_state.stop_btns[i]) {
				input_events.emplace_back(
					etype::BUTTON,
					itype::STOP,
					bevent{i, input_state.stop_btns[i]}
				);
			}
		}

		for (std::uint8_t i = 0; i < FADERS_NUM; ++i) {
			if (input_state.faders.at(i) !=
			    old_input_state.faders.at(i)) {
				input_events.emplace_back(
					etype::ENCODER,
					itype::FADER,
					eevent{i, input_state.faders.at(i)}
				);
			}
		}

		for (std::uint8_t i = 0; i < KNOBS_NUM; ++i) {
			if (input_state.knobs.at(i) !=
			    old_input_state.knobs.at(i)) {
				input_events.emplace_back(
					etype::ENCODER,
					itype::KNOB,
					eevent{i, input_state.knobs.at(i)}
				);
			}
		}

		if (input_state.wheel != old_input_state.wheel) {
			const auto diff = static_cast<std::int16_t>(
				input_state.wheel - old_input_state.wheel
			);
			input_events.emplace_back(
				etype::ENCODER,
				itype::WHEEL,
				wevent{static_cast<int8_t>(diff),
				       input_state.wheel}
			);
		}
	}

	HidDevice dev;
	InputState input_state;
	InputState old_input_state;
	OutputState output_state;

	std::array<std::uint8_t, INPUT_REPORT_SIZE> in_report{};
	std::array<std::uint8_t, OUTPUT_REPORT_SIZE> out_report{};
	std::vector<InputEvent> input_events{};
};

F1Device::F1Device(HidDevice&& dev) :
	p_impl(std::make_unique<Impl>(std::move(dev)))
{
}

F1Device::~F1Device() = default;

void F1Device::read_events(const input_event_handler& hdl)
{
	read();
	p_impl->generate_input_events();
	for (const auto& e : p_impl->input_events) {
		hdl(e, *this);
	}
}

const F1Device::InputState& F1Device::read()
{
	p_impl->dev.read(p_impl->in_report.data(), p_impl->in_report.size());
	p_impl->update_in_state();
	return p_impl->input_state;
}

void F1Device::write()
{
	p_impl->update_out_report();
	p_impl->dev.write({p_impl->out_report.data(), p_impl->out_report.size()}
	);
}

F1Device::OutputState& F1Device::output_state() noexcept
{
	assert(p_impl);
	return p_impl->output_state;
}

const F1Device::OutputState& F1Device::output_state() const noexcept
{
	assert(p_impl);
	return p_impl->output_state;
}

const char* F1Device::special_btn_name(std::uint8_t idx)
{
	switch (idx) {
	case SpecialButtons::WHEEL:
		return "WHEEL";
	case SpecialButtons::BROWSE:
		return "BROWSE";
	case SpecialButtons::SIZE:
		return "SIZE";
	case SpecialButtons::TYPE:
		return "TYPE";
	case SpecialButtons::REVERSE:
		return "REVERSE";
	case SpecialButtons::SHIFT:
		return "SHIFT";
	case SpecialButtons::CAPTURE:
		return "CAPTURE";
	case SpecialButtons::QUANT:
		return "QUANT";
	case SpecialButtons::SYNC:
		return "SYNC";
	default:
		return "UNKNOWN";
	}
}

std::pair<F1Device::SegmentChar, F1Device::SegmentChar>
F1Device::num_to_segments(std::uint8_t num)
{
	if (num >= MAX_SEGMENT_NUM) {
		const std::string msg = "num = " + std::to_string(num) +
					" >= MAX_SEGMENT_NUM = " +
					std::to_string(MAX_SEGMENT_NUM);
		throw std::out_of_range{msg};
	}
	const std::uint8_t hi_digit = num / 10;
	const std::uint8_t lo_digit = num - (hi_digit * 10);
	return {digit_to_segment(hi_digit), digit_to_segment(lo_digit)};
}

namespace
{
F1Device::SegmentChar digit_to_segment(std::uint8_t digit)
{
	// NOLINTBEGIN(*-magic-numbers)
	switch (digit) {
	case 0:
		return F1Device::SegmentChar::D0;
	case 1:
		return F1Device::SegmentChar::D1;
	case 2:
		return F1Device::SegmentChar::D2;
	case 3:
		return F1Device::SegmentChar::D3;
	case 4:
		return F1Device::SegmentChar::D4;
	case 5:
		return F1Device::SegmentChar::D5;
	case 6:
		return F1Device::SegmentChar::D6;
	case 7:
		return F1Device::SegmentChar::D7;
	case 8:
		return F1Device::SegmentChar::D8;
	case 9:
		return F1Device::SegmentChar::D9;
	default:
		assert(false);
	}
	// NOLINTEND(*-magic-numbers)
}
} // namespace
