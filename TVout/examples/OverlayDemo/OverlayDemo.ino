#include <TVout.h>
#include <fontALL.h>

#define W 136
#define H 96

TVout tv;
unsigned char x,y;
unsigned char originx = 5;
unsigned char originy = 80;
unsigned char plotx = originx;
unsigned char ploty = 40;
char s[32];
int index = 0;
int messageLen = 32;
char message[] = "...OVERLAY TEXT AND GRAPHICS ON A VIDEO SIGNAL...OVERLAY TEXT AND GRAPHICS ON A VIDEO SIGNAL";
char saveChar;
byte ledState = LOW;

void setup()  {
  tv.begin(NTSC, W, H);
  initOverlay();
  tv.select_font(font6x8);
  tv.fill(0);
  drawGraph();
  randomSeed(analogRead(0));
}

// Initialize ATMega registers for video overlay capability.
// Must be called after tv.begin().
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
  saveChar = message[index+22];
  message[index+22] = '\0';

  for(int x=6;x>=0;x--) {
    if (x<6) {
      tv.delay_frame(1);
    } 
    tv.print(x, 87, message+index);

    for(byte y=87;y<96;y++) {
      tv.draw_line(0, y, 5, y, 0);
      tv.draw_line(128, y, 134, y, 0);
    }

  }

  message[index+22] = saveChar;
  index++;
  if (index > 45) {
    index = 0;
  }

  sprintf(s, "%ums", millis());
  tv.print(0, 0, s);


  if (plotx++ > 120) {
    tv.fill(0);
    drawGraph();
    plotx = originx + 1;
    return;
  }
  byte newploty = ploty + random(0, 7) - 3;
  newploty = constrain(newploty, 15, originy);
  tv.draw_line(plotx-1, ploty, plotx, newploty, 1);
  ploty = newploty;
}


void drawGraph() {
  tv.draw_line(originx, 15, originx, originy, 1);
  tv.draw_line(originx, originy, 120, originy, 1);
  for(byte y=originy;y>15;y -= 4) {
    tv.set_pixel(originx-1, y, 1);
    tv.set_pixel(originx-2, y, 1);
  }
  for(byte x=originx;x<120;x += 4) {
    tv.set_pixel(x, originy+1, 1);
    tv.set_pixel(x, originy+2, 1);
  }
}
