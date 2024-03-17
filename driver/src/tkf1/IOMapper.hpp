#pragma once

#include <array>
#include <bitset>
#include <optional>

#include "io/MidiEvent.hpp"
#include "tkf1/F1Device.hpp"

/**
 * Mapper between MIDI and HID
 *
 * This class is the glue between the MIDI and HID parts of the driver, linking
 * MIDI events to suitable changes to the HID output state and generating MIDI
 * events from HID input events.
 */
class IOMapper final
{
public:
	using byte = MidiEvent::byte;

	constexpr static F1Device::ButtonColor WHITE = F1Device::WHITE;

	IOMapper() = default;
	IOMapper(const IOMapper&) = default;
	IOMapper& operator=(const IOMapper&) = default;
	IOMapper(IOMapper&&) = default;
	IOMapper& operator=(IOMapper&&) = default;
	~IOMapper() = default;

	enum BrightnessMode {
		MIDI_NOTE,
		MIDI_CC,
		HID,
	};

	/**
	 * Process a HID input event
	 *
	 * This returns the MIDI event that should be emitted as a result of the
	 * input event. If no event should be emitted, the optional is empty.
	 */
	std::optional<MidiEvent> process_HID_input(
		const F1Device::InputEvent& event,
		F1Device::OutputState& output_state
	);
	/**
	 * Process a MIDI input event
	 *
	 * This processes the MIDI event and optionally updates the HID output
	 * state. The returned boolean indicates whether a change to the output
	 * state has occured.
	 */
	bool process_MIDI_event(
		const MidiEvent& event, F1Device::OutputState& output_state
	);

	// NOLINTBEGIN(*-magic-numbers,*-private-member-variables-in-classes)
	/**
	 * @name Mapper configuration
	 *
	 * These structs can be freely modified to configure the drivers
	 * behavior.
	 *
	 * @{
	 */
	/**
	 * Control which buttons are toggle buttons
	 */
	struct {
		std::bitset<F1Device::MATRIX_BUTTONS_NUM> matrix{};
		std::bitset<F1Device::SPECIAL_BUTTONS_NUM> special{};
		std::bitset<F1Device::STOP_BUTTONS_NUM> stop{};
	} button_toggle;
	/**
	 * Configuration of note values to emit for buttons
	 */
	struct {
		std::array<byte, F1Device::MATRIX_BUTTONS_NUM> matrix{
			MidiEvent::note_from_name_c("C1"),
			MidiEvent::note_from_name_c("C#1"),
			MidiEvent::note_from_name_c("D1"),
			MidiEvent::note_from_name_c("D#1"),
			MidiEvent::note_from_name_c("E1"),
			MidiEvent::note_from_name_c("F1"),
			MidiEvent::note_from_name_c("F#1"),
			MidiEvent::note_from_name_c("G1"),
			MidiEvent::note_from_name_c("G#1"),
			MidiEvent::note_from_name_c("A1"),
			MidiEvent::note_from_name_c("A#1"),
			MidiEvent::note_from_name_c("B1"),
			MidiEvent::note_from_name_c("C2"),
			MidiEvent::note_from_name_c("C#2"),
			MidiEvent::note_from_name_c("D2"),
			MidiEvent::note_from_name_c("D#2"),
		};
		std::array<byte, F1Device::SPECIAL_BUTTONS_NUM> special{
			MidiEvent::note_from_name_c("E2"),
			MidiEvent::note_from_name_c("F2"),
			MidiEvent::note_from_name_c("F#2"),
			MidiEvent::note_from_name_c("G2"),
			MidiEvent::note_from_name_c("G#2"),
			MidiEvent::note_from_name_c("A2"),
			MidiEvent::note_from_name_c("A#2"),
			MidiEvent::note_from_name_c("B2"),
			MidiEvent::note_from_name_c("C3"),
		};
		std::array<byte, F1Device::STOP_BUTTONS_NUM> stop{
			MidiEvent::note_from_name_c("C#3"),
			MidiEvent::note_from_name_c("D3"),
			MidiEvent::note_from_name_c("D#3"),
			MidiEvent::note_from_name_c("E3"),
		};
	} notes;
	/**
	 * Configuration of controller values to emit for encoders
	 *
	 * Two controllers are exposed for the wheel, one emitting an absolute
	 * value, the other emitting only changes in the value.
	 */
	struct {
		std::array<byte, F1Device::KNOBS_NUM> knobs{75, 76, 77, 78};
		std::array<byte, F1Device::FADERS_NUM> faders{79, 80, 81, 82};
		byte wheel_absolute{83};
		byte wheel_diff{84};
	} controllers;

	/**
	 * Control button brightness mode
	 *
	 * If a button's mode is MIDI_NOTE, it is controlled by a MIDI event
	 * with the same note number as the event emitted when pressing the
	 * corresponding button. If the button's mode is HID, it is solely
	 * controlled by the HID input, i.e. pressing the button uses high
	 * brightness, while releasing it uses low brightness.
	 *
	 * The MIDI_CC mode allows for fine grained control of the button
	 * brightess with MIDI CC events. The corresponding CC numbers can be
	 * configured in brightness_controllers.
	 */
	struct {
		std::array<BrightnessMode, F1Device::MATRIX_BUTTONS_NUM> matrix{
			BrightnessMode::HID,
			BrightnessMode::HID,
			BrightnessMode::HID,
			BrightnessMode::HID,
			BrightnessMode::HID,
			BrightnessMode::HID,
			BrightnessMode::HID,
			BrightnessMode::HID,
			BrightnessMode::HID,
			BrightnessMode::HID,
			BrightnessMode::HID,
			BrightnessMode::HID,
			BrightnessMode::HID,
			BrightnessMode::HID,
			BrightnessMode::HID,
			BrightnessMode::HID,
		};
		std::array<BrightnessMode, F1Device::SPECIAL_BUTTONS_NUM>
			special{
				BrightnessMode::HID,
				BrightnessMode::HID,
				BrightnessMode::HID,
				BrightnessMode::HID,
				BrightnessMode::HID,
				BrightnessMode::HID,
				BrightnessMode::HID,
				BrightnessMode::HID,
				BrightnessMode::HID,
			};
		std::array<BrightnessMode, F1Device::STOP_BUTTONS_NUM> stop{
			BrightnessMode::HID,
			BrightnessMode::HID,
			BrightnessMode::HID,
			BrightnessMode::HID,
		};
	} brightness_mode;
	/**
	 * Set brightness controllers
	 *
	 * If, for any button, its brightness mode is MIDI_CC, the controller
	 * number that should control the brightness is set in this struct.
	 */
	struct {
		std::array<byte, F1Device::MATRIX_BUTTONS_NUM> matrix{
			// clang-format off
			70, 71, 72, 73,
			74, 75, 76, 77,
			78, 79, 80, 81,
			82, 83, 84, 85,
			// clang-format on
		};
		std::array<byte, F1Device::SPECIAL_BUTTONS_NUM> special{
			86, 87, 88, 89, 90, 91, 92, 93, 94
		};
		std::array<byte, F1Device::STOP_BUTTONS_NUM> stop{
			102, 103, 104, 105
		};
	} brightness_controllers;

	/**
	 * Colors of the matrix buttons
	 *
	 * This color is multiplied with the brightness factor to obtain the
	 * actual color value.
	 */
	std::array<F1Device::ButtonColor, F1Device::MATRIX_BUTTONS_NUM>
		matrix_colors{
			// clang-format off
			WHITE, WHITE, WHITE, WHITE,
			WHITE, WHITE, WHITE, WHITE,
			WHITE, WHITE, WHITE, WHITE,
			WHITE, WHITE, WHITE, WHITE,
			// clang-format on
		};

	float brightness_low_color{0.2F};
	float brightness_high_color{1.F};
	F1Device::Brightness brightness_low{0x29};
	F1Device::Brightness brightness_high{0x7f};

	byte channel{0};
	byte note_on_velocity{127};
	byte note_off_velocity{0};
	// NOLINTEND(*-magic-numbers,*-private-member-variables-in-classes)

private:
	template <
		F1Device::InputEvent::EventType,
		F1Device::InputEvent::InputType>
	std::optional<MidiEvent> process_HID_input_impl(
		const F1Device::InputEvent& event, F1Device::OutputState& output
	);

	template <std::size_t btn_size>
	std::pair<std::optional<MidiEvent>, bool> process_HID_input_button(
		const F1Device::InputEvent::ButtonEvent& button,
		const std::bitset<btn_size>& button_toggle,
		const std::array<byte, btn_size>& notes,
		std::bitset<btn_size>& last_input
	);

	void button_light_matrix_HID(
		F1Device::OutputState& output, byte idx, bool on
	);
	void button_light_special_HID(
		F1Device::OutputState& output, byte idx, bool on
	);
	void
	button_light_stop_HID(F1Device::OutputState& output, byte idx, bool on);

	struct {
		std::bitset<F1Device::MATRIX_BUTTONS_NUM> matrix{};
		std::bitset<F1Device::SPECIAL_BUTTONS_NUM> special{};
		std::bitset<F1Device::STOP_BUTTONS_NUM> stop{};
	} last_input;
};
