#pragma once

#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <string_view>

extern "C" {
#include <linux/hidraw.h>
}

#include "io/FileDescriptor.hpp"

/**
 * Abstraction for access to a HID device
 */
class HidDevice
{
public:
	using bytestr = std::basic_string_view<std::uint8_t>;

	explicit HidDevice(const char* path);
	HidDevice(const HidDevice&) = delete;
	HidDevice& operator=(const HidDevice&) = delete;
	HidDevice(HidDevice&&) = default;
	HidDevice& operator=(HidDevice&&) = default;
	~HidDevice() noexcept(false) = default;

	[[nodiscard]] hidraw_devinfo devinfo() const;

	void write(bytestr buf) const;
	void read(void* buf, std::size_t buf_size) const;

private:
	static void throw_strerror();

	FileDescriptor<> fd;
};

class HidDeviceError : public std::runtime_error
{
public:
	explicit HidDeviceError(const std::string& msg) :
		std::runtime_error(msg)
	{
	}

	explicit HidDeviceError(const char* msg) : std::runtime_error(msg)
	{
	}
};

class HidDevicePermissionDenied : public HidDeviceError
{
public:
	HidDevicePermissionDenied() : HidDeviceError("Permission denied")
	{
	}
};
