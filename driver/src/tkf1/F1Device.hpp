#pragma once

#include <array>
#include <bitset>
#include <cstddef>
#include <functional>
#include <memory>
#include <tuple>
#include <variant>

#include <linux/hidraw.h>

class HidDevice;

/**
 * Traktor Kontrol F1 device
 */
class F1Device
{
	class Impl;

public:
	using Brightness = std::uint8_t;
	using ButtonColor = std::tuple<Brightness, Brightness, Brightness>;

	struct SpecialButtons {
		constexpr static std::uint8_t WHEEL = 0;
		constexpr static std::uint8_t BROWSE = 1;
		constexpr static std::uint8_t SIZE = 2;
		constexpr static std::uint8_t TYPE = 3;
		constexpr static std::uint8_t REVERSE = 4;
		constexpr static std::uint8_t SHIFT = 5;
		constexpr static std::uint8_t CAPTURE = 6;
		constexpr static std::uint8_t QUANT = 7;
		constexpr static std::uint8_t SYNC = 8;

		SpecialButtons() = delete;
	};

	enum class SegmentChar : std::uint8_t {
		NONE = 0b0000'0000,
		UNDERLINE = 0b1000'0000,
		MIDLINE = 0b0000'0010,
		TOPLINE = 0b0001'0000,
		D0 = 0b1111'1100,
		D1 = 0b0000'1100,
		D2 = 0b1101'1010,
		D3 = 0b1001'1110,
		D4 = 0b0010'1110,
		D5 = 0b1011'0110,
		D6 = 0b1111'0110,
		D7 = 0b0001'1100,
		D8 = 0b1111'1110,
		D9 = 0b0011'1110,
		A = 0b0111'1110,
		C = 0b1111'0000,
		E = 0b1111'0010,
		F = 0b0111'0010,
		H = 0b0110'1110,
		L = 0b1110'0000,
		O = 0b1111'1100,
		P = 0b1111'1010,
		S = 0b1011'0110,
		U = 0b1110'1100,
	};

	constexpr static std::uint8_t OUTPUT_REPORT_ID{0x80};
	constexpr static std::uint8_t INPUT_REPORT_ID{0x01};
	constexpr static std::size_t OUTPUT_REPORT_SIZE{81};
	constexpr static std::size_t INPUT_REPORT_SIZE{22};
	constexpr static std::size_t MATRIX_BUTTONS_NUM{16};
	constexpr static std::size_t SPECIAL_BUTTONS_NUM{9};
	constexpr static std::size_t STOP_BUTTONS_NUM{4};
	constexpr static std::size_t KNOBS_NUM{4};
	constexpr static std::size_t FADERS_NUM{4};
	constexpr static std::size_t SEGMENTS_PER_DISPLAY{8};

	constexpr static std::uint8_t WHEEL_MAX{0xffU};
	constexpr static std::uint16_t KNOBS_MAX{0xfffU};
	constexpr static std::uint16_t FADERS_MAX{0xfffU};

	constexpr static std::uint8_t MAX_SEGMENT_NUM{100};
	constexpr static Brightness MAX_BRIGHTNESS{128};
	constexpr static Brightness DARK{0x00};
	constexpr static Brightness FULL_BRIGHTNESS{0x7f};
	constexpr static ButtonColor BLACK{DARK, DARK, DARK};
	constexpr static ButtonColor WHITE{
		FULL_BRIGHTNESS, FULL_BRIGHTNESS, FULL_BRIGHTNESS
	};
	constexpr static int16_t HID_VENDOR_ID{0x17cc};
	constexpr static int16_t HID_PRODUCT_ID{0x1120};

	struct InputState { // NOLINT(*-member-init)
		// NOLINTBEGIN(*-magic-numbers)
		std::uint8_t report_id;
		std::bitset<MATRIX_BUTTONS_NUM> matrix_btns;
		std::bitset<SPECIAL_BUTTONS_NUM> special_btns;
		std::bitset<STOP_BUTTONS_NUM> stop_btns;
		std::uint8_t wheel;
		std::array<std::uint16_t, KNOBS_NUM> knobs;
		std::array<std::uint16_t, FADERS_NUM> faders;
		// NOLINTEND(*-magic-numbers)
	};

	/**
	 * Representor for an input event
	 *
	 * Each input event is of event type BUTTON (some form of button
	 * press/release) or ENCODER (i.e. fader, knob, wheel movement).
	 * Depending on the event type, data contains the corresponding event
struct.
	 * The input type provides additional information about the data.
	 *
	 * For button events, the associated ButtonEvent contains the button
index
	 * (from the button group indicated by input_type) and whether
	 * it was pressed or release.
	 * For encoder events, the EncoderEvent struct contains the index of the
encoder
	 * (from the encoder group indicated by input_type) and the absolute
value/position of the encoder.
	 * One exception to this rule is the change of the wheel:
	 * Instead of a EncoderEvent struct, a WheelEvent struct is stored,
	 * containing the value of the encoder and the direction the encoder
moved
	 * (i.e. the difference to the previous value). Positive direction means
clockwise rotation, while negative direction means counterclockwise rotation.
	 */
	struct InputEvent {
		enum class EventType {
			BUTTON,
			ENCODER,
		};

		enum class InputType {
			MATRIX,
			STOP,
			SPECIAL,
			KNOB,
			FADER,
			WHEEL,
		};

		struct ButtonEvent {
			std::uint8_t index{0};
			bool button_press{false};
		};

		struct EncoderEvent {
			std::uint8_t index{0};
			std::uint16_t value{0};
		};

		struct WheelEvent {
			std::int8_t direction{0};
			std::uint8_t value{0};
		};

		EventType event_type{EventType::BUTTON};
		InputType input_type{InputType::MATRIX};
		std::variant<ButtonEvent, EncoderEvent, WheelEvent> data;
	};

	struct OutputState {
		// NOLINTBEGIN(*-magic-numbers)
		std::uint8_t report_id{OUTPUT_REPORT_ID};
		std::array<ButtonColor, MATRIX_BUTTONS_NUM> matrix_btns{
			// clang-format off
			BLACK, BLACK, BLACK, BLACK,
			BLACK, BLACK, BLACK, BLACK,
			BLACK, BLACK, BLACK, BLACK,
			BLACK, BLACK, BLACK, BLACK,
			// clang-format on
		};
		std::array<Brightness, STOP_BUTTONS_NUM> stop_btns{
			DARK, DARK, DARK, DARK
		};
		std::array<Brightness, SPECIAL_BUTTONS_NUM> special_btns{
			DARK, DARK, DARK, DARK, DARK, DARK, DARK, DARK, DARK
		};
		SegmentChar segment_left_char{SegmentChar::NONE};
		SegmentChar segment_right_char{SegmentChar::NONE};
		Brightness segment_left_dot{DARK};
		Brightness segment_right_dot{DARK};
		Brightness segment_left_brightness{DARK};
		Brightness segment_right_brightness{DARK};
		// NOLINTEND(*-magic-numbers)
	};

	using input_event_handler =
		std::function<void(const InputEvent&, F1Device&)>;

	explicit F1Device(HidDevice&& dev);
	F1Device(const F1Device&) = delete;
	F1Device& operator=(const F1Device&) = delete;
	F1Device(F1Device&&) noexcept;
	F1Device& operator=(F1Device&&) noexcept;
	~F1Device();

	/**
	 * Check whether a HidDevice is a Traktor Kontrol F1 device
	 *
	 * This can be used before construction of F1Device objects to check if
	 * the HID descriptor is actually of a Traktor Kontrol F1 device.
	 */
	constexpr static bool is_f1_device(const struct hidraw_devinfo& dev)
	{
		return dev.product == HID_PRODUCT_ID &&
		       dev.vendor == HID_VENDOR_ID;
	}

	/**
	 * Wait for and process input events
	 *
	 * This function waits for a change in the input state and calls the
	 * given input event handler function once for each change in the input
	 * state, providing a corresponding InputEvent.
	 */
	void read_events(const input_event_handler& hdl);
	/**
	 * Wait for and emit a input event
	 *
	 * This function reads from the HID device file descriptor and returns
	 * the parsed input report. This is also used to initialize the internal
	 * state of this object to be able to process input state changes later
	 * on.
	 *
	 * The preferred method to handle changes in the input is to use
	 * read_events() instead.
	 */
	const InputState& read();

	/**
	 * Send the current output state to the device
	 *
	 * This function is needed to apply changes made to the output state.
	 * It sends a complete HID output report to the device,
	 * containing the full output state.
	 * It should be called after changes have been made to the output state
	 * obtained by output_state().
	 */
	void write();

	/**
	 * Obtain a reference to the internal output state
	 *
	 * The internal output state may be modified as desired. To apply the
	 * changes, call write() to send an output report to the HID device.
	 */
	OutputState& output_state() noexcept;
	[[nodiscard]] const OutputState& output_state() const noexcept;

	/**
	 * Get the matrix button index of the given x/y coordinates
	 *
	 * This is just a convenience function to be able to index matrix
buttons
	 * by x/y coordinates instead of the button index. The coordinate system
considers the top left button as (0, 0) and the bottom right button as (3, 3).
	 */
	constexpr static std::uint8_t matrix_pos(std::uint8_t x, std::uint8_t y)
	{
		return y * 4 + x;
	}
	/**
	 * Get the x/y coordinates corresponding to a button index
	 *
	 * This is the inverse of the x/y to index conversion function,
	 * calculating the correct x/y coordinate from the given matrix button
	 * index.
	 */
	constexpr static std::pair<std::uint8_t, std::uint8_t>
		matrix_pos(std::uint8_t);

	/**
	 * Obtain a human-readable name of a special button index
	 */
	static const char* special_btn_name(std::uint8_t idx);

	/**
	 * Convert a number to its 7-segment coding
	 */
	static std::pair<SegmentChar, SegmentChar>
	num_to_segments(std::uint8_t num);

	/**
	 * Convert 7-bit RGB values to the internal representation
	 */
	constexpr static ButtonColor
	rgb2color(Brightness r, Brightness g, Brightness b)
	{
		return std::make_tuple(b, r, g);
	}

private:
	std::unique_ptr<Impl> p_impl;
};
