#include <TVout_ve.h>
#include <fontALL.h>
#include "edge_templates.h"

#define W 128
#define H 96

TVout_ve tv;
int x,y;
unsigned char tmp1[W/8];
unsigned char tmp2[W/8];


void setup()  {
  tv.begin(NTSC, W, H);
  initOverlay();
  initInputProcessing();
  tv.select_font(font6x8);
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

  // Tell the enhanced TVout_ve library to capture the next full frame.
  tv.capture();

  // Invert the image to look for dark areas.
  tv.fill(INVERT);

  // Resume rendering so we can see the processing.
  // This makes processing much slower.  If desired, move this resume() to end
  // of loop just before delay_frame(10);
  tv.resume();

  processLine(1, tmp1);
  processLine(2, tmp2);

  for(y=3;y<H-1;y++) {
    for(x=0;x<display.hres;x++) {
      // copy contents of tmp1 to line y-2
      display.screen[((y-2)*display.hres) + x] = tmp1[x];
      // copy contents of tmp2 to tmp1
      tmp1[x] = tmp2[x];
    }
    processLine(y, tmp2);
  }

  // write last two lines of edge data to screen
  for(x=0;x<display.hres;x++) {
    // copy contents of tmp1 to line H-3
    display.screen[((H-3)*display.hres) + x] = tmp1[x];
    // copy contents of tmp2 to line H-2
    display.screen[((H-2)*display.hres) + x] = tmp2[x];
  }

  //tv.resume();
  tv.delay_frame(5);
}


// This is an implementation of local threshold and boolean
// function based edge detection.
void processLine(int y, unsigned char *d) {
  byte b0, b1, b2;
  unsigned int index;
  byte shift;

  // first, clear the data in the destination buffer
  for(byte i=0;i<display.hres;i++) {
    d[i] = 0;
  }

  for(x=1;x<W-1;x++) {
    b0 = 0;
    b1 = 0;
    b2 = 0;
    // bytes b0, b1, and b2 hold the 3x3 grid of pixels centered around (x,y)
    index = (y*display.hres) + (x/8);
    if ((x&7) == 0) {
      b0 = ((display.screen[index-display.hres-1] << 2) | (display.screen[index-display.hres] >> 6)) & 7;
      b1 = ((display.screen[index-1] << 2) | (display.screen[index] >> 6)) & 7;
      b2 = ((display.screen[index+display.hres-1] << 2) | (display.screen[index+display.hres] >> 6)) & 7;
    } else {
      if ((x&7) < 7) {
	shift = 6-(x&7);
	b0 = (display.screen[index-display.hres] >> shift) & 7;
	b1 = (display.screen[index] >> shift) & 7;
	b2 = (display.screen[index+display.hres] >> shift) & 7;
      } else {
	b0 = ((display.screen[index-display.hres] << 1) | (display.screen[index-display.hres+1] >> 7)) & 7;
	b1 = ((display.screen[index] << 1) | (display.screen[index+1] >> 7)) & 7;
	b2 = ((display.screen[index+display.hres] << 1) | (display.screen[index+display.hres+1] >> 7)) & 7;
      }
    }


    // match the 3x3 square centered at (x,y) with the 16 edge templates.
    for(int e=0;e<16;e++) {
      if ((edges[e][0] == b0) && (edges[e][1] == b1) && (edges[e][2] == b2)) {
	// The 3x3 grid matches edge template e.
	// Set the bit (x,y) in the temporary buffer d to a 1
	d[x/8] |= (0x80 >> (x&7));
	break;
      }
    }
  }
}
