#pragma once

#include <cassert>
#include <string>
#include <vector>

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
