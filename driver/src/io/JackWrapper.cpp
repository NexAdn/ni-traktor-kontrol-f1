#include <cassert>
#include <cstddef>
#include <functional>
#include <iterator>

#include "JackWrapper.hpp"
#include "io/MidiEvent.hpp"
#include "io/Ringbuffer.hpp"
#include "io/RingbufferIterator.hpp"

#include <jack/jack.h>
#include <jack/midiport.h>
#include <jack/ringbuffer.h>
#include <jack/types.h>

using jack_client_dtor = std::function<void(jack_client_t*)>;
using jack_port_dtor = std::function<void(jack_port_t*)>;

using jack_client_ptr = std::unique_ptr<jack_client_t, jack_client_dtor>;
using jack_port_ptr = std::unique_ptr<jack_port_t, jack_port_dtor>;

class JackWrapper::Impl
{
public:
	using write_callback_t = std::function<int(void*, jack_nframes_t)>;
	using read_callback_t = std::function<int(void*, jack_nframes_t)>;

	explicit Impl(const std::string& client_name)
	{
		init_client(client_name);
		init_ports();
		init_process_callback();
		init_xrun_callback();

		read_cb = read_callback();
		write_cb = write_callback();
	}

	Impl(const Impl&) = delete;
	Impl& operator=(const Impl&) = delete;
	Impl(Impl&&) = delete;
	Impl& operator=(Impl&&) = delete;

	~Impl()
	{
		if (active) {
			jack_deactivate(client.get());
		}
	}

	void init_client(const std::string& client_name)
	{
		// NOLINTNEXTLINE(*-vararg)
		jack_client_t* client_ptr = jack_client_open(
			client_name.c_str(),
			JackOptions::JackNullOption,
			nullptr
		);
		if (client_ptr == nullptr) {
			throw JackWrapperException{"Failed to open JACK client"
			};
		}

		client.reset(client_ptr);
	}

	void init_ports()
	{
		assert(client);

		jack_port_t* midi_in_ptr = jack_port_register(
			client.get(),
			"in",
			JACK_DEFAULT_MIDI_TYPE,
			JackPortFlags::JackPortIsInput,
			0
		);
		if (midi_in_ptr == nullptr) {
			throw JackWrapperException{
				"Failed to register MIDI input port"
			};
		}

		jack_port_t* midi_out_ptr = jack_port_register(
			client.get(),
			"out",
			JACK_DEFAULT_MIDI_TYPE,
			JackPortFlags::JackPortIsOutput,
			0
		);
		if (midi_out_ptr == nullptr) {
			throw JackWrapperException{
				"Failed to register MIDI output port"
			};
		}

		midi_in = jack_port_ptr{
			midi_in_ptr, jack_port_deleter(client.get())
		};
		midi_out = jack_port_ptr{
			midi_out_ptr, jack_port_deleter(client.get())
		};
	}

	void init_process_callback()
	{
		assert(client);
		int res =
			jack_set_process_callback(client.get(), process, this);

		if (res != 0) {
			throw JackWrapperException(
				"Failed to set process callback"
			);
		}
	}

	void init_xrun_callback()
	{
		assert(client);
		int res = jack_set_xrun_callback(client.get(), xrun, this);

		if (res != 0) {
			throw JackWrapperException("Failed to set xrun callback"
			);
		}
	}

	jack_client_dtor jack_client_deleter()
	{
		return [this](jack_client_t* c) -> void {
			assert(c);
			midi_in.reset(nullptr);
			jack_client_close(c);
		};
	}

	static int process(jack_nframes_t nframes, void* userarg)
	{
		assert(userarg);
		return static_cast<Impl*>(userarg)->process_int(nframes);
	}

	[[nodiscard]] int process_int(jack_nframes_t nframes) const
	{
		if (!write_cb || !read_cb)
			return 1;

		void* write_buf = jack_port_get_buffer(midi_out.get(), nframes);
		assert(write_buf);
		const int write_res = write_cb(write_buf, nframes);

		void* read_buf = jack_port_get_buffer(midi_in.get(), nframes);
		assert(read_buf);
		const int read_res = read_cb(read_buf, nframes);

		return write_res + read_res;
	}

	static int xrun(void* userarg)
	{
		// NOLINTNEXTLINE(*-reinterpret-cast)
		return reinterpret_cast<Impl*>(userarg)->xrun_cb();
	}

	static jack_port_dtor jack_port_deleter(jack_client_t* client)
	{
		return [client](jack_port_t* port) -> void {
			assert(port);
			assert(client);
			jack_port_unregister(client, port);
		};
	}

	read_callback_t read_callback()
	{
		return [this](void* buf, jack_nframes_t) -> int {
			for (jack_nframes_t i = 0;
			     i < jack_midi_get_event_count(buf);
			     ++i) {
				jack_midi_event_t jack_event;
				const int res = jack_midi_event_get(
					&jack_event, buf, i
				);
				if (res != 0)
					return 1;

				// NOLINTBEGIN(*-pointer-arithmetic)
				for (jack_midi_data_t* i = jack_event.buffer;
				     i < jack_event.buffer + jack_event.size;
				     ++i) {
					if (in_buf.full())
						return 1;

					in_buf.push(*i);
				}
				// NOLINTEND(*-pointer-arithmetic)
			}

			return 0;
		};
	}

	write_callback_t write_callback()
	{
		return [this](void* buf, jack_nframes_t nframes) -> int {
			jack_midi_clear_buffer(buf);
			for (jack_nframes_t i = 0;
			     not out_buf.empty() && i < nframes / 3;
			     ++i) {
				MidiEvent event = out_buf.pop();
				auto bytes = event.to_bytes();
				const int res = jack_midi_event_write(
					buf, 0, bytes.data(), bytes.size()
				);
				assert(res == 0);
			}

			return 0;
		};
	}

	jack_client_ptr client{nullptr, jack_client_deleter()};
	bool active{false};
	jack_port_ptr midi_in{nullptr};
	jack_port_ptr midi_out{nullptr};

	write_callback_t write_cb;
	read_callback_t read_cb;
	JackWrapper::xrun_callback xrun_cb{[]() -> int {
		return 0;
	}};
	Ringbuffer<jack_midi_data_t> in_buf{IN_BUF_SIZE};
	Ringbuffer<MidiEvent> out_buf{OUT_BUF_SIZE};
};

JackWrapper::JackWrapper(const std::string& client_name) :
	p_impl(std::make_unique<Impl>(client_name))
{
}

JackWrapper::~JackWrapper() = default;

void JackWrapper::set_xrun_callback(xrun_callback xrun_cb)
{
	p_impl->xrun_cb = std::move(xrun_cb);
}

void JackWrapper::activate()
{
	if (p_impl->active)
		return;

	int res = jack_activate(p_impl->client.get());
	if (res != 0)
		throw JackWrapperException{"Failed to activate JACK client"};
}

void JackWrapper::deactivate()
{
	if (not p_impl->active)
		return;

	int res = jack_deactivate(p_impl->client.get());
	if (res != 0)
		throw JackWrapperException{"Failed to deactivate JACK client"};
}

std::size_t JackWrapper::read_bufsize() const noexcept
{
	return p_impl->in_buf.size();
}

std::size_t JackWrapper::write_bufsize() const noexcept
{
	return p_impl->out_buf.size();
}

JackWrapper& JackWrapper::operator<<(const MidiEvent& event)
{
	assert(not p_impl->out_buf.full());
	p_impl->out_buf.push(event);
	return *this;
}

JackWrapper& JackWrapper::operator>>(MidiEvent& event)
{
	RingbufferReadIterator it{p_impl->in_buf.data()};
	RingbufferReadIterator end{
		p_impl->in_buf.data(), RingbufferReadIterator::end_iter
	};

	auto [parsed_event, next_it] = MidiEvent::parse(it, end);
	jack_ringbuffer_read_advance(
		p_impl->in_buf.data(), std::distance(it, next_it)
	);
	event = parsed_event;
	return *this;
}
