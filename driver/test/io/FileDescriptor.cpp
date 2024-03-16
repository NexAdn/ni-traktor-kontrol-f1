#include <limits>

#include "io/FileDescriptor.hpp"

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators_all.hpp>

namespace
{
// NOLINTBEGIN(*-global-variables)
thread_local int closed_fd = -1;
thread_local int close_times = 0;
thread_local int close_res = 0;

int close_test(int fd)
{
	closed_fd = fd;
	close_times++;
	return close_res;
}
// NOLINTEND(*-global-variables)
} // namespace

using fd_t = FileDescriptor<decltype(close_test)>;

TEST_CASE("FileDescriptor")
{
	close_res = 0;
	close_times = 0;
	const int fd_num =
		GENERATE(take(3, random(1, std::numeric_limits<int>::max())));

	{
		fd_t fd{fd_num, close_test};

		REQUIRE(fd);

		SECTION("move construction")
		{
			fd_t other_fd{std::move(fd)};
			REQUIRE(other_fd);
			REQUIRE_FALSE(fd);
		}

		SECTION("operator= move")
		{
			fd_t other_fd{-1};
			other_fd = std::move(fd);
			REQUIRE(other_fd);
			REQUIRE(other_fd);
		}

		SECTION("operator*")
		{
			REQUIRE(*fd == fd_num);
		}

		SECTION("const operator*")
		{
			const fd_t other_fd{std::move(fd)};
			REQUIRE(*other_fd == fd_num);
		}

		SECTION("close()")
		{
			SECTION("closes")
			{
				fd.close();
				REQUIRE(*fd < 0);
				REQUIRE(close_times == 1);
				REQUIRE(closed_fd == fd_num);
				REQUIRE_FALSE(fd);
			}

			SECTION("throws on close error")
			{
				close_res = -1;
				REQUIRE_THROWS(fd.close());
				close_res = 0;
				// Close failed, so we need to repeat
				close_times--;
			}
		}
	}

	REQUIRE(closed_fd == fd_num);
	REQUIRE(close_times == 1);
}
