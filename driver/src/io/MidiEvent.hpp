#pragma once

#include <array>
#include <cassert>
#include <cstdint>
#include <iterator>
#include <ostream>
#include <tuple>

class MidiEvent final
{
public:
	using byte = uint8_t;

	constexpr static byte STATUS_BYTE_MASK{0x80};
	constexpr static byte TYPE_MASK{0xf0};
	constexpr static byte CHANNEL_MASK{0x0f};

	enum class Type : uint8_t {
		NOTE_OFF = 0x80,
		NOTE_ON = 0x90,
		POLYPHONIC_AFTERTOUCH = 0xa0,
		CONTROL_CHANGE = 0xb0,
		PROGRAM_CHANGE = 0xc0,
		MONOPHONIC_AFTERTOUCH = 0xd0,
		PITCH_BEND = 0xe0,
		SYSTEM_MESSAGE = 0xf0
	};

	constexpr MidiEvent() = default;
	constexpr MidiEvent(
		Type type,
		std::uint8_t channel,
		std::uint8_t data1,
		std::uint8_t data2
	) :
		type(type), channel(channel), data({.raw = {data1, data2}})
	{
	}

	MidiEvent(const MidiEvent&) = default;
	MidiEvent& operator=(const MidiEvent&) = default;
	MidiEvent(MidiEvent&&) = default;
	MidiEvent& operator=(MidiEvent&&) = default;
	~MidiEvent() = default;

	constexpr static MidiEvent
	note_off(std::string_view name, byte velocity, byte channel = 0)
	{
		return MidiEvent{
			Type::NOTE_OFF, channel, note_from_name(name), velocity
		};
	}
	constexpr static MidiEvent
	note_on(std::string_view name, byte velocity, byte channel = 0)
	{
		return MidiEvent{
			Type::NOTE_ON, channel, note_from_name(name), velocity
		};
	}
	constexpr static MidiEvent
	control_change(byte controller, byte value, byte channel = 0)
	{
		return MidiEvent{
			Type::CONTROL_CHANGE, channel, controller, value
		};
	}

	template <typename iter>
	static std::tuple<MidiEvent, iter> parse(iter raw_it, iter end)
	{
		MidiEvent event;

		for (; raw_it != end && !is_status_byte(*raw_it); ++raw_it)
			;

		if (raw_it == end)
			return {event, end};

		assert(std::distance(raw_it, end) != 0);
		const byte status = static_cast<byte>(*raw_it);
		assert(is_status_byte(status));

		event.type = static_cast<Type>(status & TYPE_MASK);
		event.channel = status & CHANNEL_MASK;

		++raw_it;
		if (event.type == MidiEvent::Type::SYSTEM_MESSAGE) {
			for (; raw_it != end &&
			       !is_status_byte(static_cast<byte>(*raw_it));
			     ++raw_it)
				;
		} else {
			// NOLINTBEGIN(*-pointer-arithmetic,*-union-access)
			for (auto* data_it = event.data.raw.begin();
			     data_it != event.data.raw.end() && raw_it != end &&
			     !is_status_byte(static_cast<byte>(*raw_it));
			     ++data_it, ++raw_it) {
				*data_it =
					static_cast<MidiEvent::byte>(*raw_it);
			}
			// NOLINTEND(*-pointer-arithmetic,*-union-access)
		}

		for (; raw_it != end && !is_status_byte(*raw_it); ++raw_it)
			;

		return {event, raw_it};
	}

	[[nodiscard]] constexpr std::array<byte, 3> to_bytes() const
	{
		assert(type != Type::SYSTEM_MESSAGE);
		// NOLINTBEGIN(*-union-access)
		return {static_cast<byte>(static_cast<byte>(type) | channel),
			data.raw[0],
			data.raw[1]};
		// NOLINTEND(*-union-access)
	}

	constexpr static bool is_status_byte(byte b)
	{
		return (b & STATUS_BYTE_MASK) != 0;
	}

	constexpr static byte note_from_name(std::string_view name)
	{
		assert(name.size() >= 2 && name.size() <= 3);
		static_assert(NOTE_NAME_VALUE.size() == NOTE_VALUE_MODULUS);

		const byte note_base_column = name[0] % NOTE_VALUE_MODULUS;
		assert(note_base_column >= 0 &&
		       note_base_column <= NOTE_NAME_VALUE.size());
		// NOLINTNEXTLINE(*-array-index)
		const byte note_base_val = NOTE_NAME_VALUE[note_base_column];
		const byte note_val = note_base_val + (name[1] == 'b'   ? -1
						       : name[1] == '#' ? 1
									: 0);

		// Doing some ASCII magic to get the numeric value from
		// ASCII chars fast. ASCII digits are just 0x30 to 0x39.
		constexpr unsigned char ASCII_NUMBER_MASK{0x30};
		constexpr unsigned char ASCII_LOWER_4_BITS{0x0F};

		unsigned char octave_char;
		if (name.size() == 2) {
			// two letter name: <letter><octave>
			octave_char = name[1];
		} else {
			assert(name.size() == 3);
			// second char must be # or b:
			// <letter><#b><octave>
			octave_char = name[2];
		}

		// NOLINTBEGIN(*-signed-bitwise)
		assert((octave_char & ASCII_NUMBER_MASK) == ASCII_NUMBER_MASK);
		const byte octave = (octave_char & ASCII_LOWER_4_BITS);
		// NOLINTEND(*-signed-bitwise)

		return (octave * NOTES_PER_OCTAVE) + note_val +
		       MIDI_OCTAVE_OFFSET;
	}

	/**
	 * The same as the constexpr variant, but
	 * enforcing compile-time evaluation
	 */
	consteval static byte note_from_name_c(const char* name)
	{
		return note_from_name(name);
	}

	Type type{Type::NOTE_OFF};
	byte channel{0};
	// MIDI has at most two bytes of data, except for control messages
	union {
		std::array<byte, 2> raw{0, 0};
		struct {
			byte note;
			byte velocity;
		} note;
		struct {
			byte controller;
			byte value;
		} controller;
		struct {
			byte program;
			byte unused0;
		} program;
		struct {
			byte velocity;
			byte unused0;
		} mono_aftertouch;
		struct {
			uint16_t bend;
		} pitch_bend;
	} data;

private:
	// NOLINTBEGIN(*-magic-numbers)
	// ASCII char to note value in octave -1, starting at C-1
	constexpr static std::array<byte, 8> NOTE_NAME_VALUE{
		0, 9, 11, 0, 2, 4, 5, 7, // *ABCDEFG
	};
	// NOLINTEND(*-magic-numbers)
	constexpr static byte NOTE_VALUE_MODULUS{8};
	constexpr static byte NOTES_PER_OCTAVE{12};
	// The piano octaves start at little higher than
	// where the MIDI octaves start
	constexpr static byte MIDI_OCTAVE_OFFSET{12};
};

constexpr bool operator==(const MidiEvent& lhs, const MidiEvent& rhs)
{
	// NOLINTBEGIN(*-union-access)
	return lhs.type == rhs.type && lhs.channel == rhs.channel &&
	       lhs.data.raw.at(0) == rhs.data.raw.at(0) &&
	       lhs.data.raw.at(1) == rhs.data.raw.at(1);
	// NOLINTEND(*-union-access)
}

std::ostream& operator<<(std::ostream& os, const MidiEvent& e);
