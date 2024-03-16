#include <array>
#include <vector>

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include "io/MidiEvent.hpp"

// NOLINTBEGIN(*-union-access,*-magic-numbers)

TEST_CASE("MidiEvent creates NOTE OFF event", "[midevent]")
{
	const MidiEvent e = MidiEvent::note_off("C#1", 124, 13);
	REQUIRE(e.type == MidiEvent::Type::NOTE_OFF);
	REQUIRE(e.channel == 13);
	REQUIRE(e.data.note.note == MidiEvent::note_from_name_c("C#1"));
	REQUIRE(e.data.note.velocity == 124);

	SECTION("builds MIDI bytes")
	{
		auto raw = e.to_bytes();
		REQUIRE(raw.at(0) ==
			(static_cast<MidiEvent::byte>(MidiEvent::Type::NOTE_OFF
			 ) |
			 13U));
		REQUIRE(raw.at(1) == MidiEvent::note_from_name_c("C#1"));
		REQUIRE(raw.at(2) == 124);
	}
}

TEST_CASE("MidiEvent creates NOTE ON event", "[midevent]")
{
	const MidiEvent e = MidiEvent::note_on("C#1", 124, 13);
	REQUIRE(e.type == MidiEvent::Type::NOTE_ON);
	REQUIRE(e.channel == 13);
	REQUIRE(e.data.note.note == MidiEvent::note_from_name_c("C#1"));
	REQUIRE(e.data.note.velocity == 124);

	SECTION("builds MIDI bytes")
	{
		auto raw = e.to_bytes();
		REQUIRE(raw.at(0) ==
			(static_cast<MidiEvent::byte>(MidiEvent::Type::NOTE_ON
			 ) |
			 13U));
		REQUIRE(raw.at(1) == MidiEvent::note_from_name_c("C#1"));
		REQUIRE(raw.at(2) == 124);
	}
}

TEST_CASE("MidiEvent creates CC event", "[midevent]")
{
	const MidiEvent e = MidiEvent::control_change(12, 24, 13);
	REQUIRE(e.type == MidiEvent::Type::CONTROL_CHANGE);
	REQUIRE(e.channel == 13);
	REQUIRE(e.data.controller.controller == 12);
	REQUIRE(e.data.controller.value == 24);

	SECTION("builds MIDI bytes")
	{
		auto raw = e.to_bytes();
		REQUIRE(raw.at(0) == (static_cast<MidiEvent::byte>(
					      MidiEvent::Type::CONTROL_CHANGE
				      ) |
				      13U));
		REQUIRE(raw.at(1) == 12);
		REQUIRE(raw.at(2) == 24);
	}
}

TEMPLATE_TEST_CASE_SIG(
	"MidiEvent parses Note MIDI events",
	"[midievent][parser][io]",
	((typename T, T raw_data, MidiEvent::Type t, MidiEvent::byte chan),
	 T,
	 raw_data,
	 t,
	 chan),
	(std::array<MidiEvent::byte, 3>,
	 std::array<MidiEvent::byte, 3>{0x83, 0x12, 0x00},
	 MidiEvent::Type::NOTE_OFF,
	 3),
	(std::array<MidiEvent::byte, 5>,
	 std::array<MidiEvent::byte, 5>{0x85, 0x12, 0x12, 0x13, 0x14},
	 MidiEvent::Type::NOTE_OFF,
	 5),
	(std::array<MidiEvent::byte, 3>,
	 std::array<MidiEvent::byte, 3>{0x93, 0x15, 0x74},
	 MidiEvent::Type::NOTE_ON,
	 3)
)
{
	const auto [e, _] = MidiEvent::parse(raw_data.begin(), raw_data.end());

	REQUIRE(e.type == t);
	REQUIRE(e.channel == chan);
	REQUIRE(e.data.note.note == raw_data.at(1));
	REQUIRE(e.data.note.velocity == raw_data.at(2));
}

TEST_CASE("MidiEvent parses CC MIDI events", "[midievent][parser]")
{
	std::vector<int> raw_data{0xbf, 0x22, 0x7f};

	SECTION("With trailing MIDI data")
	{
		for (auto x : {0x85, 0x00, 0x25, 0x73})
			raw_data.push_back(x);
	}

	const auto [e, data_end_it] =
		MidiEvent::parse(raw_data.begin(), raw_data.end());

	REQUIRE(e.type == MidiEvent::Type::CONTROL_CHANGE);
	REQUIRE(e.channel == 15);
	REQUIRE(e.data.controller.controller == 0x22);
	REQUIRE(e.data.controller.value == 0x7f);

	SECTION("With trailing MIDI data")
	{
		REQUIRE(data_end_it == raw_data.begin() + 3);
	}
}

TEST_CASE("MidiEvent compares", "[midievent]")
{
	const MidiEvent lhs{MidiEvent::Type::NOTE_OFF, 4, {{12, 14}}};
	MidiEvent rhs = lhs;

	SECTION("Compares equal on all members equal")
	{
		REQUIRE(lhs == rhs);
	}

	SECTION("Compares not equal")
	{
		SECTION("On type diff")
		{
			rhs.type = MidiEvent::Type::NOTE_ON;
		}

		SECTION("On channel diff")
		{
			rhs.channel = 12;
		}

		SECTION("On data1 diff")
		{
			rhs.data.raw.at(0) = 32;
		}

		SECTION("On data2 diff")
		{
			rhs.data.raw.at(1) = 33;
		}

		REQUIRE(lhs != rhs);
	}
}

TEST_CASE("MidiEvent retrieves note numbers from names", "[midievent]")
{
	const std::pair<const char*, int> note = GENERATE(
		std::make_pair("C0", 12),
		std::make_pair("C#0", 13),
		std::make_pair("E1", 28),
		std::make_pair("D#2", 39),
		std::make_pair("C3", 48),
		std::make_pair("Gb3", 54),
		std::make_pair("c0", 12),
		std::make_pair("c#0", 13),
		std::make_pair("e1", 28),
		std::make_pair("d#2", 39),
		std::make_pair("c3", 48),
		std::make_pair("gb3", 54)
	);
	const auto& [note_name, note_value] = note;

	REQUIRE(static_cast<int>(MidiEvent::note_from_name(note_name)) ==
		note_value);
}

// NOLINTEND(*-union-access,*-magic-numbers)
