#include <cassert>
#include <cerrno>
#include <cstring>

#include <fcntl.h>
#include <linux/hidraw.h>
#include <sys/ioctl.h>

#include "HidDevice.hpp"

// NOLINTNEXTLINE(*-vararg)
HidDevice::HidDevice(const char* path) : fd(open(path, O_RDWR))
{
	if (not fd) {
		if (errno == EPERM || errno == EACCES) {
			throw HidDevicePermissionDenied{};
		}

		throw_strerror();
	}
}

hidraw_devinfo HidDevice::devinfo() const
{
	assert(fd);
	hidraw_devinfo devinfo; // NOLINT(*-member-init)
	const int res =
		ioctl(*fd, HIDIOCGRAWINFO, &devinfo); // NOLINT(*-vararg)

	if (res < 0) {
		throw_strerror();
	}

	return devinfo;
}

void HidDevice::write(bytestr buf) const
{
	const ssize_t written = ::write(*fd, buf.data(), buf.size());
	if (written < static_cast<ssize_t>(buf.size())) {
		throw_strerror();
	}
}

void HidDevice::read(void* buf, std::size_t buf_size) const
{
	const ssize_t read = ::read(*fd, buf, buf_size);
	if (read < static_cast<ssize_t>(buf_size)) {
		throw_strerror();
	}
}

void HidDevice::throw_strerror()
{
	throw HidDeviceError{strerror(errno)};
}
