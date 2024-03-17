#include <cmath>
#include <optional>

#include <iostream>

#include "IOMapper.hpp"
#include "io/MidiEvent.hpp"
#include "tkf1/F1Device.hpp"

using Brightness = F1Device::Brightness;
using EventType = F1Device::InputEvent::EventType;
using InputType = F1Device::InputEvent::InputType;
using ButtonEvent = F1Device::InputEvent::ButtonEvent;
using EncoderEvent = F1Device::InputEvent::EncoderEvent;
using WheelEvent = F1Device::InputEvent::WheelEvent;

namespace
{
template <typename OutType, typename InType>
constexpr OutType
// NOLINTNEXTLINE(*-easily-swappable-parameters)
scale(const InType& value, const InType& in_max, const OutType& out_max)
{
	const double scale_factor =
		static_cast<double>(out_max) / static_cast<double>(in_max);
	return static_cast<OutType>(std::roundl(value * scale_factor));
}
} // namespace

// NOLINTNEXTLINE(*-macro-usage)
#define PROCESS_HID_INPUT(event, output, evt, ipt)                             \
	{                                                                      \
		if ((event).event_type == (evt) &&                             \
		    (event).input_type == (ipt))                               \
			return process_HID_input_impl<(evt), (ipt)>(           \
				(event), (output)                              \
			);                                                     \
	}

// NOLINTNEXTLINE(*-macro-usage)
#define PROCESS_HID_INPUT_IMPL(evt, ipt)                                       \
	template <>                                                            \
	std::optional<MidiEvent> IOMapper::process_HID_input_impl<evt, ipt>(   \
		const F1Device::InputEvent& event,                             \
		[[maybe_unused]] F1Device::OutputState& output                 \
	)

PROCESS_HID_INPUT_IMPL(EventType::BUTTON, InputType::MATRIX)
{
	const auto& button = std::get<ButtonEvent>(event.data);
	auto [midi_event, button_on] = process_HID_input_button(
		button, button_toggle.matrix, notes.matrix, last_input.matrix
	);
	if (midi_event) {
		button_light_matrix_HID(output, button.index, button_on);
	}

	return midi_event;
}

PROCESS_HID_INPUT_IMPL(EventType::BUTTON, InputType::SPECIAL)
{
	const auto& button = std::get<ButtonEvent>(event.data);
	auto [midi_event, button_on] = process_HID_input_button(
		button, button_toggle.special, notes.special, last_input.special
	);
	if (midi_event) {
		std::cout << "button: " << (button_on ? "on" : "off") << '\n';
		button_light_special_HID(output, button.index, button_on);
	}

	return midi_event;
}

PROCESS_HID_INPUT_IMPL(EventType::BUTTON, InputType::STOP)
{
	const auto& button = std::get<ButtonEvent>(event.data);
	auto [midi_event, button_on] = process_HID_input_button(
		button, button_toggle.stop, notes.stop, last_input.stop
	);
	if (midi_event) {
		std::cout << "button: " << (button_on ? "on" : "off") << '\n';
		button_light_stop_HID(output, button.index, button_on);
	}

	return midi_event;
}

PROCESS_HID_INPUT_IMPL(EventType::ENCODER, InputType::FADER)
{
	const auto& encoder = std::get<EncoderEvent>(event.data);
	return process_HID_input_encoder(
		encoder, controllers.faders, last_input.fader
	);
}

PROCESS_HID_INPUT_IMPL(EventType::ENCODER, InputType::KNOB)
{
	const auto& encoder = std::get<EncoderEvent>(event.data);
	return process_HID_input_encoder(
		encoder, controllers.knobs, last_input.knob
	);
}

PROCESS_HID_INPUT_IMPL(EventType::ENCODER, InputType::WHEEL)
{
	const auto& wheel = std::get<WheelEvent>(event.data);
	return MidiEvent{
		MidiEvent::Type::CONTROL_CHANGE,
		channel,
		controllers.wheel,
		wheel.direction > 0 ? wheel_inc_value : wheel_dec_value
	};
}

std::optional<MidiEvent> IOMapper::process_HID_input(
	const F1Device::InputEvent& event, F1Device::OutputState& output
)
{
	PROCESS_HID_INPUT(event, output, EventType::BUTTON, InputType::MATRIX);
	PROCESS_HID_INPUT(event, output, EventType::BUTTON, InputType::SPECIAL);
	PROCESS_HID_INPUT(event, output, EventType::BUTTON, InputType::STOP);

	PROCESS_HID_INPUT(event, output, EventType::ENCODER, InputType::FADER);
	PROCESS_HID_INPUT(event, output, EventType::ENCODER, InputType::KNOB);

	PROCESS_HID_INPUT(event, output, EventType::ENCODER, InputType::WHEEL);

	return {};
}

bool IOMapper::process_MIDI_event(
	[[maybe_unused]] const MidiEvent& event,
	[[maybe_unused]] F1Device::OutputState& output_state
)
{
	return false;
}

template <std::size_t btn_size>
std::pair<std::optional<MidiEvent>, bool> IOMapper::process_HID_input_button(
	const ButtonEvent& button,
	const std::bitset<btn_size>& button_toggle,
	const std::array<byte, btn_size>& notes,
	std::bitset<btn_size>& last_input
)
{
	const byte idx = button.index;

	// toggle
	if (button_toggle[idx] and button.button_press) {
		const MidiEvent::Type event_type =
			last_input[idx] ? MidiEvent::Type::NOTE_OFF
					: MidiEvent::Type::NOTE_ON;
		last_input[idx] = not last_input[idx];

		return {MidiEvent{
				event_type,
				channel,
				notes.at(idx),
				event_type == MidiEvent::Type::NOTE_ON
					? note_on_velocity
					: note_off_velocity
			},
			last_input[idx]};
	}

	if (button_toggle[idx] and not button.button_press) {
		return {};
	}

	// no toggle
	const MidiEvent::Type event_type = button.button_press
						   ? MidiEvent::Type::NOTE_ON
						   : MidiEvent::Type::NOTE_OFF;
	last_input[idx] = button.button_press;
	return {MidiEvent{
			event_type,
			channel,
			notes.at(idx),
			event_type == MidiEvent::Type::NOTE_ON
				? note_on_velocity
				: note_off_velocity
		},
		button.button_press};
}

template <std::size_t enc_size>
std::optional<MidiEvent> IOMapper::process_HID_input_encoder(
	const EncoderEvent& encoder,
	const std::array<byte, enc_size>& controllers,
	std::array<byte, F1Device::FADERS_NUM>& last_input
)
{
	const byte idx = encoder.index;
	const byte val =
		scale<byte>(encoder.value, F1Device::FADERS_MAX, ENCODER_MAX);

	if (val == last_input.at(idx))
		return {};

	last_input.at(idx) = val;
	return MidiEvent{
		MidiEvent::Type::CONTROL_CHANGE,
		channel,
		controllers.at(idx),
		val
	};
}

void IOMapper::button_light_matrix_HID(
	F1Device::OutputState& output, byte idx, bool on
)
{
	if (brightness_mode.matrix.at(idx) != BrightnessMode::HID)
		return;

	const double brightness =
		on ? brightness_high_color : brightness_low_color;
	const auto& [orig_b, orig_r, orig_g] = matrix_colors.at(idx);
	const Brightness r = std::lround(brightness * orig_r);
	const Brightness g = std::lround(brightness * orig_g);
	const Brightness b = std::lround(brightness * orig_b);
	const auto color = F1Device::rgb2color(r, g, b);
	output.matrix_btns.at(idx) = color;
}

void IOMapper::button_light_special_HID(
	F1Device::OutputState& output, byte idx, bool on
)
{
	if (brightness_mode.special.at(idx) != BrightnessMode::HID)
		return;

	const Brightness brightness = on ? brightness_high : brightness_low;
	output.special_btns.at(idx) = brightness;
}

void IOMapper::button_light_stop_HID(
	F1Device::OutputState& output, byte idx, bool on
)
{
	if (brightness_mode.stop.at(idx) != BrightnessMode::HID)
		return;

	const Brightness brightness = on ? brightness_high : brightness_low;
	output.stop_btns.at(idx) = brightness;
}
