#include <TVout_ve.h>
#include <fontALL.h>
#define W 128
#define H 96

TVout_ve tv;
unsigned char x, y;
unsigned char c;
unsigned char minX, minY, maxX, maxY;
char s[32];


void setup()  {
  tv.begin(NTSC, W, H);
  initOverlay();
  initInputProcessing();

  tv.select_font(font4x6);
  tv.fill(0);
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

void initInputProcessing() {
  // Analog Comparator setup
  ADCSRA &= ~_BV(ADEN); // disable ADC
  ADCSRB |= _BV(ACME); // enable ADC multiplexer
  ADMUX &= ~_BV(MUX0);  // select A2 for use as AIN1 (negative voltage of comparator)
  ADMUX |= _BV(MUX1);
  ADMUX &= ~_BV(MUX2);
  ACSR &= ~_BV(ACIE);  // disable analog comparator interrupts
  ACSR &= ~_BV(ACIC);  // disable analog comparator input capture
}

// Required
ISR(INT0_vect) {
  display.scanLine = 0;
}


void loop() {
  tv.capture();

  // uncomment if tracking dark objects
  //tv.fill(INVERT);

  // compute bounding box
  minX = W;
  minY = H;
  maxX = 0;
  maxY = 0;
  boolean found = 0;
  for (int y = 0; y < H; y++) {
    for (int x = 0; x < W; x++) {
      c = tv.get_pixel(x, y);
      if (c == 1) {
        found = true;
        if (x < minX) {
          minX = x;
        }
        if (x > maxX) {
          maxX = x;
        }
        if (y < minY) {
          minY = y;
        }
        if (y > maxY) {
          maxY = y;
        }
      }
    }
  }

  // draw bounding box
  tv.fill(0);
  if (found) {
    tv.draw_line(minX, minY, maxX, minY, 1);
    tv.draw_line(minX, minY, minX, maxY, 1);
    tv.draw_line(maxX, minY, maxX, maxY, 1);
    tv.draw_line(minX, maxY, maxX, maxY, 1);
    sprintf(s, "%d, %d", ((maxX + minX) / 2), ((maxY + minY) / 2));
    tv.print(0, 0, s);
  } else {
    tv.print(0, 0, "not found");
  }


  tv.resume();
  tv.delay_frame(5);
}

