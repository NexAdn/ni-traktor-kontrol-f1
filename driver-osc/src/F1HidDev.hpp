#pragma once

#include <exception>
#include <ostream>

#include "F1InputState.hpp"

class F1HidDev
{
public:
	using input = F1InpuState;

	struct output
	{
		constexpr static uint8_t report_id{0x80};
		std::array<std::array<uint8_t, 3>, 16> matrix_btns;
	};

	constexpr F1HidDev(int fd) : fd(fd)
	{}
	constexpr F1HidDev(const F1HidDev&) = default;
	~F1HidDev() = default;

	const input& read();
	inline const input& read_cached() const
	{
		return input_state;
	}

	void write(const output& out) const;

	constexpr static std::array<uint8_t, 3> rgb2color(uint8_t r, uint8_t g,
	                                                  uint8_t b)
	{
		return {b, r, g};
	}

private:
	int fd;
	input input_state{};
};

class F1HidDevException : public std::exception
{
public:
	inline const char* what() const noexcept
	{
		return "F1HidDevException";
	}
};
