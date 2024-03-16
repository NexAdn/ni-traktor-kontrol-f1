#include <iostream>
#include <memory>
#include <sstream>
#include <thread>

#include "io/HidDevice.hpp"
#include "tkf1/F1Device.hpp"

namespace
{
constexpr std::size_t MAX_HIDRAW_DEVICE_IDX{100};
constexpr const char* HIDRAW_PREFIX{"/dev/hidraw"};

namespace errcode
{
const int SUCCESS{0};
const int NO_DEVICE_FOUND{2};
} // namespace errcode
} // namespace

using namespace std::chrono_literals;

int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv)
{
	std::unique_ptr<F1Device> dev;

	for (std::size_t i = 0; not dev && i < MAX_HIDRAW_DEVICE_IDX; i++) {
		std::ostringstream dev_path_oss;
		dev_path_oss << HIDRAW_PREFIX << i;
		const std::string dev_path{dev_path_oss.str()};

		std::cout << "Trying " << dev_path << '\n';
		try {
			HidDevice hid_dev{dev_path.c_str()};
			const auto devinfo = hid_dev.devinfo();
			if (F1Device::is_f1_device(devinfo)) {
				dev = std::make_unique<F1Device>(
					std::move(hid_dev)
				);
			}
		} catch (HidDevicePermissionDenied&) {
			std::cout << "... Permission denied\n";
		} catch (HidDeviceError& e) {
			std::cout << "... error: " << e.what() << '\n';
			break;
		}
	}

	if (not dev) {
		std::clog << "Failed to find a suitable device!\n";
		return errcode::NO_DEVICE_FOUND;
	}

	std::cout << "... connected.\n";

	F1Device::OutputState& output = dev->output_state();
	F1Device::Brightness brightness_offset{0};
	F1Device::InputState input_state;

	std::jthread t1{[&]() {
		while (true) {
			brightness_offset++;
			for (std::size_t i = 0;
			     i < F1Device::MATRIX_BUTTONS_NUM;
			     ++i) {
				const F1Device::Brightness brightness =
					((i + 1) * 8 + brightness_offset) %
					F1Device::MAX_BRIGHTNESS;
				const F1Device::ButtonColor color{
					brightness,
					(brightness +
					 (F1Device::MAX_BRIGHTNESS / 3)) %
						F1Device::MAX_BRIGHTNESS,
					(brightness +
					 (F1Device::MAX_BRIGHTNESS / 3) * 2) %
						F1Device::MAX_BRIGHTNESS
				};

				output.matrix_btns.at(i) = color;
				if (i < F1Device::STOP_BUTTONS_NUM) {
					output.stop_btns.at(i) = brightness;
				}
			}

			for (std::uint8_t i = 0;
			     i < F1Device::SPECIAL_BUTTONS_NUM;
			     ++i) {
				output.special_btns.at(i) =
					input_state.special_btns[i]
						? F1Device::FULL_BRIGHTNESS
						: (brightness_offset %
						   F1Device::MAX_BRIGHTNESS);
			}

			const auto& [left, right] = F1Device::num_to_segments(
				brightness_offset % F1Device::MAX_SEGMENT_NUM
			);
			output.segment_left_char = left;
			output.segment_right_char = right;
			output.segment_left_brightness =
				F1Device::FULL_BRIGHTNESS;
			output.segment_right_brightness =
				F1Device::FULL_BRIGHTNESS;
			output.segment_left_dot =
				((brightness_offset / 10) % 2 == 0)
					? F1Device::FULL_BRIGHTNESS
					: F1Device::DARK;
			output.segment_right_dot =
				((brightness_offset / 10) % 2 == 0)
					? F1Device::DARK
					: (brightness_offset %
					   F1Device::MAX_BRIGHTNESS);

			dev->write();
			std::this_thread::sleep_for(20ms);
		}
	}};

	while (true) {
		input_state = dev->read();

		std::cout << "KNOBS:";
		for (std::size_t i = 0; i < F1Device::KNOBS_NUM; ++i) {
			const float percentage =
				static_cast<float>(input_state.knobs.at(i)) /
				static_cast<float>(F1Device::KNOBS_MAX) * 100.F;
			std::cout << ' ' << percentage << '%';
		}

		std::cout << "\nFADERS:";
		for (std::size_t i = 0; i < F1Device::FADERS_NUM; ++i) {
			const float percentage =
				static_cast<float>(input_state.faders.at(i)) /
				static_cast<float>(F1Device::FADERS_MAX) *
				100.F;
			std::cout << ' ' << percentage << '%';
		}

		std::cout << "\nWHEEL: "
			  << static_cast<unsigned>(input_state.wheel) << '\n';
		std::cout << "SP:";
		for (std::size_t i = 0; i < F1Device::SPECIAL_BUTTONS_NUM;
		     ++i) {
			if (input_state.special_btns[i]) {
				std::cout << ' '
					  << F1Device::special_btn_name(i);
			}
		}

		std::cout << '\n';
		for (std::uint8_t i = 0; i < 4; ++i) {
			std::cout << "+---+---+---+---+\n";
			for (std::uint8_t j = 0; j < 4; ++j) {
				const auto idx = F1Device::matrix_pos(j, i);
				std::cout << "| "
					  << (input_state.matrix_btns[idx]
						      ? "x "
						      : "  ");
			}
			std::cout << "|\n";
		}
		std::cout << "+---+---+---+---+\n";
		std::cout << "+---+---+---+---+\n";
		for (std::uint8_t i = 0; i < 4; ++i) {
			std::cout
				<< (input_state.stop_btns[i] ? "|STP" : "|   ");
		}
		std::cout << "|\n+---+---+---+---+\n";
	}

	return errcode::SUCCESS;
}

