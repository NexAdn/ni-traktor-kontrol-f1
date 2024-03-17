# Linux JACK MIDI Driver for Native Instruments Traktor Kontrol F1 Controllers

This userspace driver adds MIDI support for NI Traktor Kontrol F1 controllers
by utilizing the JACK API.
Thus, it is compatible with both JACK and PipeWire sound servers.

## Features

* Autodiscovery for a Traktor Kontrol F1 controller using the Linux hidraw API
* Highly customizable MIDI output:
  * Configurable Note names for each button
  * Configurable CC numbers for faders, knobs and endless encoders
  * Configurable latching for buttons
* Highly customizable LED behavior
  * Customizable high and low brightness values for each and colors for each matrix LED
  * LED brightness can be coupled to either
    * Pressing the corresponding button
    * MIDI Note events (Note On for high, Note Off for low)
    * MIDI CC events (controller value sets the button brightness)

## Limitations

* The current version of the driver only detects one controller, so support for more than one controller is not available.
* Access to `/dev/hidraw*` is heavily limited by default. Depending on the system, udev rules might need to be installed for access to the correct hidraw device to be possible with a regular user (e.g. by setting the group to `plugdev` and adding the user to this group).
* There is no user interface or configuration file. The driver must be configured in the C++ source code (although it is very easy to do so!).
* The JACK client exposed by this driver is not recognized as a device (but as a regular JACK client), so some software (especially general-purpose audio APIs) might have issues connecting to the driver (as they don't see it as a device which can be connected to).

## Dependencies

* Build time
  * meson
  * ninja
  * libjack (e.g. JACK, PipeWire)
  * Catch2 (`-Dtest=true` only)
* Runtime
  * libjack (e.g. JACK, PipeWire)

## Building

```
meson setup build --buildtype=release -Dtest=false
ninja -C build
```

The compiled binaries are located in `build/src`:

* `tkf1-drv` is the actual driver exectuable.
* `tkf1-devtest` is a simple testing application which connects to the controller, displays changing patterns on the LEDs and segment displays and shows the controller input in the console output.
