#include <vector>

#include <catch2/catch_test_macros.hpp>

#include "io/MidiEvent.hpp"
#include "io/MidiStream.hpp"

TEST_CASE("MidiStream", "[midistream][io]")
{
	std::size_t callback_called{0};
	MidiStream stream{1024};

	const std::vector<MidiEvent::byte> example_data{
		// A series of zero bytes at the beginning should be no problem
		0x00,
		0x00,
		0x00,
		0x00,
		0x80, // Note off, note 01, vel 00
		0x01,
		0x00,
		0x90, // Note on, note 01, vel 7f
		0x01,
		0x7f,
		0xb0, // CC04 = 7f
		0x04,
		0x7f,
		0xb0, // CC05 = 60
		0x05,
		0x60,
		0xb0, // CC04 = 00
		0x04,
		0x00,
		0xb0, // CC05 = 00
		0x05,
		0x00,
		// A series of zero bytes at the end should be no problem
		0x00,
		0x00,
		0x00,
		0x00,
	};

	const std::size_t written = stream.write(
		reinterpret_cast<const char*>(example_data.data()),
		example_data.size()
	);
	REQUIRE(written == example_data.size());

	SECTION("Write notifies callback")
	{
		stream.set_write_notification_callback(
			[&]([[maybe_unused]] MidiStream&) { ++callback_called; }
		);
		stream.write(
			reinterpret_cast<const char*>(example_data.data()),
			example_data.size()
		);
		REQUIRE(callback_called == 1);
	}

	MidiStream::iterator iter = stream.begin();

	SECTION("Begin iterator")
	{
		SECTION("Dereferences to first MidiEvent")
		{
			const MidiEvent expected{
				MidiEvent::Type::NOTE_OFF, 0, 0x01, 0x00
			};
			REQUIRE(*iter == expected);
		}
	}

	SECTION("Incremented iterator")
	{
		++iter;

		SECTION("Dereferences to next MidiEvent")
		{
			const MidiEvent expected{
				MidiEvent::Type::NOTE_ON, 0, 0x01, 0x7f
			};
			REQUIRE(*iter == expected);
		}

		SECTION("Advances ringbuffer on advance")
		{
			const auto pre_advance_distance =
				std::distance(stream.begin(), stream.end());
			iter.advance_read_ptr();
			REQUIRE(std::distance(stream.begin(), stream.end()) ==
				pre_advance_distance - 1);
		}
	}

	SECTION("Iterating through until end parses all MidiEvents")
	{
		const std::vector<MidiEvent> expected{
			{MidiEvent::Type::NOTE_OFF, 0, 0x01, 0x00},
			{MidiEvent::Type::NOTE_ON, 0, 0x01, 0x7f},
			{MidiEvent::Type::CONTROL_CHANGE, 0, 0x04, 0x7f},
			{MidiEvent::Type::CONTROL_CHANGE, 0, 0x05, 0x60},
			{MidiEvent::Type::CONTROL_CHANGE, 0, 0x04, 0x00},
			{MidiEvent::Type::CONTROL_CHANGE, 0, 0x05, 0x00},
		};

		auto expected_iter = expected.begin();
		for (const auto& event : stream) {
			REQUIRE(expected_iter != expected.end());
			REQUIRE(*expected_iter == event);
			++expected_iter;
		}
		REQUIRE(expected_iter == expected.end());
	}
}
