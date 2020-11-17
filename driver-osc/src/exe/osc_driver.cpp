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

constexpr auto rgb2c(uint8_t r, uint8_t g, uint8_t b)
{
	return F1HidDev::rgb2color(r, g, b);
}

namespace colors
{
	constexpr float lightness_on{1.f};
	constexpr float lightness_off{.2f};

	constexpr std::array<std::array<uint8_t, 3>, 16> mtx{
	  rgb2c(0x00, 0x00, 0x7f), rgb2c(0x7f, 0x00, 0x10), rgb2c(0x00, 0x7f, 0x00),
	  rgb2c(0x7f, 0x7f, 0x00), rgb2c(0x00, 0x00, 0x7f), rgb2c(0x7f, 0x00, 0x10),
	  rgb2c(0x00, 0x7f, 0x00), rgb2c(0x7f, 0x7f, 0x00), rgb2c(0x00, 0x00, 0x7f),
	  rgb2c(0x7f, 0x00, 0x10), rgb2c(0x00, 0x7f, 0x00), rgb2c(0x7f, 0x7f, 0x00),
	  rgb2c(0x00, 0x00, 0x7f), rgb2c(0x7f, 0x00, 0x10), rgb2c(0x00, 0x7f, 0x00),
	  rgb2c(0x7f, 0x7f, 0x00),
	};
} // namespace colors

int exec_driver(int fd, const char* nsm_url, std::string_view exe_name);
void set_colors(F1HidDev dev, const F1InputChange& input = {});
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

	set_colors(dev);

	std::bitset<4> stop_buttons;

	F1InpuState last{};

	while (true) {
		F1InpuState current = dev.read();
		debug("Received input event\n");

		F1InputChange input_diff = current - last;

		decltype(input_diff.pressed_buttons.stop) new_pressed_stop;
		decltype(input_diff.released_buttons.stop) new_released_stop;

		for (auto& btn : input_diff.pressed_buttons.stop) {
			stop_buttons.flip(btn);
			if (stop_buttons[btn]) {
				new_pressed_stop.push_back(btn);
			} else {
				new_released_stop.push_back(btn);
			}
		}

		input_diff.pressed_buttons.stop = new_pressed_stop;
		input_diff.released_buttons.stop = new_released_stop;

		nsh.broadcast_input_event(input_diff);
		set_colors(dev, input_diff);

		last = current;
	}

	return 0;
}

void set_colors(F1HidDev dev, const F1InputChange& input)
{
	static F1HidDev::output output;
	static bool first_time{true};
	if (first_time) {
		debug("[KontrolF1] Setting initial button colors\n");
		first_time = false;
		for (size_t i = 0; i < 16; i++) {
			auto color = colors::mtx.at(i);
			for (auto& c : color) {
				c *= colors::lightness_off;
			}
			output.matrix_btns.at(i) = color;
		}

		for (size_t i = 0; i < 4; i++) {
			output.stop_btns.at(i) = 0x7f * colors::lightness_off;
		}

		dev.write(output);
	}

	for (auto& btn : input.pressed_buttons.matrix) {
		debug(std::string{"[KontrolF1] Turning on lights for matrix button "}
		      + std::to_string(btn) + '\n');
		auto color = colors::mtx[btn];
		for (size_t i = 0; i < color.size(); ++i) {
			color[i] *= colors::lightness_on;
		}

		output.matrix_btns[btn] = color;
	}

	for (auto& btn : input.released_buttons.matrix) {
		debug(std::string{"[KontrolF1] Turning off lights for matrix button "}
		      + std::to_string(btn) + '\n');
		auto color = colors::mtx[btn];
		for (size_t i = 0; i < color.size(); ++i) {
			color[i] *= colors::lightness_off;
		}

		output.matrix_btns[btn] = color;
	}

	for (auto& btn : input.pressed_buttons.stop) {
		debug(std::string{"[KontrolF1] Turning on lights for stop button "}
		      + std::to_string(btn) + '\n');
		output.stop_btns[btn] = colors::lightness_on * 0x7f;
	}

	for (auto& btn : input.released_buttons.stop) {
		debug(std::string{"[KontrolF1] Turning off lights for stop button "}
		      + std::to_string(btn) + '\n');
		output.stop_btns[btn] = colors::lightness_off * 0x7f;
	}

	debug("[KontrolF1] Writing output\n");
	dev.write(output);
}
} // namespace
