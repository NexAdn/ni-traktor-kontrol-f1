#pragma once

#include <ostream>
#include <stdexcept>

#include "F1InputState.hpp"

/**
 * Representation of a Traktor Kontrol F1 HID device
 *
 * This class contains everything necessary to interface with a Traktor Kontrol
 * F1. Methods are provided to set outputs and wait for inputs, but the file
 * descriptor to the raw HID device needs to be provided beforehand, but the
 * file descriptor to the raw HID device needs to be provided beforehand.
 */
class F1HidDev
{
public:
	using input = F1InpuState;

	/**
	 * Representation of data to be sent to the device
	 *
	 * As Traktor Kontrol F1 only supports setting LEDs as HID output, the
	 * elements represent the LED brightness values (ranging vom 0x00 to 0x7f).
	 */
	struct output
	{
		constexpr static uint8_t report_id{0x80};
		std::array<std::array<uint8_t, 3>, 16> matrix_btns;
		std::array<uint8_t, 4> stop_btns;
	};

	/**
	 * Create a F1HidDev representing the hidraw device at the given file
	 * descriptor
	 *
	 * The file descriptor needs to be a valid file descriptor pointing to a
	 * hidraw device file of a Traktor Kontrol F1
	 *
	 * The user is responsible that the file descriptor is valid throughout the
	 * lifetime of this object
	 */
	constexpr F1HidDev(int fd) : fd(fd)
	{}
	/**
	 * Copy the F1HiDev interface
	 *
	 * This method call is safe as managing the file descriptor is not done
	 * within this class, but has to be done by the user.
	 */
	constexpr F1HidDev(const F1HidDev&) = default;
	~F1HidDev() = default;

	/**
	 * Read a fresh set of values from the Traktor Kontrol F1
	 *
	 * This method waits until the Traktor Kontrol F1 sends new data (i.e. the
	 * inputs on the F1 change). It writes the new state into an internal cache
	 * which can be retrieved using {@link read_cached()}.
	 */
	const input& read();
	/**
	 * Return the last received values from the Traktor Kontrol F1
	 *
	 * This method does not retrieve the current state from the Traktor Kontrol
	 * F1 synchronously, but rather returns the state which was transmitted
	 * during the last {@link read()} call.
	 */
	inline const input& read_cached() const
	{
		return input_state;
	}

	/**
	 * Write the given output to the Traktor Kontrol F1
	 */
	void write(const output& out) const;

	/**
	 * Convert 7-bit RGB values to the internal representation
	 *
	 * As the Traktor Kontrol F1 expects 24 bits of color in BRG, this method
	 * provides an interface to turn three separate RGB values into the correct
	 * internal representation. This representation can then be assigned to an
	 * {@link output} object to be sent to the F1.
	 */
	constexpr static std::array<uint8_t, 3> rgb2color(uint8_t r, uint8_t g,
	                                                  uint8_t b)
	{
		return {b, r, g};
	}

private:
	int fd;
	input input_state{};
};

/**
 * Exception representing errors during the communication with the Traktor
 * Kontrol F1
 *
 * Details of the error which occured are returned by {@link what()}.
 */
class F1HidDevException : public std::runtime_error
{
public:
	F1HidDevException(const std::string& msg) : std::runtime_error(msg)
	{}
};
