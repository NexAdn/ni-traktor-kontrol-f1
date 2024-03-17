#pragma once

#include <functional>
#include <memory>
#include <stdexcept>
#include <string>

#include <jack/types.h>

class MidiEvent;

class JackWrapper final
{
	class Impl;

public:
	constexpr static const char* DEFAULT_CLIENT_NAME = "Traktor Kontrol F1";
	constexpr static std::size_t IN_BUF_SIZE{8192U};
	constexpr static std::size_t OUT_BUF_SIZE{128U};

	using xrun_callback = std::function<int()>;

	explicit JackWrapper(
		const std::string& client_name = DEFAULT_CLIENT_NAME
	);
	JackWrapper(const JackWrapper&) = delete;
	JackWrapper& operator=(const JackWrapper&) = delete;
	JackWrapper(JackWrapper&&) = delete;
	JackWrapper& operator=(JackWrapper&&) = delete;
	~JackWrapper();

	void set_xrun_callback(xrun_callback xrun_cb);

	void activate();
	void deactivate();

	[[nodiscard]] std::size_t read_bufsize() const noexcept;
	[[nodiscard]] std::size_t write_bufsize() const noexcept;

	/**
	 * Send a MIDI event to the output buffer
	 */
	JackWrapper& operator<<(const MidiEvent& event);
	/**
	 * Read a MIDI event from the input buffer
	 */
	JackWrapper& operator>>(MidiEvent& event);

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
