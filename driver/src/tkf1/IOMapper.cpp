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
		F1Device::OutputState& output                                  \
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

std::optional<MidiEvent> IOMapper::process_HID_input(
	const F1Device::InputEvent& event, F1Device::OutputState& output
)
{
	PROCESS_HID_INPUT(event, output, EventType::BUTTON, InputType::MATRIX);
	PROCESS_HID_INPUT(event, output, EventType::BUTTON, InputType::SPECIAL);
	PROCESS_HID_INPUT(event, output, EventType::BUTTON, InputType::STOP);

	// PROCESS_HID_INPUT(event, output, EventType::ENCODER,
	// InputType::FADER);

	// PROCESS_HID_INPUT(event, output, EventType::ENCODER,
	// InputType::KNOB);

	// PROCESS_HID_INPUT(event, output, EventType::ENCODER,
	// InputType::WHEEL);

	// Really, we should never get here, but just in case, do
	// nothing in prod
	// assert(false);
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
