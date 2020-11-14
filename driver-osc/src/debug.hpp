#pragma once

#ifdef DEBUG
#include <iostream>
#include <mutex>
#include <string_view>
#endif

#ifdef DEBUG
#define debug(str) debug_int(str, __FILE__, __LINE__)
#endif

namespace
{
#ifdef DEBUG
void debug_int(std::string_view str, std::string_view file, size_t line)
{
	static std::mutex cout_mtx;
	std::lock_guard<std::mutex> lck(cout_mtx);
#ifdef DBG_MODULE_NAME
	std::cout << "[DBG] [" DBG_MODULE_NAME ":" << file << ':' << line << "] "
	          << str;
#else
	std::cout << "[DBG] " << file << ":" << line << ": " << str;
#endif
}
#else
void debug(std::string_view str)
{}
#endif
} // namespace
