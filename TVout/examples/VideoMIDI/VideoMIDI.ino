#include <TVout.h>
#include <fontALL.h>
#include <pollserial.h>
#include "reverse.h"
#define W 128
#define H 96
#define WB 16 // width in bytes
#define MIDI_EN A3
#define HISTORY_LEN 5
#define NOTE_ON 0x90
#define NOTE_OFF 0x80
#define CC 0xB0
#define CHANNEL 0 // actual channel (1) minus 1

TVout tv;
unsigned char x,y;
pollserial pserial;
byte noteHistory[HISTORY_LEN];
unsigned int noteOnCount = 0;
unsigned int noteOffCount = 0;

void setup()  {
  tv.begin(NTSC, W, H);
  initOverlay();
  initInputProcessing();

  tv.select_font(font4x6);
  tv.fill(0);

  pinMode(MIDI_EN, OUTPUT);
  digitalWrite(MIDI_EN, HIGH);

  pserial.begin(31250);

  for(byte i=0;i<HISTORY_LEN;i++) {
    noteHistory[i] = 0;
  }

  // Send control change message to turn all notes off
  pserial.write(((byte)CC | (CHANNEL & 0x0F)));
  pserial.write(123); // all notes off
  pserial.write((byte)0);
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
  int index;
  int sum = 0;
  int leftIndex, rightIndex;
  byte left, right;
  byte b;

  tv.capture();

  for(byte r=0;r<H;r++) {
    for(byte c=0;c<8;c++) {
      // Flip left side to right side
      leftIndex = (r*WB) + c;
      rightIndex = (r*WB) + ((WB-1)-c);
      left = display.screen[leftIndex];
      right = display.screen[rightIndex];

      display.screen[rightIndex] = pgm_read_byte(reverseTable + left);
      display.screen[leftIndex] = pgm_read_byte(reverseTable + right);
    }
  }


  tv.resume();

  // Count the number of pixels that are "on".
  for(byte r=0;r<H;r++) {
    for(byte c=0;c<8;c++) {
      index = (r*16) + c;
      b = display.screen[index];
      for(byte i=0;i<8;i++) {
      	sum += (b & 0x1);
      	b = b >> 1;
      }
    }
  }

  // map the number of pixels to a MIDI note.
  byte note = map(sum, 0, 10000, 20, 110);
  note = constrain(note, 20, 110);
  pserial.write(((byte)NOTE_ON | (CHANNEL & 0x0F)));
  pserial.write(note);
  pserial.write(100);  // velocity

  // delay a random amount from 5-15
  tv.delay_frame(5 + (sum % 11));
}

