# Arduino-TVout

This is a library for generating composite video on an ATmega microcontroller. Hosted here to keep it available from Arduino IDE (and for simple download as the original Google Code has been archived).

This repository is based upon the last published version on Google Code, known as Beta1. It has been patched to get around compilation issues associated with using assembly macros on newer versions of the Arduino IDE. The compilation problems with macros emerged after Arduino 1.6.8.

Install the 3 libraries in your Arduino sketchbook with the following structure:

```
Arduino sketchbook
    |
    +--libraries
          |
          +--TVout
          |     |
          |     +--...
          |
          +--TVoutfonts
          |     |
          |     +--...
          |
          +--pollserial
                |
                +--...
```

Currently the output is NTSC or PAL at a resolution of 128x96 by default. The library currently works on ATmega168,328,1280,2560,644p,1284p,32U4,AT90USB1286 and more can be added by editing spec/hardware_setup.h.

There are some timing issues with the m1284p, may be related to sanguino core.

MCU | SYNC | VIDEO | AUDIO | Arduino | SYNC | VIDEO | AUDIO
---|---|---|---|---|---|---|---
m168,m328 | B 1 | D 7 | B 3 | NG,Decimila,UNO | 9 | 7 | 11
m1280,m2560 | B 5 | A 7 | B 4 | Mega | 11 | A7(D29) | 10
m644,m1284p | D 5 | A 7 | D 7 | sanguino | 13 | A7(D24) | 8
m32u4 | B 5 | B 4 | B 7 | Leonardo | 9 | 8 | 11
AT90USB1286 | B 5 | F 7 | B 4 | -- | -- | -- | --

## Connections

SYNC is on OCR1A and AUDIO is on OCR2A (except on the Arduino Leonardo, where AUDIO is on OCR0A)

There are some timing issues with the m1284p, may be related to sanguino core.

On NG, Decimila, UNO and Nano the sync is pin 9, video on 7 and audio on 11. On Mega2560	sync is pin 11, video is on A7(D29) and audio is on pin 10.
