# Traktor Kontrol F1 HID protocol

This page attempts to summarize the results of the reverse engineering to a proper specification of all the known fields of the HID protocol.

## HID Usage

  * Usage Page: 0xff01 (Vendor specific)
  * Usages:
    * Inputs (Report ID = 0x01):
      * 2 – Button input
      * 3 – Endless encoder
      * 4 – Knob
      * 5 – Fader
    * Outputs (Report ID = 0x80):
      * 129 – LED Brightness
    * Features (Report ID = 0xf0):
      * 4
      * 5
    * Features (Report ID = 0xf1):
      * 129
    * Features (Report ID = 0xd0):
      * 209

### Inputs
#### 2 – Button input
  * Width: 1 bit
  * States: 0 (button not pressed) and 1 (button pressed)
  * Button input is always provided as a bitfield, containing the state of multiple buttons at once (each bit representing a state of a single button)
  * Applies to:
    * Special buttons (Sync, Quant, Shift, etc.)
    * Matrix buttons
    * Stop buttons
    * Selector wheel button

#### 3 – Endless encoder
  * Width: 8 bit (0x00 .. 0xff)
  * State:
    * Turning the encoder to the left by 1 position decreases the value by 1
    * Turning the encoder to the right by 1 position increases the value by 1
    * The exact value is irrelevant, only the change of value indicates a movement of the encoder
  * Applies to:
    * Selector wheel

#### 4 – Knob
  * Width: 12 bit (2 bytes, stored LSB first)
  * State:
    * Turning the knob to the leftmost position gets the minimum value
    * Turning the knob to the rightmost position gets the maximum value
    * The exact value give the position the knob is in
    * Calibration might be necessary to determine which values correspond to the leftmost/rightmost position
  * Applies to:
    * Knobs in the top row (Filter 1 - 4)

#### 5 – Fader
  * Width: 12 bit (2 bytes, stored LSB first)
  * State:
    * Pulling the fader to the bottom position gets the minimum value
    * Pushing the fader to the top position gets the maximum value
    * The exact value give the position the fader is in
    * Calibration might be necessary to determine which values correspond to the bottom/top position
  * Applies to:
    * Faders 1 - 4

### Outputs
#### 129 – LED Brightness
  * Width: 7 bit (stored as 1 whole byte with the most significant bit as don't care)
  * State:
    * Set the brightness of the corresponding LED from 0x00 (off) up to 0x7f (full brightness)
  * Applies to:
    * All elements on the device which can emit light (including every segment of the 7-segment display individually!)

### Features

Features have not been reverse engineered and it is not planned to reverse engineer them.
Apparently, Features are not necessary to implement a driver which can read all device inputs and write all device outputs freely.

## Reading values

Data is sent from the device as report.
The first byte tells the report ID which can be used to determine the format of the provided data.
As Traktor Kontrol F1 has only one Input collection and all inputs are sent with every report,
the ID is always 0x01.

### Input format

Ordered by the bytes as they are e.g. written into memory upon performing a `read()` call on the `hidraw` device:
  * 0 – Report ID (= 0x01)
  * 1 – Button matrix (top half)
    * Bit 7 – Button (1, 1)
    * Bit 6 — Button (2, 1)
    * Bit 5 – Button (3, 1)
    * Bit 4 – Button (4, 1)
    * Bit 3 – Button (1, 2)
    * Bit 2 – Button (2, 2)
    * Bit 1 – Button (3, 2)
    * Bit 0 – Button (4, 2)
  * 2 – Button matrix (bottom half)
    * Bit 7 – Button (1, 3)
    * Bit 6 — Button (2, 3)
    * Bit 5 – Button (3, 3)
    * Bit 4 – Button (4, 3)
    * Bit 3 – Button (1, 4)
    * Bit 2 – Button (2, 4)
    * Bit 1 – Button (3, 4)
    * Bit 0 – Button (4, 4)
  * 3 – Special buttons (bottom half)
    * Bit 7 – Shift
    * Bit 6 – Reverse
    * Bit 5 – Type
    * Bit 4 – Size
    * Bit 3 – Browse
    * Bit 2 – Selector wheel button
    * Bit 1 – don't care
    * Bit 0 – don't care
  * 4 – Stop buttons, Special buttons (top half)
    * Bit 7 – Stop 1
    * Bit 6 – Stop 2
    * Bit 5 – Stop 3
    * Bit 4 – Stop 4
    * Bit 3 – Sync
    * Bit 2 – Quant
    * Bit 1 – Capture
    * Bit 0 – don't care
  * 5 – Selector wheel
  * 6 – Knob 1 LSB
  * 7 – Knob 1 MSB
  * 8 – Knob 2 LSB
  * 9 – Knob 2 MSB
  * 10 – Knob 3 LSB
  * 11 – Knob 3 MSB
  * 12 – Knob 4 LSB
  * 13 – Knob 4 MSB
  * 14 – Fader 1 LSB
  * 15 – Fader 1 MSB
  * 16 – Fader 2 LSB
  * 17 – Fader 2 MSB
  * 18 – Fader 3 LSB
  * 19 – Fader 3 MSB
  * 20 – Fader 4 LSB
  * 21 – Fader 4 MSB

## Writing values (i.e. setting LEDs)

Data is sent with report ID 0x80.
Each byte is one LED's brightness value in the range 0x00 .. 0x7f.
All LEDs states are transmitted in one report.

### Output format

Ordered by the bytes as they are e.g. written into memory before performing a `write()` call on the `hidraw` device:

  * 0 – Report ID (= 0x80)
  * 1 to 8 – 7 segment display, right number
    * 1 – dot
    * 2 – middle
    * 3 – lower right
    * 4 – upper right
    * 5 – top
    * 6 – upper left
    * 7 – lower left
    * 8 – bottom
  * 9 to 16 – 7 segment display, left number
    * 9 – dot
    * 10 – middle
    * 11 – lower right
    * 12 – upper right
    * 13 – top
    * 14 – upper left
    * 15 – lower left
    * 16 – bottom
  * 17 to 25 – special buttons
    * 17 – Browse
    * 18 – Size
    * 19 – Type
    * 20 – Reverse
    * 21 – Shift
    * 22 – Shift
    * 23 – Capture
    * 24 – Quant
    * 25 – Sync
  * 26 to 73 – RGB LED button matrix
    * 26 – (1, 1) blue
    * 27 – (1, 1) red
    * 28 – (1, 1) green
    * 29 – (2, 1) blue
    * 30 – (2, 1) red
    * 31 – (2, 1) green
    * 32 – (3, 1) blue
    * 33 – (3, 1) red
    * 34 – (3, 1) green
    * 35 – (4, 1) blue
    * 36 – (4, 1) red
    * 37 – (4, 1) green
    * 38 – (1, 2) blue
    * 39 – (1, 2) red
    * 40 – (1, 2) green
    * 41 – (2, 2) blue
    * 42 – (2, 2) red
    * 43 – (2, 2) green
    * 44 – (3, 2) blue
    * 45 – (3, 2) red
    * 46 – (3, 2) green
    * 47 – (4, 2) blue
    * 48 – (4, 2) red
    * 49 – (4, 2) green
    * 50 – (1, 3) blue
    * 51 – (1, 3) red
    * 52 – (1, 3) green
    * 53 – (2, 3) blue
    * 54 – (2, 3) red
    * 55 – (2, 3) green
    * 56 – (3, 3) blue
    * 57 – (3, 3) red
    * 58 – (3, 3) green
    * 59 – (4, 3) blue
    * 60 – (4, 3) red
    * 61 – (4, 3) green
    * 62 – (1, 4) blue
    * 63 – (1, 4) red
    * 64 – (1, 4) green
    * 65 – (2, 4) blue
    * 66 – (2, 4) red
    * 67 – (2, 4) green
    * 68 – (3, 4) blue
    * 69 – (3, 4) red
    * 70 – (3, 4) green
    * 71 – (4, 4) blue
    * 72 – (4, 4) red
    * 73 – (4, 4) green
  * 74 to 81 – Stop buttons (each button has 2 separate LEDs)
    * 74 Stop 4 right
    * 75 Stop 4 left
    * 76 Stop 3 right
    * 77 Stop 3 left
    * 78 Stop 2 right
    * 79 Stop 2 left
    * 80 Stop 1 right
    * 81 Stop 1 left
