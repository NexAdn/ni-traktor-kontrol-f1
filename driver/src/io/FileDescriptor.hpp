#pragma once

#include <functional>
#include <system_error>
#include <utility>

#include <unistd.h>

/**
 * Wrapper for a file descriptor with automatic close
 */
template <typename Close = decltype(::close)>
class FileDescriptor
{
public:
	using close_function_t = std::function<int(int)>;

	explicit constexpr FileDescriptor(
		int fd, Close close_function = ::close
	) :
		fd(fd), close_function(std::move(close_function))
	{
	}
	FileDescriptor(const FileDescriptor&) = delete;
	FileDescriptor& operator=(const FileDescriptor&) = delete;
	constexpr FileDescriptor(FileDescriptor&& other_fd) noexcept :
		fd(other_fd.fd), close_function(other_fd.close_function)
	{
		other_fd.fd = -1;
	}
	constexpr FileDescriptor& operator=(FileDescriptor&& other_fd) noexcept
	{
		if (this == &other_fd)
			return *this;

		fd = other_fd.fd;
		other_fd.fd = -1;
		close_function = other_fd.close_function;

		return *this;
	}
	~FileDescriptor() noexcept(false)
	{
		close();
	}

	void close()
	{
		if (fd >= 0) {
			const int res = close_function(fd);

			if (res < 0) {
				throw std::system_error{std::error_code{
					errno, std::system_category()
				}};
			}

			fd = -1;
		}
	}

	constexpr operator bool() const noexcept // NOLINT(*-explicit-conversions)
	{
		return fd >= 0;
	}

	constexpr int operator*() const noexcept
	{
		return fd;
	}

	constexpr int& operator*() noexcept
	{
		return fd;
	}

private:
	int fd{-1};
	close_function_t close_function{::close};
};
