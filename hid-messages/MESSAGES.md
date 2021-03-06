# HID messages

## Initial

  * All faders in top position
  * Knobs in middle position
  * Buttons unpressed

## Special stuff

  * Without pressing anything, the device sends HID messages, probably indicating the current state of all inputs. Example:

```
001:023:000:STREAM             1604764451.349367
 01 00 00 00 00 00 F9 07 08 08 ED 07 E5 07 F6 0F
 F7 0F F7 0F F7 0F

001:023:000:STREAM             1604764479.719357
 01 00 00 00 00 00 F9 07 08 08 ED 07 E5 07 F9 0F
 F7 0F F7 0F F7 0F
```

## Buttons

Buttons are pressed left to right, top to bottom.

  * Sync:

```
001:023:000:STREAM             1604764543.909417
 01 00 00 00 08 00 F9 07 08 08 ED 07 E5 07 F9 0F
 F7 0F F7 0F F7 0F

001:023:000:STREAM             1604764544.889415
 01 00 00 00 00 00 F9 07 08 08 ED 07 E5 07 F9 0F
 F7 0F F7 0F F7 0F
```

  * Quant:

```
001:023:000:STREAM             1604764566.989427
 01 00 00 00 04 00 F9 07 08 08 ED 07 E5 07 F9 0F
 F7 0F F7 0F F7 0F

001:023:000:STREAM             1604764567.989382
 01 00 00 00 00 00 F9 07 08 08 ED 07 E5 07 F9 0F
 F7 0F F7 0F F7 0F
```

  * Capture:

```
001:023:000:STREAM             1604764587.249423
 01 00 00 00 02 00 F9 07 08 08 ED 07 E5 07 F9 0F
 F7 0F F7 0F F7 0F

001:023:000:STREAM             1604764588.029435
 01 00 00 00 00 00 F9 07 08 08 ED 07 E5 07 F9 0F
 F7 0F F7 0F F7 0F
```

  * Shift:

```
001:023:000:STREAM             1604764606.589398
 01 00 00 80 00 00 F9 07 08 08 ED 07 E5 07 F9 0F
 F7 0F F7 0F F7 0F

001:023:000:STREAM             1604764607.459376
 01 00 00 00 00 00 F9 07 08 08 ED 07 E5 07 F9 0F
 F7 0F F7 0F F7 0F
```

  * Reverse:

```
001:023:000:STREAM             1604764627.609394
 01 00 00 40 00 00 F9 07 08 08 ED 07 E5 07 F9 0F
 F7 0F F7 0F F7 0F

001:023:000:STREAM             1604764628.219417
 01 00 00 00 00 00 F9 07 08 08 ED 07 E5 07 F9 0F
 F7 0F F7 0F F7 0F
```

  * Type:

```
001:023:000:STREAM             1604764645.498428
 01 00 00 20 00 00 F9 07 08 08 ED 07 E5 07 F9 0F
 F7 0F F7 0F F7 0F

001:023:000:STREAM             1604764646.068426
 01 00 00 00 00 00 F9 07 08 08 ED 07 E5 07 F9 0F
 F7 0F F7 0F F7 0F
```

  * Size:

```
001:023:000:STREAM             1604764663.388481
 01 00 00 10 00 00 F9 07 08 08 ED 07 E5 07 F9 0F
 F7 0F F7 0F F7 0F

001:023:000:STREAM             1604764664.008430
 01 00 00 00 00 00 F9 07 08 08 ED 07 E5 07 F9 0F
 F7 0F F7 0F F7 0F
```

  * Browse:

```
001:023:000:STREAM             1604764682.998399
 01 00 00 08 00 00 F9 07 08 08 ED 07 E5 07 F9 0F
 F7 0F F7 0F F7 0F

001:023:000:STREAM             1604764683.668444
 01 00 00 00 00 00 F9 07 08 08 ED 07 E5 07 F9 0F
 F7 0F F7 0F F7 0F
```

  * Matrix (x, y): (1, 1)

```
001:023:000:STREAM             1604764725.588455
 01 80 00 00 00 00 F9 07 08 08 ED 07 E5 07 F9 0F
 F7 0F F7 0F F7 0F

001:023:000:STREAM             1604764726.068444
 01 00 00 00 00 00 F9 07 08 08 ED 07 E5 07 F9 0F
 F7 0F F7 0F F7 0F
```

  * (2, 1)

```
001:023:000:STREAM             1604764749.478462
 01 40 00 00 00 00 F9 07 08 08 ED 07 E5 07 F9 0F
 F7 0F F7 0F F7 0F

001:023:000:STREAM             1604764749.938502
 01 00 00 00 00 00 F9 07 08 08 ED 07 E5 07 F9 0F
 F7 0F F7 0F F7 0F
```

  * (3, 1)

```
001:023:000:STREAM             1604764765.968502
 01 20 00 00 00 00 F9 07 08 08 ED 07 E5 07 F9 0F
 F7 0F F7 0F F7 0F

001:023:000:STREAM             1604764766.348502
 01 00 00 00 00 00 F9 07 08 08 ED 07 E5 07 F9 0F
 F7 0F F7 0F F7 0F
```

  * (4, 1)

```
001:023:000:STREAM             1604764782.208520
 01 10 00 00 00 00 F9 07 08 08 ED 07 E5 07 F9 0F
 F7 0F F7 0F F7 0F

001:023:000:STREAM             1604764782.558508
 01 00 00 00 00 00 F9 07 08 08 ED 07 E5 07 F9 0F
 F7 0F F7 0F F7 0F
```

  * (1, 2)

```
001:023:000:STREAM             1604764803.198524
 01 08 00 00 00 00 F9 07 08 08 ED 07 E5 07 F9 0F
 F7 0F F7 0F F7 0F

001:023:000:STREAM             1604764803.578518
 01 00 00 00 00 00 F9 07 08 08 ED 07 E5 07 F9 0F
 F7 0F F7 0F F7 0F
```

  * (2, 2)

```
001:023:000:STREAM             1604764820.168520
 01 04 00 00 00 00 F9 07 08 08 ED 07 E5 07 F9 0F
 F7 0F F7 0F F7 0F

001:023:000:STREAM             1604764820.488531
 01 00 00 00 00 00 F9 07 08 08 ED 07 E5 07 F9 0F
 F7 0F F7 0F F7 0F
```

  * (3, 2)

```
001:023:000:STREAM             1604764837.717499
 01 02 00 00 00 00 F9 07 08 08 ED 07 E5 07 F9 0F
 F7 0F F7 0F F7 0F

001:023:000:STREAM             1604764838.017497
 01 00 00 00 00 00 F9 07 08 08 ED 07 E5 07 F9 0F
 F7 0F F7 0F F7 0F
```

  * (4, 2)

```
001:023:000:STREAM             1604764856.597501
 01 01 00 00 00 00 F9 07 08 08 ED 07 E5 07 F9 0F
 F7 0F F7 0F F7 0F

001:023:000:STREAM             1604764856.897469
 01 00 00 00 00 00 F9 07 08 08 ED 07 E5 07 F9 0F
 F7 0F F7 0F F7 0F
```

  * (1, 3)

```
001:023:000:STREAM             1604764875.087624
 01 00 80 00 00 00 F9 07 08 08 ED 07 E5 07 F9 0F
 F7 0F F7 0F F7 0F

001:023:000:STREAM             1604764875.417509
 01 00 00 00 00 00 F9 07 08 08 ED 07 E5 07 F9 0F
 F7 0F F7 0F F7 0F
```

  * (2, 3)

```
001:023:000:STREAM             1604764891.427522
 01 00 40 00 00 00 F9 07 08 08 ED 07 E5 07 F9 0F
 F7 0F F7 0F F7 0F

001:023:000:STREAM             1604764891.707514
 01 00 00 00 00 00 F9 07 08 08 ED 07 E5 07 F9 0F
 F7 0F F7 0F F7 0F
```

  * (3, 3)

```
001:023:000:STREAM             1604764908.817580
 01 00 20 00 00 00 F9 07 08 08 ED 07 E5 07 F9 0F
 F7 0F F7 0F F7 0F

001:023:000:STREAM             1604764909.097567
 01 00 00 00 00 00 F9 07 08 08 ED 07 E5 07 F9 0F
 F7 0F F7 0F F7 0F
```

  * (4, 3)

```
001:023:000:STREAM             1604764924.217528
 01 00 10 00 00 00 F9 07 08 08 ED 07 E5 07 F9 0F
 F7 0F F7 0F F7 0F

001:023:000:STREAM             1604764924.477533
 01 00 00 00 00 00 F9 07 08 08 ED 07 E5 07 F9 0F
 F7 0F F7 0F F7 0F
```

  * (1, 4)

```
001:023:000:STREAM             1604764950.787566
 01 00 08 00 00 00 F9 07 08 08 ED 07 E5 07 F9 0F
 F7 0F F7 0F F7 0F

001:023:000:STREAM             1604764951.027588
 01 00 00 00 00 00 F9 07 08 08 ED 07 E5 07 F9 0F
 F7 0F F7 0F F7 0F
```

  * (2, 4)

```
001:023:000:STREAM             1604764967.117578
 01 00 04 00 00 00 F9 07 08 08 ED 07 E5 07 F9 0F
 F7 0F F7 0F F7 0F

001:023:000:STREAM             1604764967.257536
 01 00 00 00 00 00 F9 07 08 08 ED 07 E5 07 F9 0F
 F7 0F F7 0F F7 0F
```

  * (3, 4)

```
001:023:000:STREAM             1604764997.037596
 01 00 02 00 00 00 F9 07 08 08 ED 07 E5 07 F9 0F
 F7 0F F7 0F F7 0F

001:023:000:STREAM             1604764997.237595
 01 00 00 00 00 00 F9 07 08 08 ED 07 E5 07 F9 0F
 F7 0F F7 0F F7 0F
```

  * (4, 4)

```
001:023:000:STREAM             1604765011.687603
 01 00 01 00 00 00 F9 07 08 08 ED 07 E5 07 F9 0F
 F7 0F F7 0F F7 0F

001:023:000:STREAM             1604765011.797601
 01 00 00 00 00 00 F9 07 08 08 ED 07 E5 07 F9 0F
 F7 0F F7 0F F7 0F
```

  * Stop 1

```
001:023:000:STREAM             1604765031.536578
 01 00 00 00 80 00 F9 07 08 08 ED 07 E5 07 F9 0F
 F7 0F F7 0F F7 0F

001:023:000:STREAM             1604765031.746615
 01 00 00 00 00 00 F9 07 08 08 ED 07 E5 07 F9 0F
 F7 0F F7 0F F7 0F
```

  * Stop 2

```
001:023:000:STREAM             1604765044.206630
 01 00 00 00 40 00 F9 07 08 08 ED 07 E5 07 F9 0F
 F7 0F F7 0F F7 0F

001:023:000:STREAM             1604765044.436616
 01 00 00 00 00 00 F9 07 08 08 ED 07 E5 07 F9 0F
 F7 0F F7 0F F7 0F
```

  * Stop 3

```
001:023:000:STREAM             1604765057.096584
 01 00 00 00 20 00 F9 07 08 08 ED 07 E5 07 F9 0F
 F7 0F F7 0F F7 0F

001:023:000:STREAM             1604765057.286590
 01 00 00 00 00 00 F9 07 08 08 ED 07 E5 07 F9 0F
 F7 0F F7 0F F7 0F
```

  * Stop 4

```
001:023:000:STREAM             1604765068.686584
 01 00 00 00 10 00 F9 07 08 08 ED 07 E5 07 F9 0F
 F7 0F F7 0F F7 0F

001:023:000:STREAM             1604765068.906593
 01 00 00 00 00 00 F9 07 08 08 ED 07 E5 07 F9 0F
 F7 0F F7 0F F7 0F
```

  * Selector wheel button:

```
001:023:000:STREAM             1604765345.405619
 01 00 00 04 00 00 F9 07 08 08 ED 07 E5 07 F9 0F
 F7 0F F7 0F F7 0F

001:023:000:STREAM             1604765345.865703
 01 00 00 00 00 00 F9 07 08 08 ED 07 E5 07 F9 0F
 F7 0F F7 0F F7 0F
```

## Knobs

Knobs start in middle position, are turned left, then back to middle, then
right, then back to middle.
Detailed dumps are contained in `knobs/knob-KNOB-turn-FROM-TO.txt`.

## Faders

Faders start in top position, are moved down, then back up.
Detailed dumps are contained in `faders/fader-FADER-pull-FROM-TO.txt`.

## Selector wheel

The selector wheel is first turned left for a momend and then turned right for a moment.
Detailed dumps are contained in `wheel/wheel-DIRECTION.txt`.
