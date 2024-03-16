#include <cassert>

#include "RingbufferIterator.hpp"

RingbufferReadIterator::value_type RingbufferReadIterator::operator*()
{
	assert(ringbuf);
	assert(read_data[0].buf);
	if (!is_readable())
		throw std::out_of_range{
			"Dereferencing unreadable RingbufferReadIterator"
		};

	// NOLINTBEGIN(*-pointer-arithmetic)
	if (read_data[0].len > read_offset) {
		return read_data[0].buf[read_offset];
	}

	assert(read_data[1].len > 0);
	return read_data[1].buf[read_offset];
	// NOLINTEND(*-pointer-arithmetic)
}

void RingbufferReadIterator::advance_read_ptr() const
{
	jack_ringbuffer_read_advance(ringbuf, read_offset);
}

void RingbufferReadIterator::update_read_data()
{
	jack_ringbuffer_get_read_vector(ringbuf, read_data.data());
}
