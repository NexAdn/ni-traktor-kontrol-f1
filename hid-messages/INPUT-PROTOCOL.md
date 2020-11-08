# Input protocol assumptions
## HID layout

The HID messages contain bytes 0-21 (left to right).
Bits are numbered right to left.
Byte/Bit positions in the following are given as `BYTE/BIT`.

The bytes have the following general functions:

  * 0:
  * 1: Button matrix (top half)
  * 2: Button matrix (bottom half)
  * 3: Special buttons (bottom row, selector wheel button)
  * 4: Special buttons (top row without selector wheel button), Stop button
  * 5: Selector wheel endl. encoder
  * 6: Knob 1 LSB
  * 7: Knob 1 MSB
  * 8: Knob 2 LSB
  * 9: Knob 2 MSB
  * 10: Knob 3 LSB
  * 11: Knob 3 MSB
  * 12: Knob 4 LSB
  * 13: Knob 4 MSB
  * 14: Fader 1 LSB
  * 15: Fader 1 MSB
  * 16: Fader 2 LSB
  * 17: Fader 2 MSB
  * 18: Fader 3 LSB
  * 19: Fader 3 MSB
  * 20: Fader 4 LSB
  * 21: Fader 4 MSB

## Buttons
### Special buttons

  * Sync: 4/3
  * Quant: 4/2
  * Capture: 4/1
  * Shift: 3/7
  * Reverse: 3/6
  * Type: 3/5
  * Size: 3/4
  * Browse: 3/3
  * Selector wheel button: 3/2

### Button matrix

Buttons are (x, y) (left-right, top-bottom).

  * (1, 1): 1/7
  * (2, 1): 1/6
  * (3, 1): 1/5
  * (4, 1): 1/4
  * (1, 2): 1/3
  * (2, 2): 1/2
  * (3, 2): 1/1
  * (4, 2): 1/0
  * (1, 3): 2/7
  * (2, 3): 2/6
  * (3, 3): 2/5
  * (4, 3): 2/4
  * (1, 4): 2/3
  * (2, 4): 2/2
  * (3, 4): 2/1
  * (4, 4): 2/0

As one can see, buttons start from top left with 1/7.
Each row from left to right, the next bit to the right is being used.

The whole range is from 1/7 to 2/0, ranging exactly 2 bytes (16 bits).

### Stop buttons

  * Stop 1: 4/7
  * Stop 2: 4/6
  * Stop 3: 4/5
  * Stop 4: 4/4

Stop buttons take the upper half byte of byte 4, thus preceding the bits for
the top half special buttons.

## Knobs

Knobs provide 12 bits of precision and are stored LSB first throughout bytes 6
to 13 with each knob being represented by 2 bytes (the upper 4 bits of MSB are
all zero).
Turning the knob to the leftmost position gives the minimum value (near 0x000)
while turning the knob to the rightmost position gives the maximum value (near
0xFFF).
As the values are not exactly 0x000 or 0xFFF, calibration might be necessary to
determine the leftmost, rightmost and therefore middle position of the knob.

Example calibration of the testing device:

  * Knob 1: 0x001 to 0xFFB
  * Knob 2: 0x002 to 0xFFB
  * Knob 3: 0x000 to 0xFFA
  * Knob 4: 0x001 to 0xFFC

The exact calibration has fluctuations of about 2, but is apparently always
nearly from 0x000 to 0xFFF (at least for the testing device).

## Faders

Faders provide 12 bits of precision anologous to knobs and are stored LSB first
throughout bytes 14 to 21.
Faders range from 0x000 (bottom position) to 0xFFF (top position, value
assumed) and need to be calibrated like the knobs.

Example calibration of the testing devices:

  * Fader 1: 0x000 to 0xFF6
  * Fader 2: 0x001 to 0xFF8
  * Fader 3: 0x000 to 0xFF7
  * Fader 4: 0x000 to 0xFF7

Like the knobs, the exact calibration has fluctuations of about 2, but is
apparently always somewhere near 0x000 to 0xFFF (at least for the testing
device).

## Wheel

The wheel takes byte 5 and stores an 8 bit value of the virtual “position” the encoder is in.
As the steps of the wheel are much more coarse, it takes several rounds of turning the wheel in a certain direction to reach either 0x00 or 0xFF.
In either case, an integer overflow/underflow happens so the wheel can be spinned endlessly in both directions while the turning direction can be determined easily.

Turning the wheel by one position to the left decreases the value by 1.
Turning the wheel by one position to the right increases the value by 1.
Therfore, one can determine by how many positions the wheel has been turned to
the right by getting the difference of the current to the previous value.
