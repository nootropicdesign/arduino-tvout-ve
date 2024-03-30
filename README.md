# Arduino-TVout for Video Experimenter

This is a library for generating composite video on an ATmega microcontroller and is a modified version of TVout specifically for using features of the Video Experimenter shield. It allows video output to be overlaid on top of a video signal, allows frames to be captured and modified, etc. For more information, see the [Video Experimenter website](https://nootropicdesign.com/ve).

This repository is based upon the last published version on Google Code, known as Beta1. It has been patched to get around compilation issues associated with using assembly macros on newer versions of the Arduino IDE. The compilation problems with macros emerged after Arduino 1.6.8.

The three zip files TVout.zip, TVoutfonts.zip, and pollserial.zip can be installed in Arduino with Sketch > Include Library > Add .ZIP Library, or you can install the 3 libraries in your Arduino sketchbook with the following structure:


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

