#include <catch2/generators/catch_generators.hpp>
#include <limits>
#include <linux/hidraw.h>

#include "tkf1/F1Device.hpp"

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators_all.hpp>

TEST_CASE("F1Device::is_f1_device")
{
	const std::uint32_t bustype = GENERATE(
		take(3,
		     random(std::numeric_limits<std::uint32_t>::min(),
			    std::numeric_limits<std::uint32_t>::max()))
	);

	struct hidraw_devinfo devinfo {
		.bustype = bustype, .vendor = F1Device::HID_VENDOR_ID,
		.product = F1Device::HID_PRODUCT_ID,
	};

	SECTION("valid device")
	{
		REQUIRE(F1Device::is_f1_device(devinfo));
	}

	SECTION("invalid device")
	{
		SECTION("vendor mismatch")
		{
			const std::int16_t invalid_vendor = GENERATE(take(
				3,
				random(std::numeric_limits<std::int16_t>::min(),
				       std::int16_t{0x17cbU})
			));
			devinfo.vendor = invalid_vendor;
			REQUIRE_FALSE(F1Device::is_f1_device(devinfo));
		}

		SECTION("product mismatch")
		{
			const std::int16_t invalid_product = GENERATE(take(
				3,
				random(std::numeric_limits<std::int16_t>::min(),
				       std::int16_t{0x111fU})
			));
			devinfo.product = invalid_product;
			REQUIRE_FALSE(F1Device::is_f1_device(devinfo));
		}
	}
}

TEST_CASE("F1Device::matrix_pos x/y to idx")
{
	const std::uint8_t x = GENERATE(range(0, 3));
	const std::uint8_t y = GENERATE(range(0, 3));
	REQUIRE(F1Device::matrix_pos(x, y) == y * 4 + x);
}

TEST_CASE("F1Device::special_btn_name")
{
	std::uint8_t idx = std::numeric_limits<std::uint8_t>::max();
	std::string name;

	SECTION("known names")
	{
		const std::pair<unsigned, const char*> idx_name = GENERATE(
			std::make_pair(0U, "WHEEL"),
			std::make_pair(1U, "BROWSE"),
			std::make_pair(2U, "SIZE"),
			std::make_pair(3U, "TYPE"),
			std::make_pair(4U, "REVERSE"),
			std::make_pair(5U, "SHIFT"),
			std::make_pair(6U, "CAPTURE"),
			std::make_pair(7U, "QUANT"),
			std::make_pair(8U, "SYNC")
		);
		idx = idx_name.first;
		name = idx_name.second;
	}

	SECTION("unknown names")
	{
		const unsigned idx2 = GENERATE(take(3, random(9U, 255U)));
		idx = static_cast<std::uint8_t>(idx2);
		name = "UNKNOWN";
	}

	REQUIRE(F1Device::special_btn_name(idx) == name);
}

TEST_CASE("F1Device::num_to_segments")
{
	SECTION("valid numbers")
	{
		const std::uint8_t hi_digit = GENERATE(range(0, 9));
		const std::uint8_t lo_digit = GENERATE(range(0, 9));
		const std::uint8_t val = hi_digit * 10 + lo_digit;

		F1Device::SegmentChar hi_char;
		F1Device::SegmentChar lo_char;

		// NOLINTBEGIN(*-magic-numbers)
		switch (hi_digit) {
		case 0:
			hi_char = F1Device::SegmentChar::D0;
			break;
		case 1:
			hi_char = F1Device::SegmentChar::D1;
			break;
		case 2:
			hi_char = F1Device::SegmentChar::D2;
			break;
		case 3:
			hi_char = F1Device::SegmentChar::D3;
			break;
		case 4:
			hi_char = F1Device::SegmentChar::D4;
			break;
		case 5:
			hi_char = F1Device::SegmentChar::D5;
			break;
		case 6:
			hi_char = F1Device::SegmentChar::D6;
			break;
		case 7:
			hi_char = F1Device::SegmentChar::D7;
			break;
		case 8:
			hi_char = F1Device::SegmentChar::D8;
			break;
		case 9:
			hi_char = F1Device::SegmentChar::D9;
			break;
		default:
			assert(false);
		}

		switch (lo_digit) {
		case 0:
			lo_char = F1Device::SegmentChar::D0;
			break;
		case 1:
			lo_char = F1Device::SegmentChar::D1;
			break;
		case 2:
			lo_char = F1Device::SegmentChar::D2;
			break;
		case 3:
			lo_char = F1Device::SegmentChar::D3;
			break;
		case 4:
			lo_char = F1Device::SegmentChar::D4;
			break;
		case 5:
			lo_char = F1Device::SegmentChar::D5;
			break;
		case 6:
			lo_char = F1Device::SegmentChar::D6;
			break;
		case 7:
			lo_char = F1Device::SegmentChar::D7;
			break;
		case 8:
			lo_char = F1Device::SegmentChar::D8;
			break;
		case 9:
			lo_char = F1Device::SegmentChar::D9;
			break;
		default:
			assert(false);
		}
		// NOLINTEND(*-magic-numbers)

		const auto& [hi, lo] = F1Device::num_to_segments(val);
		CHECK(hi == hi_char);
		REQUIRE(lo == lo_char);
	}

	SECTION("invalid numbers")
	{
		const std::uint8_t val = GENERATE(
			take(3,
			     random(F1Device::MAX_SEGMENT_NUM,
				    std::numeric_limits<std::uint8_t>::max()))
		);
		REQUIRE_THROWS(F1Device::num_to_segments(val));
	}
}
