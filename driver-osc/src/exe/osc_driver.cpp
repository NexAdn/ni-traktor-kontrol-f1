#include <iostream>
#include <string>
#include <thread>

#include <fcntl.h>
#include <linux/hidraw.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <unistd.h>

#include "F1HidDev.hpp"
#include "NonSessionHandler.hpp"

#define DBG_MODULE_NAME "DRV_EXE"
#include "debug.hpp"

namespace
{
constexpr const char* HIDRAW_PREFIX{"/dev/hidraw"};
constexpr int16_t F1_VENDOR_ID{0x17cc};
constexpr int16_t F1_PRODUCT_ID{0x1120};

int exec_driver(int fd, const char* nsm_url, std::string_view exe_name);
} // namespace

int main(int argc, char** argv)
{
	const char* nsm_url = getenv("NSM_URL");
	const char* exe_name = argv[0];

	if (!nsm_url) {
		std::cerr
		  << "No NSM_URL provided. No session management possible. Aborting.\n";
		return 1;
	}

	int fd;

	for (size_t i = 0;; i++) {
		debug(std::string{"Trying "} + HIDRAW_PREFIX + std::to_string(i)
		      + '\n');
		std::string path = std::string{HIDRAW_PREFIX} + std::to_string(i);
		fd = open(path.c_str(), O_RDWR);

		debug(std::string{"  open() returned with code "} + std::to_string(fd)
		      + '\n');
		if (fd < 0) {
			std::cerr << "Failed to open " << HIDRAW_PREFIX << i << std::endl
			          << "Aborting\n";
			return 2;
		}

		debug("  Retrieving devinfo\n");
		hidraw_devinfo devinfo;
		int res = ioctl(fd, HIDIOCGRAWINFO, &devinfo);

		if (res < 0) {
			std::cerr << "ioctl() failed.\nAborting.";
			return 3;
		}

		if (devinfo.vendor == F1_VENDOR_ID
		    && devinfo.product == F1_PRODUCT_ID) {
			std::cout << "[KontrolF1] Found device!\n";

			return exec_driver(fd, nsm_url, exe_name);
		}

		debug("  Device mismatch. Trying next device.\n");
	}
}

namespace
{
int exec_driver(int fd, const char* nsm_url, std::string_view exe_name)
{
	debug("Starting driver core\n");

	F1HidDev dev(fd);
	NonSessionHandler nsh(nsm_url, exe_name, F1Default::signals);

	std::cout << "[KontrolF1] Connecting to NSM\n";
	nsh.start_session();
	while (!nsh.session_is_ready() && !nsh.session_has_failed()) {
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}

	if (nsh.session_has_failed()) {
		std::cerr << "[KontrolF1] NSM session has failed. Aborting\n";
		return 4;
	}

	F1InpuState last{};

	while (true) {
		F1InpuState current = dev.read();
		debug("Received input event\n");

		F1InputChange input_diff = current - last;

		nsh.broadcast_input_event(input_diff);

		last = current;
	}

	return 0;
}
} // namespace
