#pragma once

#include <array>
#include <cassert>
#include <functional>
#include <memory>
#include <stdexcept>
#include <string_view>

#include <jack/jack.h>
#include <jack/ringbuffer.h>

class NonSignal;
class F1InputChange;

/**
 * A connection to JACK as a client
 *
 * This class provides the abstraction to connect to the JACK server as a client
 * and to register all necessary callbacks. It is the main interface for
 * interacting with JACK.
 */
class JackClient
{
public:
	using jack_client_ptr =
	  std::unique_ptr<jack_client_t, std::function<void(jack_client_t*)>>;
	using jack_port_ptr =
	  std::unique_ptr<jack_port_t, std::function<void(jack_port_t*)>>;
	using jack_ringbuffer_ptr =
	  std::unique_ptr<jack_ringbuffer_t,
	                  std::function<void(jack_ringbuffer_t*)>>;
	using byte_t = uint8_t;

	constexpr static jack_options_t JACK_OPTIONS{
	  JackOptions::JackNoStartServer};
	constexpr static const char* OUT_PORT_NAME{"out"};
	constexpr static size_t INPUT_BUF_SIZE{256};

	constexpr static byte_t MIDI_NOTE_OFF{0x80};
	constexpr static byte_t MIDI_NOTE_ON{0x90};
	constexpr static byte_t MIDI_NOTE_OFF_VEL{0x00};
	constexpr static byte_t MIDI_NOTE_ON_VEL{0x7f};
	constexpr static byte_t MIDI_CONTROL_CHANGE{0xB0};
	constexpr static byte_t MIDI_CC_OFF{0x00};
	constexpr static byte_t MIDI_CC_ON{0x7f};

	struct MIDI_CONTROLLER
	{
		// Used some free or general purpose MIDI controllers
		constexpr static std::array<byte_t, 4> KNOB{0x10, 0x11, 0x12, 0x13};
		constexpr static std::array<byte_t, 4> FADER{0x50, 0x51, 0x52, 0x53};
		constexpr static std::array<byte_t, 9> SPECIAL{
		  0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e};
		constexpr static std::array<byte_t, 4> STOP{0x70, 0x71, 0x72, 0x73};
	};
	struct MIDI_NOTE
	{
		// 48 == C3
		constexpr static byte_t MATRIX_BASE{48};
	};

	inline JackClient(std::string_view client_name) : client_name(client_name)
	{}
	JackClient(const JackClient&) = delete;
	inline ~JackClient()
	{
		if (is_connected())
			disconnect();
	}

	/**
	 * Initiate the connetion to the JACK server
	 */
	void connect();
	/**
	 * Close an open connection to the JACK server
	 */
	void disconnect();

	inline bool is_connected() const noexcept
	{
		return jack_client.get() != nullptr;
	}

	inline void set_channel(uint8_t channel)
	{
		midi_channel = channel;
	}

	/**
	 * Push events to the outbound MIDI event ring buffer
	 *
	 * This method only pushes MIDI events for values that actually have
	 * changed. Other values don't generate events.
	 */
	void push_event(const F1InputChange& changes);

private:
	inline void write_midi(JackClient::byte_t status,
	                       JackClient::byte_t v1,
	                       JackClient::byte_t v2);
	inline size_t bytes_in_input_buffer() const;

	static int process_cb(jack_nframes_t nframes, void* userdata);

	std::string client_name;
	jack_client_ptr jack_client{nullptr, jack_client_close};
	jack_port_ptr out_port{nullptr, [this](jack_port_t* port) {
		                       int res =
		                         jack_port_unregister(jack_client.get(), port);
		                       assert(res == 0);
	                       }};
	jack_ringbuffer_ptr input_buf{jack_ringbuffer_create(INPUT_BUF_SIZE),
	                              jack_ringbuffer_free};

	uint8_t midi_channel;

	size_t created_midi_bytes{0};
	size_t sent_midi_bytes{0};
};

class JackClientException : public std::runtime_error
{
public:
	JackClientException(const std::string& what)
	    : std::runtime_error(what.c_str())
	{}
};
