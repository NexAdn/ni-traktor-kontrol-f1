#pragma once

#include <array>
#include <cassert>
#include <cstdint>
#include <iterator>
#include <stdexcept>

#include <jack/ringbuffer.h>

/**
 * Iterator for accessing a jack_ringbuffer_t
 *
 * This class abstracts the access to data in a jack_ringbuffer_t like
 * a standard library iterator.
 * This should allow using most standard library algorithms
 * to iterate the ringbuffer.
 *
 * This class satisfies the named requirements of a
 * _LegacyRandomAccessIterator_.
 *
 * Advancing the ringbuffer or calling @c RingbufferReadIterator::advance()
 * invalidates all iterators.
 */
class RingbufferReadIterator final
{
public:
	struct EndIter {
	};

	constexpr static EndIter end_iter{}; // NOLINT(*-identifier-naming)

	using byte = uint8_t;

	using difference_type = std::size_t;
	using value_type = byte;
	using pointer = value_type*;
	using reference = value_type&;
	using iterator_category = std::random_access_iterator_tag;

	/**
	 * Create an end iterator
	 */
	RingbufferReadIterator(
		jack_ringbuffer_t* ringbuf, [[maybe_unused]] EndIter unused
	) :
		ringbuf(ringbuf)
	{
		assert(ringbuf != nullptr);
		update_read_data();
		// One past the available read buffer
		read_offset = read_data.at(0).len + read_data.at(1).len;
	}
	explicit RingbufferReadIterator(jack_ringbuffer_t* ringbuf) :
		// One past the available read buffer
		ringbuf(ringbuf)
	{
		assert(ringbuf != nullptr);
		update_read_data();
	}
	RingbufferReadIterator(const RingbufferReadIterator&) = default;
	RingbufferReadIterator&
	operator=(const RingbufferReadIterator&) = default;
	RingbufferReadIterator(RingbufferReadIterator&&) = default;
	RingbufferReadIterator& operator=(RingbufferReadIterator&&) = default;
	~RingbufferReadIterator() = default;

	/**
	 * Alias to @c RingbufferReadIterator::is_readable()
	 */
	constexpr
	operator bool() const noexcept // NOLINT(*-explicit-conversions)
	{
		return is_readable();
	}

	constexpr RingbufferReadIterator& operator++() noexcept
	{
		++read_offset;
		return *this;
	}

	constexpr RingbufferReadIterator& operator+=(std::size_t add_read_offset
	) noexcept
	{
		read_offset += add_read_offset;
		return *this;
	}

	/**
	 * Read the byte at the current position
	 *
	 * @return The byte at the current position, if present.
	 *
	 * @throw std::out_of_range if the element to read
	 * is outside the read vector.
	 *
	 * @throw std::invalid_argument if the iterator is an end iterator.
	 */
	value_type operator*();

	/**
	 * Check if this iterator points to a readable region in the ringbuffer
	 */
	[[nodiscard]] constexpr bool is_readable() const noexcept
	{
		return read_offset < read_data.at(0).len + read_data.at(1).len;
	}

	/**
	 * Advance the ringbuffer's read pointer to
	 * the position this iterator points to
	 *
	 * This is the last operation after
	 * performing a bunch of read operations on a ringbuffer.
	 * After having read all the data,
	 * this method must be called on the iterator past
	 * the end of the processed range
	 * to advance the ringbuffer's read pointer.
	 *
	 * This invalidates all iterators for the given ringbuffer.
	 */
	void advance_read_ptr() const;

	[[nodiscard]] constexpr bool is_end() const noexcept
	{
		return !is_readable();
	}

	/**
	 * Calculate absolute distance between to @c RingbufferReadIterator
	 *
	 * The distance is expressed by calculating the difference in
	 * the positions of two iterators.
	 * This function is commutative;
	 * the distance will always be positive.
	 *
	 * Only the distance between iterators for the same ringbuffer
	 * can be calculated.
	 *
	 * @param lhs Left-hand-side iterator
	 * @param rhs Right-hand-side iterator
	 *
	 * @throw std::invalid_argument If two iterators
	 * from different ringbuffers are supplied.
	 */
	constexpr friend difference_type operator-(
		const RingbufferReadIterator& lhs,
		const RingbufferReadIterator& rhs
	)
	{
		if (lhs.ringbuf != rhs.ringbuf)
			throw std::invalid_argument{
				"Trying to calculate distance between "
				"iterators of diffent buffers"
			};

		if (lhs.read_offset < rhs.read_offset)
			return rhs.read_offset - lhs.read_offset;

		return lhs.read_offset - rhs.read_offset;
	}

	constexpr friend bool operator==(
		const RingbufferReadIterator& lhs,
		const RingbufferReadIterator& rhs
	) noexcept
	{
		if (lhs.is_end() && rhs.is_end())
			return true;

		return lhs.ringbuf == rhs.ringbuf &&
		       lhs.read_offset == rhs.read_offset;
	}

private:
	[[nodiscard]] constexpr bool is_end_iterator() const noexcept
	{
		return read_data[0].len + read_data[1].len < read_offset + 1;
	}

	void update_read_data();

	jack_ringbuffer_t* ringbuf;
	std::array<jack_ringbuffer_data_t, 2> read_data{
		jack_ringbuffer_data_t{nullptr, 0},
		jack_ringbuffer_data_t{nullptr, 0}
	};
	size_t read_offset{0};
};

/**
 * Wrapper for easy creation of begin iterators for a ringbuffer
 */
inline RingbufferReadIterator begin(jack_ringbuffer_t* buf)
{
	return RingbufferReadIterator{buf};
}

/**
 * Wrapper for easy creation of past-the-end iterators for a ringbuffer
 */
inline RingbufferReadIterator end(jack_ringbuffer_t* buf)
{
	return RingbufferReadIterator{buf, RingbufferReadIterator::end_iter};
}
