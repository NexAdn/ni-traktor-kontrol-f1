#pragma once

#include <array>
#include <bitset>
#include <cstdint>
#include <vector>

enum class F1InputButtonIdx : uint8_t
{
	// TODO: Indices of the input buttons
};

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

F1InputChange operator-(const F1InpuState& new_state,
                        const F1InpuState& old_state);
