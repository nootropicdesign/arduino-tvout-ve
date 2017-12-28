#include <TVout.h>
#include <pollserial.h>
#include <fontALL.h>

TVout TV;
pollserial pserial;

void setup()  {
  TV.begin(_NTSC,184,72);
  TV.select_font(font6x8);
  initOverlay();
  TV.println("Serial Terminal");
  TV.println("-- Version 0.1 --");
  TV.set_hbi_hook(pserial.begin(57600));
}

void initOverlay() {
  TCCR1A = 0;
  // Enable timer1.  ICES0 is set to 0 for falling edge detection on input capture pin.
  TCCR1B = _BV(CS10);

  // Enable input capture interrupt
  TIMSK1 |= _BV(ICIE1);

  // Enable external interrupt INT0 on pin 2 with falling edge.
  EIMSK = _BV(INT0);
  EICRA = _BV(ISC01);
}

// Required to reset the scan line when the vertical sync occurs
ISR(INT0_vect) {
  display.scanLine = 0;
}

void loop() {
  if (pserial.available()) {
    TV.print((char)pserial.read());
  }
}