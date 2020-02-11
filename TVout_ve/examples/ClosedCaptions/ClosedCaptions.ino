#include <TVout_ve.h>
#include <fontALL.h>
#include <pollserial.h>
#define W 128
#define H 96
#define BITWIDTH 5
#define THRESHOLD 3

TVout_ve tv;
pollserial pserial;
unsigned char x, y;
char s[32];
int start = 40;
unsigned char ccdata[16];
// TiVo, DVD player
byte bpos[][8] = {{26, 32, 38, 45, 51, 58, 64, 70}, {78, 83, 89, 96, 102, 109, 115, 121}};
// VCR
//byte bpos[][8] = {{27, 33, 40, 46, 52, 59, 65, 72}, {78, 84, 91, 97, 103, 110, 116, 123}};
boolean wroteOutput = false;
boolean newline = false;
char c[2];
char lastControlCode[2];
int line = 13;
int dataCaptureStart = 310;
unsigned int loopCount = 0;

void setup()  {
  tv.begin(_NTSC, W, H);
  tv.set_hbi_hook(pserial.begin(57600));
  initOverlay();
  initInputProcessing();

  tv.setDataCapture(line, dataCaptureStart, ccdata);

  y = 0;

  tv.select_font(font6x8);
  tv.fill(0);

  // uncomment this to display the bit positions on the screen
  // for alignment/debugging
  //displayBitPositions();
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
  wroteOutput = false;
  for (x = 0; x < display.hres; x++) {
    ccdata[x] = 0;
  }
}


// display the captured data line on the screen
void displayccdata() {
  y = 0;
  for (x = 0; x < display.hres; x++) {
    display.screen[(y * display.hres) + x] = ccdata[x];
  }
}


void loop() {
  byte pxsum;
  byte i;
  byte parityCount;
  loopCount++;


  // Use this code to adjust the data capture line using a
  // potentiometer connected to A5.
  // The default value is line 13.
  /*
    if (loopCount % 1000 == 0) {
    line = getValue();
    line = (line / 10) - 1;
    displayValue(line);
    tv.setDataCapture(line, dataCaptureStart, ccdata);
    }
  */


  // Use this code to adjust the data capture start time using a
  // potentiometer connected to A5.
  // The default value is 310
  /*
    if (loopCount % 1000 == 0) {
    dataCaptureStart = getValue();
    displayValue(dataCaptureStart);
    tv.setDataCapture(line, dataCaptureStart, ccdata);
    }
  */

  // Display the captured data line to the screen so we can see it.
  displayccdata();

  if ((ccdata[0] > 0) && (!wroteOutput)) {
    // we have new data to decode
    for (byte bytenum = 0; bytenum < 2; bytenum++) {
      c[bytenum] = 0;
      parityCount = 0;
      for (int bit = 0; bit < 8; bit++) {
        pxsum = 0;
        for (int w = 0; w < BITWIDTH; w++) {
          i = bpos[bytenum][bit] + w;
          if (((ccdata[i / 8] >> (7 - (i % 8))) & 1) == 1) {
            pxsum++;
          }
        }
        if (pxsum >= THRESHOLD) {
          // consider the bit to be "on"
          c[bytenum] |= (1 << bit);
          parityCount++;
        }
      }

      if ((parityCount % 2) == 1) {
        // parity check matches
        // strip off the MSB because it's the parity bit
        c[bytenum] &= 0x7F;
      } else {
        // parity check failed
        c[bytenum] = 0;
      }
    }

    // output the data
    if ((c[0] > 0) && (c[0] < ' ')) {
      // control character
      if ((c[0] != lastControlCode[0]) && (c[1] != lastControlCode[1])) {
        if (!newline) {
          pserial.write('\n');
          newline = true;
        }
        lastControlCode[0] = c[0];
        lastControlCode[1] = c[1];
      }
    } else {
      if (c[0] > 0) {
        newline = false;
        lastControlCode[0] = 0;
        pserial.write(c[0]);
        //tv.print(c[0]);
      }
      if (c[1] > 0) {
        newline = false;
        lastControlCode[0] = 0;
        pserial.write(c[1]);
        //tv.print(c[1]);
      }
    }
    wroteOutput = true;
  }
}

int getValue() {
  int value;
  ADCSRA |= _BV(ADEN); // enable ADC
  value = analogRead(5);
  initInputProcessing();
  return value;
}
void displayValue(int v) {
  tv.print(0, 3, "        ");
  sprintf(s, "%i", v);
  tv.print(0, 3, s);
}

// for alignment debugging
void displayTicks() {
  y = 2;
  for (x = 0; x < W; x++) {
    if ((x % 2) == 0) {
      tv.set_pixel(x, y, 1);
    }
  }
  y = 3;
  for (x = 0; x < W; x++) {
    if ((x % 5) == 0) {
      tv.set_pixel(x, y, 1);
    }
  }
  y = 4;
  for (x = 0; x < W; x++) {
    if ((x % 10) == 0) {
      tv.set_pixel(x, y, 1);
    }
  }
}

void displayBitPositions() {
  y = 1;
  tv.draw_line(0, y, W - 1, y, 0);
  for (byte bytenum = 0; bytenum < 2; bytenum++) {
    for (byte bit = 0; bit < 8; bit++) {
      tv.set_pixel(bpos[bytenum][bit], y, 1);
    }
  }

}
