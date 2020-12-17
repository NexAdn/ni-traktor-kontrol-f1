#pragma once

#include <cassert>
#include <string>
#include <vector>

/**
 * Representation of a signal within the NSM environment
 *
 * This class represents both local and remote signals (i.e. our signals and
 * signals other peers have offered us).
 *
 * Signals are simple (path, min, max, default, direction) tuples representing
 * some input or output of the application. Signals can be connected to other
 * signals (i.e. IN signals to OUT signals) to create a signal flow across
 * applications which makes controlling one application from another possible
 * (which is what we want!).
 *
 * This class does not implement any semantics itself (other than string
 * conversion of the {@link NonSignal::Direction} value).
 */
class NonSignal
{
public:
	enum class Direction
	{
		IN,
		OUT,
		BIDIRECTIONAL
	};

	inline auto direction_str() const
	{
		return direction_str(direction);
	}
	/**
	 * Convert the direction value to a string as needed by the Non API
	 */
	static std::string direction_str(Direction direction)
	{
		switch (direction) {
		case Direction::IN:
			return "in";
		case Direction::OUT:
		case Direction::BIDIRECTIONAL:
			return "out";
		}

		return "INVALID";
	}

	const std::string path;
	const float min;
	const float max;
	const float default_value;
	const Direction direction{Direction::OUT};
};

using NonSignalList = std::vector<NonSignal>;

/**
 * Default values for the Traktor Kontrol F1
 */
namespace F1Default
{
const NonSignalList signals{
  {"/kontrolf1/mtx/0", 0.f, 1.f, 0.f, NonSignal::Direction::OUT},
  {"/kontrolf1/mtx/1", 0.f, 1.f, 0.f, NonSignal::Direction::OUT},
  {"/kontrolf1/mtx/2", 0.f, 1.f, 0.f, NonSignal::Direction::OUT},
  {"/kontrolf1/mtx/3", 0.f, 1.f, 0.f, NonSignal::Direction::OUT},
  {"/kontrolf1/mtx/4", 0.f, 1.f, 0.f, NonSignal::Direction::OUT},
  {"/kontrolf1/mtx/5", 0.f, 1.f, 0.f, NonSignal::Direction::OUT},
  {"/kontrolf1/mtx/6", 0.f, 1.f, 0.f, NonSignal::Direction::OUT},
  {"/kontrolf1/mtx/7", 0.f, 1.f, 0.f, NonSignal::Direction::OUT},
  {"/kontrolf1/mtx/8", 0.f, 1.f, 0.f, NonSignal::Direction::OUT},
  {"/kontrolf1/mtx/9", 0.f, 1.f, 0.f, NonSignal::Direction::OUT},
  {"/kontrolf1/mtx/10", 0.f, 1.f, 0.f, NonSignal::Direction::OUT},
  {"/kontrolf1/mtx/11", 0.f, 1.f, 0.f, NonSignal::Direction::OUT},
  {"/kontrolf1/mtx/12", 0.f, 1.f, 0.f, NonSignal::Direction::OUT},
  {"/kontrolf1/mtx/13", 0.f, 1.f, 0.f, NonSignal::Direction::OUT},
  {"/kontrolf1/mtx/14", 0.f, 1.f, 0.f, NonSignal::Direction::OUT},
  {"/kontrolf1/mtx/15", 0.f, 1.f, 0.f, NonSignal::Direction::OUT},
  {"/kontrolf1/stop/0", 0.f, 1.f, 0.f, NonSignal::Direction::OUT},
  {"/kontrolf1/stop/1", 0.f, 1.f, 0.f, NonSignal::Direction::OUT},
  {"/kontrolf1/stop/2", 0.f, 1.f, 0.f, NonSignal::Direction::OUT},
  {"/kontrolf1/stop/3", 0.f, 1.f, 0.f, NonSignal::Direction::OUT},
  {"/kontrolf1/special/0", 0.f, 1.f, 0.f, NonSignal::Direction::OUT},
  {"/kontrolf1/special/1", 0.f, 1.f, 0.f, NonSignal::Direction::OUT},
  {"/kontrolf1/special/2", 0.f, 1.f, 0.f, NonSignal::Direction::OUT},
  {"/kontrolf1/special/3", 0.f, 1.f, 0.f, NonSignal::Direction::OUT},
  {"/kontrolf1/special/4", 0.f, 1.f, 0.f, NonSignal::Direction::OUT},
  {"/kontrolf1/special/5", 0.f, 1.f, 0.f, NonSignal::Direction::OUT},
  {"/kontrolf1/special/6", 0.f, 1.f, 0.f, NonSignal::Direction::OUT},
  {"/kontrolf1/special/7", 0.f, 1.f, 0.f, NonSignal::Direction::OUT},
  {"/kontrolf1/special/8", 0.f, 1.f, 0.f, NonSignal::Direction::OUT},
  {"/kontrolf1/knob/0", 0.f, 1.f, 0.f, NonSignal::Direction::OUT},
  {"/kontrolf1/knob/1", 0.f, 1.f, 0.f, NonSignal::Direction::OUT},
  {"/kontrolf1/knob/2", 0.f, 1.f, 0.f, NonSignal::Direction::OUT},
  {"/kontrolf1/knob/3", 0.f, 1.f, 0.f, NonSignal::Direction::OUT},
  {"/kontrolf1/fader/0", 0.f, 1.f, 0.f, NonSignal::Direction::OUT},
  {"/kontrolf1/fader/1", 0.f, 1.f, 0.f, NonSignal::Direction::OUT},
  {"/kontrolf1/fader/2", 0.f, 1.f, 0.f, NonSignal::Direction::OUT},
  {"/kontrolf1/fader/3", 0.f, 1.f, 0.f, NonSignal::Direction::OUT},
};
}
