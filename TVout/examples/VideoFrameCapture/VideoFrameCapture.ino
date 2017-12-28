#include <TVout.h>
#include <fontALL.h>
#define W 128
#define H 96

TVout tv;
unsigned char x,y;
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

ISR(INT0_vect) {
  display.scanLine = 0;
}

void loop() {
  tv.capture();
  //tv.fill(INVERT);
  tv.resume();
  tv.delay_frame(5);
  delay(1);
}

