#include <cassert>

#ifndef NDEBUG
#include <iomanip>
#endif

#include <jack/midiport.h>

#include "F1InputState.hpp"
#include "JackClient.hpp"

#include "debug.hpp"

void JackClient::connect()
{
	assert(!is_connected());
	debug("Connecting JackClient");

	jack_status_t status;
	jack_client_t* client =
	  jack_client_open(client_name.c_str(), JackClient::JACK_OPTIONS, &status);
	if (!client)
		throw JackClientException(
		  std::string{"Failed to create jack client! Status: "}
		  + std::to_string(status));
	jack_client.reset(client);

	debug("Registering callbacks");
	int res = jack_set_process_callback(jack_client.get(), process_cb, this);
	if (res != 0) {
		throw JackClientException(
		  std::string{"Failed to register process callback! res: "}
		  + std::to_string(res));
	}

	debug("Registering out port");
	jack_port_t* out = jack_port_register(jack_client.get(),
	                                      OUT_PORT_NAME,
	                                      JACK_DEFAULT_MIDI_TYPE,
	                                      JackPortFlags::JackPortIsOutput,
	                                      0);
	if (!out)
		throw JackClientException("Failed to create jack port!");
	out_port.reset(out);

	debug("Activating client");
	res = jack_activate(client);
	if (res != 0) {
		throw JackClientException(
		  std::string{"Failed to activate jack client! res: "}
		  + std::to_string(res));
	}
}

void JackClient::disconnect()
{
	assert(is_connected());
	debug("Disconnecting JackClient");
	int res = jack_deactivate(jack_client.get());
	assert(res == 0);
	res = jack_client_close(jack_client.get());
	assert(res == 0);
	jack_client.reset(nullptr);
}

void JackClient::push_event(const F1InputChange& event)
{
	debug("Pushing event");
	{
		byte_t note_on = MIDI_NOTE_ON | midi_channel;
		assert(note_on & 0x80);
		byte_t vel = MIDI_NOTE_ON_VEL;
		assert((vel & 0x80) == 0);
		for (auto& btn : event.pressed_buttons.matrix) {
			byte_t key = MIDI_NOTE::MATRIX_BASE + btn;
			assert((key & 0x80) == 0);

			write_midi(note_on, key, vel);
		}
	}
	{
		byte_t note_off = MIDI_NOTE_OFF | midi_channel;
		assert(note_off & 0x80);
		byte_t vel = MIDI_NOTE_OFF_VEL;
		assert((vel & 0x80) == 0);
		for (auto& btn : event.released_buttons.matrix) {
			byte_t key = MIDI_NOTE::MATRIX_BASE + btn;
			assert((key & 0x80) == 0);

			write_midi(note_off, key, vel);
		}
	}
	{
		byte_t cc = MIDI_CONTROL_CHANGE | midi_channel;
		assert(cc & 0x80);
		byte_t cc_on = MIDI_CC_ON;
		assert((cc_on & 0x80) == 0);
		byte_t cc_off = MIDI_CC_OFF;
		assert((cc_off & 0x80) == 0);

		for (auto& btn : event.pressed_buttons.stop) {
			byte_t cc_controller = MIDI_CONTROLLER::STOP[btn];
			assert((cc_controller & 0x80) == 0);

			write_midi(cc, cc_controller, cc_on);
		}
		for (auto& btn : event.released_buttons.stop) {
			byte_t cc_controller = MIDI_CONTROLLER::STOP[btn];
			assert((cc_controller & 0x80) == 0);

			write_midi(cc, cc_controller, cc_off);
		}
		for (auto& btn : event.pressed_buttons.special) {
			byte_t cc_controller = MIDI_CONTROLLER::SPECIAL[btn];
			assert((cc_controller & 0x80) == 0);

			write_midi(cc, cc_controller, cc_on);
		}
		for (auto& btn : event.released_buttons.special) {
			byte_t cc_controller = MIDI_CONTROLLER::SPECIAL[btn];
			assert((cc_controller & 0x80) == 0);

			write_midi(cc, cc_controller, cc_off);
		}
	}
}

inline void JackClient::write_midi(byte_t status, byte_t v1, byte_t v2)
{
	jack_ringbuffer_t* rb = input_buf.get();
	JackClient::byte_t midi_event[] = {status, v1, v2};
	size_t bytes_written =
	  jack_ringbuffer_write(rb, reinterpret_cast<const char*>(midi_event), 3);
	assert(bytes_written == 3);
	created_midi_bytes += 3;
#ifndef NDEBUG
	const auto fmtflags = std::cout.flags();
	std::cout << std::hex << std::setfill('0') << std::setw(2)
	          << static_cast<int>(status) << ' ' << std::setw(2)
	          << static_cast<int>(v1) << ' ' << std::setw(2)
	          << static_cast<int>(v2) << '\n';
	std::cout.flags(fmtflags);
#endif
	debug(std::string{"CM:\t"} + std::to_string(created_midi_bytes) + "\tSM:\t"
	      + std::to_string(sent_midi_bytes) + "\tBS:\t"
	      + std::to_string(bytes_in_input_buffer()));
}

inline size_t JackClient::bytes_in_input_buffer() const
{
	return jack_ringbuffer_read_space(input_buf.get());
}

int JackClient::process_cb(jack_nframes_t nframes, void* userdata)
{
	auto& jc = *reinterpret_cast<JackClient*>(userdata);
	assert(&jc);
	jack_ringbuffer_t* in_buf = jc.input_buf.get();
	assert(in_buf);
	void* out_buf = jack_port_get_buffer(jc.out_port.get(), nframes);
	assert(out_buf);
	jack_midi_clear_buffer(out_buf);
	if (jc.bytes_in_input_buffer() == 0)
		return 0;

	size_t bytes_to_send = jc.bytes_in_input_buffer();
	debug(std::string{"Writing MIDI ("} + std::to_string(bytes_to_send)
	      + " bytes)");

	// jack_midi_data_t* data_buf =
	//  jack_midi_event_reserve(out_buf, 0, bytes_to_send);
	// if (!data_buf) {
	//	debug("Cannot reserve buffer for MIDI event");
	//	return 1;
	//}
	// size_t bytes_read = jack_ringbuffer_read(
	//  in_buf, reinterpret_cast<char*>(data_buf), bytes_to_send);
	// if (bytes_read != bytes_to_send) {
	//	debug("Didn't read enough event bytes!");
	//	return 2;
	//}
	//
	for (size_t i = 0; i < bytes_to_send;) {
		byte_t midi_event[3];
		size_t read_bytes =
		  jack_ringbuffer_read(in_buf, reinterpret_cast<char*>(midi_event), 3);
		if (read_bytes != 3) {
			debug("Didn't read enough event bytes!");
			return 2;
		}
		int write_res = jack_midi_event_write(out_buf, 0, midi_event, 3);
		if (write_res != 0) {
			debug("MIDI write failed");
			return 2;
		}

		i += read_bytes;
	}

	jc.sent_midi_bytes += bytes_to_send;

	return 0;
}
