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
Currently, the input part of the device has been reverse engineered.
There are a few bits of the HID messages left which are still unclear.
A reference driver implementation is in progress.

Outputs have not yet been reverse engineered.

## Tools
  * vim/Okular (Notes taking)
  * usbutils
  * Testing OS: Gentoo Linux

## Useful links
  * https://en.wikipedia.org/wiki/USB_human_interface_device_class
  * https://www.linuxjournal.com/article/8145
