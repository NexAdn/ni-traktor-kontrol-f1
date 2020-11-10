#include <cstdlib>
#include <iostream>
#include <string_view>

#include "NonSessionHandler.hpp"

namespace {
std::string get_env(std::string_view key)
{
	const char* value = std::getenv(key.data());
	if (value) {
		return value;
	} else {
		return "";
	}
}
}

int main(int argc, char** argv)
{
	auto nsm_url = get_env("NSM_URL");
	
	if (nsm_url == "") {
		std::cerr << "No NSM_URL provided. Aborting\n";
		return 1;
	}

	std::cout << "Starting session\n";

	NonSessionHandler nsh(nsm_url, argv[0]);
	nsh.start_session();

	std::this_thread::sleep_for(std::chrono::seconds(10));

	return 0;
}
