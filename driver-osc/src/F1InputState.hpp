/**
 * @file
 *
 * @brief Classes and utilities for handling input data from {@link F1HidDev}.
 */
#pragma once

#include <array>
#include <bitset>
#include <cstdint>
#include <vector>

enum class F1InputButtonIdx : uint8_t
{
	// TODO: Indices of the input buttons
};

/**
 * Representation of the input as received by the Traktor Kontrol F1
 *
 * This struct provides the input as it is received from the F1 in a prepared
 * form (i.e. correct endianness, bitsets instead of integers, correct names
 * instead of offsets).
 *
 * Members in this class represent the buttons which conform to their names,
 * i.e. matrix represents all buttons of the button matrix, stop represents all
 * stop buttons, etc.
 *
 * **Note on special buttons:** Special buttons are all buttons which occur only
 * once on the hardware and/or have a special meaning.
 * This affects the middle two rows of buttons (Sync, Quant, Shift, Reverse,
 * etc.). Each of these buttons has a meaning of its own, mainly independent of
 * the rest of the hardware. To avoid further processing complexity in the
 * backend, all of these buttons are represented using a single array.
 */
struct F1InpuState
{
	uint8_t report_id;
	std::bitset<16> matrix_btns;
	std::bitset<9> special_btns;
	std::bitset<4> stop_btns;
	uint8_t wheel;
	std::array<uint16_t, 4> knobs;
	std::array<uint16_t, 4> faders;
};

/**
 * Represenation of a change of states
 *
 * This struct represents an input change and contains only the changed inputs.
 * Any unchanged values should be left out.
 * The only exception to this is wheel, where the change of input means
 * by wheel_diff != 0.
 */
struct F1InputChange
{
	struct btn_diff
	{
		std::vector<uint8_t> matrix;
		std::vector<uint8_t> stop;
		std::vector<uint8_t> special;
	};

	btn_diff pressed_buttons;
	btn_diff released_buttons;

	int8_t wheel_diff;
	uint8_t wheel;

	std::vector<std::pair<uint8_t, uint16_t>> knobs;
	std::vector<std::pair<uint8_t, uint16_t>> faders;
};

/**
 * Create an input diff from two raw states
 *
 * This operator can be used to create a diff of two {@link F1InpuState} states.
 * The diff contains only inputs which really have changed. For Details and
 * Exceptions, see {@link F1InputChange}.
 */
F1InputChange operator-(const F1InpuState& new_state,
                        const F1InpuState& old_state);

/**
 * Default values and ranges of the raw input.
 */
namespace F1Default
{
constexpr float KNOB_MAX{0xfff};
constexpr float KNOB_MIN{0x0};
constexpr float FADER_MAX = KNOB_MAX;
constexpr float FADER_MIN = KNOB_MIN;
} // namespace F1Default
