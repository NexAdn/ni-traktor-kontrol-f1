#include <unistd.h>

#include "F1HidDev.hpp"

namespace
{
constexpr const size_t INPUT_BUF_SIZE{22};
}

const F1HidDev::input& F1HidDev::read()
{
	uint8_t raw_input[INPUT_BUF_SIZE];
	ssize_t res = ::read(fd, raw_input, sizeof(raw_input));

	if (res < 0) {
		throw F1HidDevException{};
	}

	input_state.report_id = raw_input[0];

	for (size_t i = 0; i < 8; i++) {
		input_state.matrix_btns[i] = (1 << (7 - i)) & raw_input[1];
	}
	for (size_t i = 0; i < 8; i++) {
		input_state.matrix_btns[i + 8] = (1 << (7 - i)) & raw_input[2];
	}
	for (size_t i = 2; i < 8; i++) {
		input_state.special_btns[i - 2] = (1 << i) & raw_input[3];
	}
	for (size_t i = 1; i < 4; i++) {
		input_state.special_btns[i + 5] = (1 << i) & raw_input[4];
	}
	for (size_t i = 0; i < 4; i++) {
		input_state.stop_btns[i] = (1 << (4 + (3 - i))) & raw_input[4];
	}
	input_state.wheel = raw_input[5];
	for (size_t i = 0; i < 4; i++) {
		input_state.knobs[i] =
		  static_cast<uint16_t>(raw_input[6 + i * 2])
		  | (static_cast<uint16_t>(raw_input[7 + i * 2]) << 8);
	}
	for (size_t i = 0; i < 4; i++) {
		input_state.faders[i] =
		  raw_input[14 + i * 2]
		  | (static_cast<uint16_t>(raw_input[15 + i * 2] & 0xff) << 8);
	}

	return read_cached();
}

void F1HidDev::write(const output& output) const
{
	std::array<uint8_t, 81> data;
	data.at(0) = 0x80;

	for (size_t i = 1; i < 81; i++)
		data.at(i) = 0x0;

	for (size_t i = 0; i < 16; i++) {
		for (size_t j = 0; j < 3; j++) {
			data.at(i * 3 + j + 0x19) = output.matrix_btns.at(i).at(j);
		}
	}

	ssize_t res = ::write(fd, data.data(), data.size());

	if (res != data.size())
		throw F1HidDevException{};
}
