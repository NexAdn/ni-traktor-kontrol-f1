# Reverse Engineering Native Instruments Traktor Kontrol F1 HID protocol

This is my first attempt at reverse engineering the communication with a propietary device.
In this case, I'm trying to learn how one needs to talk to a Traktor Kontrol F1.

## The device
The Traktor Kontrol F1 is a DJ controller consisting of some faders, knobs and
a 4x4 launch pad as well as some utility buttons and one endless encoder which
is e.g. used for browsing the files in the Traktor Pro 2 DJ software.

The Traktor Kontrol F1 transports input events via USB-HID.  Furthermore, the
F1 can receive messages to control the RGB color value of the buttons as well
as the value shown on the 7 segment display next to the endless encoder.

## Status of this project

The inputs and outputs of the device have been reverse engineered.
The features are still missing and are probably not gonna be added.

A [Non](https://non.tuxfamily.org)-compatible OSC driver implementation is being developed (see [driver-osc](driver-osc/README.md)).

## Main tools used for reverse engineering
  * vim/Okular (Notes taking)
  * usbutils
  * Testing OS: Gentoo Linux

## Useful links
  * https://en.wikipedia.org/wiki/USB_human_interface_device_class
  * https://www.linuxjournal.com/article/8145
  * https://www.usb.org/sites/default/files/hid1_11.pdf
  * https://www.usb.org/sites/default/files/hut1_21.pdf
  * https://www.kernel.org/doc/html/latest/hid/hidraw.html

## License

Copyright (C) 2020 Adrian Schollmeyer.

This repository is licensed under the terms of the GNU General Public License Version 2 or later.
A copy of the GPL can be found in [LICENSE.md](LICENSE.md).
