#include "MidiEvent.hpp"

std::ostream& operator<<(std::ostream& os, const MidiEvent& e)
{
	os << "MIDI[";
	switch (e.type) {
	case MidiEvent::Type::NOTE_OFF:
		os << "Note Off";
		break;
	case MidiEvent::Type::NOTE_ON:
		os << "Note On";
		break;
	case MidiEvent::Type::POLYPHONIC_AFTERTOUCH:
		os << "PAftertouch";
		break;
	case MidiEvent::Type::CONTROL_CHANGE:
		os << "CC";
		break;
	case MidiEvent::Type::PROGRAM_CHANGE:
		os << "PC";
		break;
	case MidiEvent::Type::MONOPHONIC_AFTERTOUCH:
		os << "MAftertouch";
		break;
	case MidiEvent::Type::PITCH_BEND:
		os << "Pitch Bend";
		break;
	case MidiEvent::Type::SYSTEM_MESSAGE:
		os << "SysMsg";
		break;
	}
	os << ' ' << std::hex << static_cast<int>(e.channel) << ':';

	if (e.type == MidiEvent::Type::SYSTEM_MESSAGE) {
		assert(e.control_message_data);
		for (MidiEvent::byte b : *(e.control_message_data))
			os << ' ' << static_cast<int>(b);
	} else {
		// NOLINTBEGIN(*-union-access)
		os << ' ' << static_cast<int>(e.data.raw[0]) << '/'
		   << static_cast<int>(e.data.raw[1]);
		// NOLINTEND(*-union-access)
	}
	os << ']' << std::dec;
	return os;
}
