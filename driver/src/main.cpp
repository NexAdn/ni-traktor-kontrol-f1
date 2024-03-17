#include <iomanip>
#include <iostream>
#include <memory>
#include <sstream>

#include <jack/types.h>
#include <thread>

#include "io/HidDevice.hpp"
#include "io/JackWrapper.hpp"
#include "io/MidiEvent.hpp"
#include "tkf1/F1Device.hpp"
#include "tkf1/IOMapper.hpp"

using namespace std::chrono_literals;
using Color = F1Device::ButtonColor;

namespace
{
constexpr std::size_t MAX_HIDRAW_DEVICE_IDX{100};
constexpr const char* HIDRAW_PREFIX{"/dev/hidraw"};
constexpr std::size_t BATCH_SIZE{1024};
constexpr std::chrono::milliseconds MIDI_NO_EVENTS_WAIT{50ms};

std::unique_ptr<F1Device> discover_device();

namespace errcode
{
const int SUCCESS{0};
const int NO_DEVICE_FOUND{2};
} // namespace errcode

namespace colors
{
constexpr Color RED = F1Device::rgb2color(
	F1Device::FULL_BRIGHTNESS, F1Device::DARK, F1Device::DARK
);
constexpr Color GREEN = F1Device::rgb2color(
	F1Device::DARK, F1Device::FULL_BRIGHTNESS, F1Device::DARK
);
constexpr Color BLUE = F1Device::rgb2color(
	F1Device::DARK, F1Device::DARK, F1Device::FULL_BRIGHTNESS
);
constexpr Color YELLOW = F1Device::rgb2color(
	F1Device::FULL_BRIGHTNESS, F1Device::FULL_BRIGHTNESS, F1Device::DARK
);
} // namespace colors
} // namespace

int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv)
{
	std::shared_ptr<F1Device> dev = discover_device();
	std::shared_ptr<JackWrapper> jack;
	IOMapper io_mapper;

	io_mapper.matrix_colors = {
		// clang-format off
		colors::RED, colors::GREEN, colors::GREEN, colors::RED,
		colors::GREEN, colors::YELLOW, colors::YELLOW, colors::GREEN,
		colors::GREEN, colors::BLUE, colors::YELLOW, colors::GREEN,
		colors::RED, colors::GREEN, colors::GREEN, colors::RED,
		// clang-format on
	};
	io_mapper.button_toggle.matrix.set();

	if (not dev) {
		std::clog << "Failed to find a suitable device!\n";
		return errcode::NO_DEVICE_FOUND;
	}

	std::cout << "Connected to Traktor Kontrol F1\n";

	jack = std::make_unique<JackWrapper>();
	jack->activate();

	std::jthread input_thread{[&]() {
		while (true) {
			dev->read_events([&](const F1Device::InputEvent event,
					     [[maybe_unused]] F1Device&) {
				auto midi = io_mapper.process_HID_input(
					event, dev->output_state()
				);
				if (midi) {
					std::cout << "Writebuf: "
						  << jack->write_bufsize()
						  << '\n';
					const auto& bytes = midi->to_bytes();
					const auto fmtflags = std::cout.flags();
					std::cout << std::hex
						  << std::setfill('0')
						  << std::setw(2)
						  << static_cast<unsigned>(
							     bytes.at(0)
						     )
						  << ' ' << std::setw(2)
						  << static_cast<unsigned>(
							     bytes.at(1)
						     )
						  << ' ' << std::setw(2)
						  << static_cast<unsigned>(
							     bytes.at(2)
						     )
						  << '\n';
					std::cout.flags(fmtflags);
					*jack << *midi;
				}
			});
			dev->write();
		}
	}};

	while (true) {
		bool write_hid{false};
		for (std::size_t i = 0;
		     jack->read_bufsize() > 0 && i < BATCH_SIZE;
		     ++i) {
			MidiEvent event;
			*jack >> event;

			write_hid = io_mapper.process_MIDI_event(
				event, dev->output_state()
			);
		}

		if (write_hid) {
			dev->write();
		} else {
			std::this_thread::sleep_for(MIDI_NO_EVENTS_WAIT);
		}
	}

	return errcode::SUCCESS;
}

namespace
{
std::unique_ptr<F1Device> discover_device()
{
	for (std::size_t i = 0; i < MAX_HIDRAW_DEVICE_IDX; i++) {
		std::ostringstream dev_path_oss;
		dev_path_oss << HIDRAW_PREFIX << i;
		const std::string dev_path{dev_path_oss.str()};

		std::cout << "Trying " << dev_path << '\n';
		try {
			HidDevice hid_dev{dev_path.c_str()};
			const auto devinfo = hid_dev.devinfo();
			if (F1Device::is_f1_device(devinfo)) {
				return std::make_unique<F1Device>(
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
	return {nullptr};
}
} // namespace
