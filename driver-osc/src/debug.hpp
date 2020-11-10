#pragma once

#ifdef DEBUG
#include <iostream>
#include <mutex>
#include <string_view>
#endif

namespace
{
#ifdef DEBUG
void debug(std::string_view str)
{
	static std::mutex cout_mtx;
	std::lock_guard<std::mutex> lck(cout_mtx);
	std::cout << "[DBG] " << str;
}
#else
void debug(std::string_view str)
{}
#endif
} // namespace
