#include <utility>

#include "MidiStream.hpp"

MidiStream::iterator::iterator(MidiStream& stream) :
	buf_iter(stream.buf.get()),
	end_iter(stream.buf.get(), RingbufferReadIterator::end_iter),
	next_buf_iter(end_iter)
{
}

MidiStream::iterator::iterator(
	MidiStream& stream, [[maybe_unused]] iterator::EndIter unused
) :
	buf_iter(stream.buf.get(), RingbufferReadIterator::end_iter),
	end_iter(buf_iter),
	next_buf_iter(end_iter)
{
}

MidiStream::iterator::value_type MidiStream::iterator::operator*()
{
	const auto [event, next_iter] = MidiEvent::parse(buf_iter, end_iter);
	next_buf_iter = next_iter;
	return event;
}

MidiStream::iterator& MidiStream::iterator::operator++()
{
	if (next_buf_iter == end_iter)
		**this;

	buf_iter = next_buf_iter;
	next_buf_iter = end_iter;
	return *this;
}

void MidiStream::iterator::advance_read_ptr() const
{
	buf_iter.advance_read_ptr();
}

MidiStream::MidiStream(std::size_t buf_size)
{
	jack_ringbuffer_t* buf_ptr = jack_ringbuffer_create(buf_size);
	if (buf_ptr == nullptr)
		throw std::bad_alloc{};

	buf.reset(buf_ptr);
}

void MidiStream::set_write_notification_callback(write_notification_cb cb)
{
	write_notification_callback = std::move(cb);
}

std::size_t MidiStream::write(const char* data, size_t count)
{
	const std::size_t written =
		jack_ringbuffer_write(buf.get(), data, count);
	if (written > 0 && write_notification_callback)
		write_notification_callback(*this);

	return written;
}

MidiStream::iterator MidiStream::begin()
{
	return iterator{*this};
}

MidiStream::iterator MidiStream::end()
{
	return iterator{*this, iterator::end};
}
