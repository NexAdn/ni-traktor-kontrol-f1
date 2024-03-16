#pragma once

#include <functional>
#include <memory>
#include <stdexcept>
#include <string>

#include <jack/types.h>

class JackWrapper final
{
	class Impl;

public:
	constexpr static const char* DEFAULT_CLIENT_NAME = "Traktor Kontrol F1";

	using write_callback = std::function<int(void*, jack_nframes_t)>;
	using read_callback = std::function<int(void*, jack_nframes_t)>;
	using xrun_callback = std::function<int()>;

	explicit JackWrapper(
		const std::string& client_name = DEFAULT_CLIENT_NAME
	);
	JackWrapper(const JackWrapper&) = delete;
	JackWrapper& operator=(const JackWrapper&) = delete;
	JackWrapper(JackWrapper&&) = delete;
	JackWrapper& operator=(JackWrapper&&) = delete;
	~JackWrapper();

	void set_write_callback(write_callback write_cb);
	void set_read_callback(read_callback read_cb);
	void set_xrun_callback(xrun_callback xrun_cb);

	void activate();
	void deactivate();

private:
	std::unique_ptr<Impl> p_impl;
};

class JackWrapperException : public std::runtime_error
{
public:
	explicit JackWrapperException(const std::string& errmsg) :
		std::runtime_error(errmsg)
	{
	}
};
