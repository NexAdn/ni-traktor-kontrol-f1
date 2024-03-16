#pragma once

#include <functional>
#include <iterator>
#include <memory>

#include <jack/ringbuffer.h>
#include <jack/types.h>

#include "MidiEvent.hpp"
#include "RingbufferIterator.hpp"

class MidiStream final
{
public:
	using ringbuffer_dtor = std::function<void(jack_ringbuffer_t*)>;
	using ringbuffer_ptr =
		std::unique_ptr<jack_ringbuffer_t, ringbuffer_dtor>;

	using write_notification_cb = std::function<void(MidiStream&)>;

	class iterator final // NOLINT(*-identifier-naming)
	{
	public:
		using iterator_category = std::input_iterator_tag;
		using value_type = MidiEvent;
		using difference_type = std::ptrdiff_t;
		using pointer = value_type*;
		using reference = value_type&;

	private:
		struct EndIter {
		};
		constexpr static EndIter end{}; // NOLINT(*-identifier-naming)

		explicit iterator(MidiStream& stream);
		iterator(MidiStream& stream, EndIter);

	public:
		iterator(const iterator&) = default;
		iterator& operator=(const iterator&) = default;
		iterator(iterator&&) = default;
		iterator& operator=(iterator&&) = default;
		~iterator() = default;

		value_type operator*();
		iterator& operator++();

		void advance_read_ptr() const;

		[[nodiscard]] constexpr bool is_end() const noexcept
		{
			return buf_iter == end_iter;
		}

		constexpr friend bool
		operator==(const iterator& lhs, const iterator& rhs)
		{
			return lhs.buf_iter == rhs.buf_iter;
		}

	private:
		RingbufferReadIterator buf_iter;
		RingbufferReadIterator end_iter;
		RingbufferReadIterator next_buf_iter;

		friend class MidiStream;
	};

	explicit MidiStream(std::size_t buf_size);
	MidiStream(const MidiStream&) = delete;
	MidiStream& operator=(const MidiStream&) = delete;
	MidiStream(MidiStream&&) = delete;
	MidiStream& operator=(MidiStream&&) = delete;
	~MidiStream() = default;

	void set_write_notification_callback(write_notification_cb cb);

	size_t write(const char* data, size_t count);

	iterator begin();
	iterator end();

private:
	ringbuffer_ptr buf{nullptr, jack_ringbuffer_free};
	write_notification_cb write_notification_callback{};
};
