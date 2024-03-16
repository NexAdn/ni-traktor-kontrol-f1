#include <cstddef>
#include <random>
#include <stdexcept>

#include "io/Ringbuffer.hpp"

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators_all.hpp>

// NOLINTBEGIN(*-magic-numbers)

TEST_CASE("Ringbuffer", "[ringbuffer]")
{
	const std::size_t buffer_size = GENERATE(take(10, random(16, 1024)));
	Ringbuffer<int> buf{buffer_size};

	REQUIRE(buf.size() == 0); // NOLINT(readability-container-size-empty)
	REQUIRE(buf.empty());
	REQUIRE(buf.capacity() == buffer_size);

	SECTION("Adding elements increases size")
	{
		std::vector<int> elements{0, 42, 1337};
		for (auto& element : elements) {
			buf.push(element);
		}

		REQUIRE(buf.size() == elements.size());

		SECTION("pop() returns first pushed element and decreases "
			"size()")
		{
			const std::size_t prev_size = buf.size();
			const int popped = buf.pop();
			REQUIRE(popped == elements.at(0));
			REQUIRE(buf.size() == prev_size - 1);
		}
	}

	REQUIRE_FALSE(buf.full());

	SECTION("Capacity limits")
	{
		SECTION("Writing infinitely throws eventually")
		{
			while (not buf.full()) {
				buf.push({});
			}

			REQUIRE(buf.full());
			REQUIRE_THROWS_AS(buf.push({}), std::out_of_range);
		}

		SECTION("Reading infinitely throws eventually")
		{
			while (not buf.empty()) {
				buf.pop();
			}

			REQUIRE(buf.empty());
			REQUIRE_THROWS_AS(buf.pop(), std::out_of_range);
		}
	}

	SECTION("Copy and move operations")
	{
		// NOLINTBEGIN(*-unnecessary-copy-initialization)

		// Get some example data into the buffer so it is not in a
		// default constructed state
		const std::size_t pre_insert_elements =
			GENERATE(take(10, random(1, 100)));
		std::mt19937 rng{Catch::Generators::Detail::getSeed()};
		for (std::size_t i = 0;
		     i < pre_insert_elements && not buf.full();
		     ++i) {
			buf.push(static_cast<int>(rng()));
		}

		SECTION("Copies to new buffer")
		{
			Ringbuffer<int> new_buf = buf;
			REQUIRE(new_buf.size() == buf.size());
			REQUIRE(new_buf.empty() == buf.empty());
			REQUIRE(new_buf.full() == buf.full());
			REQUIRE(new_buf.capacity() == buf.capacity());
			REQUIRE(new_buf.pop() == buf.pop());
		}

		SECTION("Copy-assigns to new buffer")
		{
			const std::size_t initial_size =
				GENERATE(take(10, random(1, 1024)));
			Ringbuffer<int> new_buf{initial_size};
			new_buf = buf;
			REQUIRE(new_buf.size() == buf.size());
			REQUIRE(new_buf.empty() == buf.empty());
			REQUIRE(new_buf.full() == buf.full());
			REQUIRE(new_buf.capacity() == buf.capacity());
			REQUIRE(new_buf.pop() == buf.pop());
		}

		SECTION("Moves")
		{
			const std::size_t old_size = buf.size();
			const bool old_empty = buf.empty();
			const bool old_full = buf.full();
			const std::size_t old_capacity = buf.capacity();

			SECTION("Move-constructs to new buffer")
			{
				Ringbuffer<int> new_buf = std::move(buf);
				REQUIRE(new_buf.size() == old_size);
				REQUIRE(new_buf.empty() == old_empty);
				REQUIRE(new_buf.full() == old_full);
				REQUIRE(new_buf.capacity() == old_capacity);
			}

			SECTION("Move-assigns to new buffer")
			{
				const std::size_t initial_size =
					GENERATE(take(10, random(1, 1024)));
				Ringbuffer<int> new_buf{initial_size};
				new_buf = std::move(buf);
				REQUIRE(new_buf.size() == old_size);
				REQUIRE(new_buf.empty() == old_empty);
				REQUIRE(new_buf.full() == old_full);
				REQUIRE(new_buf.capacity() == old_capacity);
			}
		}

		// NOLINTEND(*-unnecessary-copy-initialization)
	}
}

// NOLINTEND(*-magic-numbers)
