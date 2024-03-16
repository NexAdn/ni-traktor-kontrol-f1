#pragma once

#include <array>
#include <cassert>
#include <functional>
#include <memory>
#include <new>
#include <stdexcept>
#include <type_traits>

#include <jack/ringbuffer.h>

/**
 * Type-safe and auto-freeing wrapper around a jack_ringbuffer_t
 *
 * This class provides a type-safe fixed-size lockless queue
 * by utilizing an underlying jack_ringbuffer_t.
 * Methods are offered similar to the standard library's methods of std::queue.
 */
template <typename T>
class Ringbuffer final
{
public:
	explicit Ringbuffer(std::size_t size) : buf_size(size)
	{
		static_assert(std::is_default_constructible<T>::value);
		static_assert(std::is_move_assignable<T>::value);
		static_assert(std::is_trivially_copyable<T>::value);

		init_buf();
		assert_invariants();
	}
	Ringbuffer(const Ringbuffer& o) : buf_size(o.buf_size)
	{
		init_buf();
		copy_buf(o);
		assert_invariants();
	}
	Ringbuffer& operator=(const Ringbuffer& o)
	{
		if (&o == this)
			return *this;

		buf_size = o.buf_size;
		init_buf();
		copy_buf(o);
		assert_invariants();

		return *this;
	}
	Ringbuffer(Ringbuffer&& o) noexcept :
		buf(std::move(o.buf)), buf_size(o.buf_size)
	{
		assert_invariants();
	}
	Ringbuffer& operator=(Ringbuffer&& o) noexcept
	{
		buf = std::move(o.buf);
		buf_size = o.buf_size;
		return *this;
	}
	~Ringbuffer() = default;

	[[nodiscard]] bool empty() const noexcept
	{
		assert_invariants();
		return jack_ringbuffer_read_space(buf.get()) < sizeof(T);
	}
	[[nodiscard]] bool full() const noexcept
	{
		assert_invariants();
		return jack_ringbuffer_write_space(buf.get()) < sizeof(T);
	}
	[[nodiscard]] std::size_t size() const noexcept
	{
		assert_invariants();
		return jack_ringbuffer_read_space(buf.get()) / sizeof(T);
	}
	[[nodiscard]] constexpr std::size_t capacity() const noexcept
	{
		return buf_size;
	}

	void push(const T& v)
	{
		assert_invariants();

		if (full())
			throw std::out_of_range{"ringbuffer full"};
		[[maybe_unused]] const std::size_t bytes_written =
			jack_ringbuffer_write(
				buf.get(),
				reinterpret_cast<const char*>(&v), // NOLINT(*-reinterpret-cast)
				sizeof(T)
			);
		assert(bytes_written == sizeof(T));

		assert_invariants();
	}

	T pop()
	{
		assert_invariants();

		if (empty())
			throw std::out_of_range{"ringbuffer empty"};

		T d;
		[[maybe_unused]] const std::size_t bytes_read =
			jack_ringbuffer_read(
				buf.get(),
				reinterpret_cast<char*>(&d), // NOLINT(*-reinterpret-cast)
				sizeof(T)
			);
		assert(bytes_read == sizeof(T));

		assert_invariants();
		return std::move(d);
	}

private:
	void init_buf()
	{
		jack_ringbuffer_t* ringbuf =
			jack_ringbuffer_create(buf_size * sizeof(T));
		if (ringbuf == nullptr)
			throw std::bad_alloc{};
		buf.reset(ringbuf);
	}

	void copy_buf(const Ringbuffer& o)
	{
		std::array<jack_ringbuffer_data_t, 2> read_vec{};
		jack_ringbuffer_get_read_vector(o.buf.get(), read_vec.data());

		if (read_vec[0].len == 0)
			return;

		[[maybe_unused]] std::size_t bytes_written =
			jack_ringbuffer_write(
				buf.get(), read_vec[0].buf, read_vec[0].len
			);
		assert(bytes_written == read_vec[0].len);

		if (read_vec[1].len > 0) {
			bytes_written = jack_ringbuffer_write(
				buf.get(), read_vec[1].buf, read_vec[1].len
			);
			assert(bytes_written == read_vec[1].len);
		}
	}

#ifndef NDEBUG
	void assert_invariants() const
	{
		assert(buf);
		assert(jack_ringbuffer_read_space(buf.get()) % sizeof(T) == 0);
	}
#else
	constexpr void assert_invariants() const noexcept
	{
	}
#endif

	std::unique_ptr<
		jack_ringbuffer_t,
		std::function<void(jack_ringbuffer_t*)>>
		buf{nullptr, jack_ringbuffer_free};
	std::size_t buf_size;
};
