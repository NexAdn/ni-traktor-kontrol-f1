# Outputs

## Available device outputs
  * Button matrix
    * 16 x RGB
  * Stop buttons
    * 4 x On/Off
  * Special buttons
    * 8 x On/Off
  * 7 segment display
    * 2 x 7seg

## Possible outputs

Possible data:
  * Button matrix (48 byte)
    * 16 x 3 x 8 bit color
  * Stop buttons (4 bit)
    * 4 x 1 bit On/Off
  * Special buttons (8 bit)
    * 8 x 1 bit On/Off
  * 7seg display (8 bit)
    * 1 x 8 bit value

Possible distribution:
  * Button matrix
    * 16 x 3 x 8 bit
  * Stop buttons
    * 4 x 8 bit
    * 1 x 8 bit
  * Special buttons
    * 8 x 8 bit
    * 1 x 8 bit
  * 7seg display
    * 1 x 8 bit
    * 2 x 8 bit (BCD)

The largest possible distribution in this case yields 62 bytes of data.

## HID interface
### From devinfo.md
  * 80x8 Usage=129

### Measurements

Apparently, all outputs are 7 bit precision brightness values.
Everything on the F1 can be controlled with 128 different brightness values for each available color/LED/segment.
RGB values are in the order BRG.

### Real Outputs

  * 0...f: 7 segment display (individual segment brightness)
    * 0...7: right segment (order: dot, middle, lower right, upper right, top, upper left, lower left, bottom)
    * 8...f: left segment (same order as right segment)
  * 10...17: special buttons (only brightness, single color), order:
    * 10: browse
    * 11: size
    * 12: type
    * 13: reverse
    * 14: shift
    * 15: capture
    * 16: quant
    * 17: sync
  * 18...47: button matrix
    * order: (1, 1), (2, 1), ..., (1, 4), ..., (4, 4)
    * color: BRG
  * 48...4f: stop buttons
    * order: stop 4 right, stop 4 left, stop 3 right, stop 3 left, ..., stop 1 left
