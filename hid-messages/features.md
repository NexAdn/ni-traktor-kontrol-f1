# Feature protocol
## Available features

  * Report ID 0xf0
    * 4x8 Usage=4
    * 4x8 Usage=5
  * Report ID 0xd0
    * 63x8 Usage=209
  * Report ID 0xf1
    * 48x8 Usage=129

## Report 0xf0 - assumptions

  * Usages and amount match those of knob/fader inputs
    * might configure something with knobs/faders, but no clue, what it might be
  * Amount is the same as special buttons on the device
    * might configure something with special buttons
    * different usages are quite strange when dealing with the special buttons
  * Amount is the same as configurable LEDs for stop buttons on the device
    * different usages might mean differen LEDs (left/right)
    * no clue, which behavior might be configured

## Report 0xd0 - assumptions

  * I have no idea what this might mean

## Report 0xf1 - assumptions

  * 48x8 might represent 16xBRG color values for the button matrix
    * unclear, what the (possible) BRG value could mean
  * Usage matches LED output usage
