#include "F1InputState.hpp"

F1InputChange operator-(const F1InpuState& new_state,
                        const F1InpuState& old_state)
{
	constexpr static bool OFF{false};
	constexpr static bool ON{true};

	F1InputChange diff;

	for (size_t i = 0; i < new_state.matrix_btns.size(); ++i) {
		auto old_btn_state = old_state.matrix_btns[i];
		auto new_btn_state = new_state.matrix_btns[i];
		if (old_btn_state == OFF && new_btn_state == ON) {
			diff.pressed_buttons.matrix.push_back(i);
		} else if (old_btn_state == ON && new_btn_state == OFF) {
			diff.released_buttons.matrix.push_back(i);
		}
	}

	for (size_t i = 0; i < new_state.special_btns.size(); ++i) {
		auto old_btn_state = old_state.special_btns[i];
		auto new_btn_state = new_state.special_btns[i];
		if (old_btn_state == OFF && new_btn_state == ON) {
			diff.pressed_buttons.special.push_back(i);
		} else if (old_btn_state == ON && new_btn_state == OFF) {
			diff.released_buttons.special.push_back(i);
		}
	}

	for (size_t i = 0; i < new_state.stop_btns.size(); ++i) {
		auto old_btn_state = old_state.stop_btns[i];
		auto new_btn_state = new_state.stop_btns[i];
		if (old_btn_state == OFF && new_btn_state == ON) {
			diff.pressed_buttons.stop.push_back(i);
		} else if (old_btn_state == ON && new_btn_state == OFF) {
			diff.released_buttons.stop.push_back(i);
		}
	}

	diff.wheel_diff = new_state.wheel - old_state.wheel;

	diff.wheel = new_state.wheel;

	for (size_t i = 0; i < new_state.knobs.size(); ++i) {
		auto old_knob_state = old_state.knobs.at(i);
		auto new_knob_state = new_state.knobs.at(i);
		if (new_knob_state != old_knob_state) {
			diff.knobs.push_back({i, new_knob_state});
		}
	}

	for (size_t i = 0; i < new_state.faders.size(); ++i) {
		auto old_fader_state = old_state.faders.at(i);
		auto new_fader_state = new_state.faders.at(i);
		if (new_fader_state != old_fader_state) {
			diff.faders.push_back({i, new_fader_state});
		}
	}

	return diff;
}
