#include <cassert>
#include <iostream>
#include <thread>

#include <jack/jack.h>
#include <jack/midiport.h>
#include <jack/ringbuffer.h>

#include "JackClient.hpp"
#include "debug.hpp"

namespace
{
constexpr const char* CLIENT_NAME{"my"};
constexpr size_t RINGBUFFER_SIZE{1024};

struct process_metadata
{
	jack_client_t* client;
	jack_port_t* in;
	jack_port_t* out;
	jack_ringbuffer_t* in_buf;
	jack_ringbuffer_t* out_buf;
	size_t buf_size;
	volatile size_t frames_processed{0};
};

// volatile size_t processed{0};

int process(jack_nframes_t nframes, void* arg)
{
	process_metadata* meta = reinterpret_cast<process_metadata*>(arg);
	// char* raw_data[64];

	jack_midi_clear_buffer(meta->out_buf);
	jack_nframes_t frames = jack_midi_get_event_count(meta->in_buf);
	if (frames > 64)
		frames = 64;

	void* in_buf = jack_port_get_buffer(meta->in, frames);
	void* out_buf = jack_port_get_buffer(meta->out, frames);
	jack_midi_clear_buffer(out_buf);
	jack_midi_event_t event;
	uint32_t idx{0};
	jack_nframes_t frame{0};
	while (jack_midi_event_get(&event, in_buf, idx) == 0) {
		int res =
		  jack_midi_event_write(out_buf, frame, event.buffer, event.size);
		if (res != 0)
			return 1;
		idx++;
		frame++;
		meta->frames_processed++;
	}

	return 0;
}
} // namespace

int main(int argc, char** argv)
{
	jack_status_t status;
	jack_client_t* client =
	  jack_client_open(CLIENT_NAME, JackClient::JACK_OPTIONS, &status);
	assert(client);
	assert((status & JackStatus::JackFailure) == 0);

	debug("Client opened");

	jack_port_t* in = jack_port_register(client, "in", JACK_DEFAULT_MIDI_TYPE,
	                                     JackPortFlags::JackPortIsInput, 0);
	assert(in);
	assert((status & JackStatus::JackFailure) == 0);
	jack_port_t* out = jack_port_register(client, "out", JACK_DEFAULT_MIDI_TYPE,
	                                      JackPortFlags::JackPortIsOutput, 0);
	assert(out);
	assert((status & JackStatus::JackFailure) == 0);
	debug("Ports created!");

	process_metadata meta;
	meta.client = client;
	meta.in = in;
	meta.out = out;
	meta.buf_size = RINGBUFFER_SIZE;
	meta.in_buf = jack_ringbuffer_create(meta.buf_size);
	assert(meta.in_buf);
	meta.out_buf = jack_ringbuffer_create(meta.buf_size);
	assert(meta.out_buf);

	int res = jack_set_process_callback(client, process, &meta);
	assert(res == 0);
	debug("Process callback set!\n");

	res = jack_activate(client);
	assert(res == 0);
	debug("Client started!\n");

	size_t last_frames_processed = meta.frames_processed;
	while (true) {
		if (last_frames_processed != meta.frames_processed) {
			std::cout << "Frames processed:\t" << meta.frames_processed
			          << std::endl;
			last_frames_processed = meta.frames_processed;
		}
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
	return 0;
}
