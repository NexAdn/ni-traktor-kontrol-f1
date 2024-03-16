#include <functional>
#include <memory>

#include <catch2/catch_test_macros.hpp>

#include <jack/ringbuffer.h>

#include "io/RingbufferIterator.hpp"

namespace
{
constexpr std::size_t BUF_SIZE{1024};
constexpr std::size_t DATA_SIZE{16};
}

TEST_CASE("RingbufferReadIterator", "[iterator][io]")
{
	std::unique_ptr<
		jack_ringbuffer_t,
		std::function<void(jack_ringbuffer_t*)>>
		buf{jack_ringbuffer_create(BUF_SIZE), jack_ringbuffer_free};

	std::array<char, DATA_SIZE> example_data{};
	for (size_t i = 0; i < DATA_SIZE; ++i) {
		example_data.at(i) = static_cast<char>(i);
	}

	const size_t written = jack_ringbuffer_write(
		buf.get(), example_data.data(), example_data.size()
	);
	REQUIRE(written == example_data.size());

	RingbufferReadIterator iter{buf.get()};

	SECTION("Newly created iterators")
	{
		RingbufferReadIterator iter2{buf.get()};

		SECTION("Are equal")
		{
			REQUIRE(iter == iter2);
		}

		SECTION("Read the first data in the buffer")
		{
			const char first_element = example_data.at(0);
			REQUIRE(*iter == first_element);
			REQUIRE(*iter == *iter2);
		}

		SECTION("Are readable")
		{
			REQUIRE(iter.is_readable());
			REQUIRE(iter2.is_readable());
			REQUIRE(iter);
			REQUIRE(iter2);
		}
	}

	SECTION("Incrementing iterator")
	{
		RingbufferReadIterator iter2{buf.get()};
		++iter2;
		++iter2;
		++iter2;

		SECTION("Increases distance")
		{
			REQUIRE(iter2 - iter == 3);
		}

		SECTION("Makes not equal to un-incremented iterator")
		{
			REQUIRE(iter2 != iter);
		}

		SECTION("Reads the nth element in the buffer")
		{
			const char fourth_element = example_data.at(3);
			REQUIRE(*iter2 == fourth_element);
		}

		SECTION("Is readable")
		{
			REQUIRE(iter2.is_readable());
			REQUIRE(iter2);
		}

		SECTION("Past available read buffer")
		{
			for (size_t i = 0; i < 2 * DATA_SIZE; ++i)
				++iter2;

			SECTION("Causes std::out_of_range on dereference")
			{
				REQUIRE_THROWS_AS(*iter2, std::out_of_range);
			}

			SECTION("Is no longer readable")
			{
				REQUIRE(!iter2.is_readable());
				REQUIRE(!iter2);
			}
		}
	}

	SECTION("Advancing readable buffer segment to overroll region")
	{
		// Write pointer is now positioned so writing the example data
		// again causes an overroll
		jack_ringbuffer_write_advance(
			buf.get(), BUF_SIZE - (DATA_SIZE + DATA_SIZE / 2)
		);
		jack_ringbuffer_read_advance(
			buf.get(), BUF_SIZE - DATA_SIZE / 2
		);
		const std::size_t written = jack_ringbuffer_write(
			buf.get(), example_data.data(), example_data.size()
		);
		REQUIRE(written == example_data.size());

		SECTION("Reads data correctly")
		{
			RingbufferReadIterator it{buf.get()};
			for (std::size_t i = 0; i < DATA_SIZE; ++i, ++it) {
				REQUIRE(*it == example_data.at(i));
			}
		}

		SECTION("Iterators stay readable past overroll")
		{
			for (RingbufferReadIterator it{buf.get()};
			     it !=
			     RingbufferReadIterator{
				     buf.get(),
				     RingbufferReadIterator::end_iter};
			     ++it) {
				REQUIRE(it.is_readable());
				REQUIRE(it);
			}
		}

		SECTION("Read until end iterator reads correctly")
		{
			std::size_t items_read{0};
			for (RingbufferReadIterator it{buf.get()};
			     it !=
			     RingbufferReadIterator{
				     buf.get(),
				     RingbufferReadIterator::end_iter};
			     ++it, ++items_read) {
				// do nothing
			}

			REQUIRE(items_read == example_data.size());
		}
	}

	SECTION("End iterator")
	{
		RingbufferReadIterator end_iter{
			buf.get(), RingbufferReadIterator::end_iter};

		SECTION("Isn't equal to normal iterator")
		{
			REQUIRE(end_iter != iter);
		}

		SECTION("Is equal to other end iterator")
		{
			RingbufferReadIterator end_iter2{
				buf.get(), RingbufferReadIterator::end_iter};
			REQUIRE(end_iter == end_iter2);
		}

		SECTION("Is not readable")
		{
			REQUIRE(!end_iter.is_readable());
			REQUIRE(!end_iter);
		}

		SECTION("Causes std::out_of_range on dereference")
		{
			REQUIRE_THROWS_AS(*end_iter, std::out_of_range);
		}
	}

	SECTION("Advancing ringbuf by iterator")
	{
		constexpr static std::size_t increment = 5;
		const std::size_t pre_increment_distance =
			end(buf.get()) - begin(buf.get());

		RingbufferReadIterator new_begin = begin(buf.get());
		new_begin += increment;
		new_begin.advance_read_ptr();

		SECTION("Reduces distance from begin to end")
		{
			REQUIRE(end(buf.get()) - begin(buf.get()) ==
				pre_increment_distance - increment);
		}
	}
}
