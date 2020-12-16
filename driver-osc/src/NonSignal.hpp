#pragma once

#include <cassert>
#include <string>
#include <string_view>
#include <vector>

class NonPeer;

class NonSignal
{
public:
	enum class Direction
	{
		IN,
		OUT
	};

	enum class Type
	{
		DUMMY,
		HARDWARE,
		EXTERNAL
	};

	inline NonSignal(std::string_view path, float min = 0.f, float max = 1.f,
	                 float default_value = 0.f,
	                 Direction direction = Direction::OUT,
	                 Type type = Type::DUMMY)
	    : path(path)
	    , min(min)
	    , max(max)
	    , default_value(default_value)
	    , direction(direction)
	    , type(type)
	{}
	inline NonSignal(const NonSignal&) = default;
	inline virtual ~NonSignal() = default;

	inline auto direction_str() const
	{
		return direction_str(direction);
	}
	static std::string direction_str(Direction direction)
	{
		switch (direction) {
		case Direction::IN:
			return "in";
		case Direction::OUT:
			return "out";
		}

		return "INVALID";
	}

	inline bool operator==(const NonSignal& other) const noexcept
	{
		return path == other.path;
	}

	const std::string path;
	const float min;
	const float max;
	const float default_value;
	const Direction direction{Direction::OUT};
	const Type type;
};

class NonRemoteSignal : public NonSignal
{
public:
	inline NonRemoteSignal(NonPeer& owner, std::string_view path, float min,
	                       float max, float default_value,
	                       Direction direction = Direction::IN)
	    : NonSignal(path, min, max, default_value, direction, Type::EXTERNAL)
	    , owner(owner)
	{}
	inline NonRemoteSignal(const NonRemoteSignal&) = default;
	inline ~NonRemoteSignal() = default;

	inline bool operator==(const NonRemoteSignal& other) const noexcept
	{
		return path == other.path && &owner == &other.owner;
	}

	void send_value(float value);

	NonPeer& owner;
};

using NonSignalList = std::vector<NonSignal>;
using NonRemoteSignalList = std::vector<NonRemoteSignal>;

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
