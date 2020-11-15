#include <iostream>
#include <thread>

#include <stdio.h>
#include <string.h>

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <linux/hiddev.h>

#include <unistd.h>

#include "../F1HidDev.hpp"

#define TKF1_VENDOR 0x17cc
#define TKF1_PRODUCT 0x1120
#define TKF1_DEVFILE "/dev/hidraw2"

#define BUF_SIZE 22

namespace
{
template <typename T>
constexpr T vmap(T val, T from_min, T from_max, T to_min, T to_max)
{
	return (val - from_min) * (to_max - to_min) / (from_max - from_min)
	       + to_min;
}
} // namespace

int main(int argc, char** argv)
{
	int fd = open(TKF1_DEVFILE, O_RDWR);

	if (fd < 0) {
		printf("open(): Failed!\n");
		return 1;
	}

	F1HidDev dev(fd);

	/* std::array<uint8_t, 81> output;
	output.at(0) = 0x80;

	for (size_t i = 1; i < 81; i++) {
	    output.at(i) = 0;
	}

	for (size_t i = 0x2e; i < 81; i++) {
	    for (size_t j = 0; j < 0x81; j++) {
	        std::cout << std::hex << i - 1 << "\t->\t" << j;
	        std::string unused;
	        std::getline(std::cin, unused);
	        output.at(i) = j;
	        ssize_t res = write(fd, output.data(), output.size());
	        if (res != 81) {
	            std::cout << "[ERROR] write() failed. res is " << res
	                      << std::endl;
	        }
	    }
	} */

	// F1HidDev::output output_data;

	/*while (true) {
	    uint8_t r{0}, g{0}, b{0};
	    for (size_t i = 0; i < 16; i++) {
	        r += 8;
	        g += 16;
	        b += 32;

	        output_data.matrix_btns.at(i) = F1HidDev::rgb2color(r, g, b);
	    }

	    r += 1;
	    g += 2;
	    b += 3;

	    dev.write(output_data);

	    std::this_thread::sleep_for(std::chrono::milliseconds(50));
	}*/

	/* while (true) {
	    std::cout << "Byte: ";
	    int byte;
	    std::cin >> byte;
	    std::cout << "Val:  ";
	    uint16_t val;
	    std::cin >> val;

	    try {
	        output.at(byte + 1) = static_cast<uint8_t>(val);
	    }
	    catch (std::out_of_range&) {
	        std::cout << "Invalid byte.\n";
	        continue;
	    }

	    ssize_t res = write(fd, output.data(), output.size());
	    std::cout << "Res: " << res << std::endl;
	} */

	// std::array<uint8_t, 81> output_data;
	F1HidDev::output output_data;
	/*uint8_t rainbow{0};
	for (size_t i = 0; i < 16; i++) {
	    output_data.matrix_btns.at(i) = F1HidDev::rgb2color(rainbow, rainbow,
	rainbow); rainbow += 1;
	}

	dev.write(output_data);
	return 0;*/

	// write(fd, output_data.data(), output_data.size());

	uint8_t button_r{0};
	uint8_t button_g{0};
	uint8_t button_b{0};

	while (true) {
		auto& data = dev.read();

		std::cout << "[REPORT ID " << static_cast<int>(data.report_id) << "]\n";

		std::cout << "Button Matrix\n";

		for (size_t j = 0; j < 4; j++) {
			std::cout << "   ";
			for (size_t i = 0; i < 4; i++) {
				std::cout << ' ' << (data.matrix_btns[i + 4 * j] ? 'X' : '-');
			}
			std::cout << std::endl;
		}

		for (size_t i = 0; i < 16; i++) {
			if (data.matrix_btns[i]) {
				output_data.matrix_btns.at(i) =
				  F1HidDev::rgb2color(i * 8, 0x7f - (i * 8), 0);
			} else {
				output_data.matrix_btns.at(i) =
				  F1HidDev::rgb2color(button_r, button_g, button_b);
			}
		}

		std::cout << "\nStop buttons\n";
		std::cout << "   ";
		for (size_t i = 0; i < 4; i++) {
			std::cout << ' ' << (data.stop_btns[i] ? 'S' : '.');
		}

		dev.write(output_data);

		std::cout << "\nSpecial buttons\n";
		constexpr std::array<char, 9> special_btn_chars{'W', 'B', 'S', 'T', 'R',
		                                                '+', 'C', 'Q', 'S'};
		for (size_t i = 0; i < 9; i++) {
			std::cout << (data.special_btns[i] ? special_btn_chars.at(i) : '-')
			          << ' ';
		}
		std::cout << std::endl;

		std::cout << "\nWheel: " << static_cast<unsigned>(data.wheel)
		          << std::endl;

		std::cout << "Knobs and faders\n";
		for (size_t i = 0; i < 4; i++) {
			std::cout << std::hex << "K" << i << " " << data.knobs.at(i)
			          << '\t';
		}
		for (size_t i = 0; i < 4; i++) {
			std::cout << std::hex << "F" << i << " " << data.faders.at(i)
			          << '\t';
		}
		float brightness = vmap<float>(data.faders.at(3), 0, 0xfff, 0.f, 1.f);
		button_r =
		  vmap<uint16_t>(data.faders.at(0) * brightness, 0, 0xfff, 0, 0x7f);
		button_g =
		  vmap<uint16_t>(data.faders.at(1) * brightness, 0, 0xfff, 0, 0x7f);
		button_b =
		  vmap<uint16_t>(data.faders.at(2) * brightness, 0, 0xfff, 0, 0x7f);
		std::cout << std::endl;
	}

	return 0;
}
