#include <TVout.h>
#include <fontALL.h>
#include <Controllers.h>
#include "TVBlaster.h"
#include "explosions.h"


TVout tv;
int oldCrosshairX, oldCrosshairY;
int crosshairX, crosshairY;
int laserX, laserY;
char s[32];
Explosion explosions[MAX_EXPLOSIONS];
int explosionIndex;
int nExplosions = 0;
boolean randomPlacement = false;
unsigned long loopCount = 0;
boolean useNunchuk;

void setup() {
  tv.begin(NTSC, W, H);
  useNunchuk = Nunchuk.init(tv, 4);
  initOverlay();
  tv.select_font(font6x8);
  crosshairX = W / 2;
  crosshairY = H / 2;
  oldCrosshairX = crosshairX;
  oldCrosshairY = crosshairY;
  for (int i = 0; i < MAX_EXPLOSIONS; i++) {
    explosions[i].index = 255;
  }
  explosionIndex = 0;
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

ISR(INT0_vect) {
  display.scanLine = 0;
}

void loop() {
  loopCount++;

  drawCrosshair();
  tv.delay_frame(1);
  getInput();
  drawExplosions();

}

void drawExplosions() {
  for (byte i = 0; i < MAX_EXPLOSIONS; i++) {
    if (explosions[i].index != 255) {
      if (explosions[i].index > 1) {
        erasebitmap(explosions[i].x, explosions[i].y, explosion_bitmaps + ((explosions[i].index - 1) * SIZEOF_EXPLOSION_BITMAP_RECORD), 0, 0, 0);
      }

      if (explosions[i].index == 2) {
        tv.draw_line(0, H - 1, laserX, laserY, 0);
        tv.draw_line(W - 1, H - 1, laserX, laserY, 0);
        laserX = -1;
        laserY = -1;
        erasebitmap(explosions[i].x, explosions[i].y, explosion_bitmaps, 0, 0, 0);
      }
      if (explosions[i].index > 5) {
        explosions[i].index = 255;
        nExplosions--;
      } else {
        overlaybitmap(explosions[i].x, explosions[i].y, explosion_bitmaps + (explosions[i].index++ * SIZEOF_EXPLOSION_BITMAP_RECORD), 0, 0, 0);
      }
    }
  }
}

void drawCrosshair() {
  tv.set_pixel(oldCrosshairX, oldCrosshairY - 3, 0);
  tv.set_pixel(oldCrosshairX, oldCrosshairY - 2, 0);
  tv.set_pixel(oldCrosshairX, oldCrosshairY + 2, 0);
  tv.set_pixel(oldCrosshairX, oldCrosshairY + 3, 0);

  tv.set_pixel(oldCrosshairX - 3, oldCrosshairY, 0);
  tv.set_pixel(oldCrosshairX - 2, oldCrosshairY, 0);
  tv.set_pixel(oldCrosshairX + 2, oldCrosshairY, 0);
  tv.set_pixel(oldCrosshairX + 3, oldCrosshairY, 0);

  tv.set_pixel(crosshairX, crosshairY - 3, 1);
  tv.set_pixel(crosshairX, crosshairY - 2, 1);
  tv.set_pixel(crosshairX, crosshairY + 2, 1);
  tv.set_pixel(crosshairX, crosshairY + 3, 1);

  tv.set_pixel(crosshairX - 3, crosshairY, 1);
  tv.set_pixel(crosshairX - 2, crosshairY, 1);
  tv.set_pixel(crosshairX + 2, crosshairY, 1);
  tv.set_pixel(crosshairX + 3, crosshairY, 1);
}

void getInput() {
  int jx, jy;
  if (useNunchuk && ((loopCount % 1) == 0)) {
    Nunchuk.getData();
  }
  oldCrosshairX = crosshairX;
  oldCrosshairY = crosshairY;
  if (useNunchuk) {
    jx = Nunchuk.getJoystickX();
    if (jx < 100) {
      crosshairX -= D;
      if (crosshairX < MINX) {
        crosshairX = MINX;
      }
    }
    if (jx > 180) {
      crosshairX += D;
      if (crosshairX > MAXX) {
        crosshairX = MAXX;
      }
    }

    jy = Nunchuk.getJoystickY();
    if (jy > 180) {
      crosshairY -= D;
      if (crosshairY < MINY) {
        crosshairY = MINY;
      }
    }
    if (jy < 100) {
      crosshairY += D;
      if (crosshairY > MAXY) {
        crosshairY = MAXY;
      }
    }
  }


  if ((loopCount % 3) == 0) {
    if (useNunchuk && (Nunchuk.getButtonZ() == 1)) {

      if (laserX != -1) {
        tv.draw_line(0, H - 1, laserX, laserY, 0);
        tv.draw_line(W - 1, H - 1, laserX, laserY, 0);
      }
      laserX = crosshairX;
      laserY = crosshairY;
      //tv.draw_line(0, H-1, laserX, laserY, 1);
      //tv.draw_line(W-1, H-1, laserX, laserY, 1);
      if (explosions[explosionIndex].index != 255) {
        erasebitmap(explosions[explosionIndex].x, explosions[explosionIndex].y, explosion_bitmaps, 0, 0, 0);
        erasebitmap(explosions[explosionIndex].x, explosions[explosionIndex].y, explosion_bitmaps + ((explosions[explosionIndex].index - 1) * SIZEOF_EXPLOSION_BITMAP_RECORD), 0, 0, 0);
      } else {
        nExplosions++;
      }

      if (randomPlacement) {
        explosions[explosionIndex].x = crosshairX - 19 + random(0, 4);
        explosions[explosionIndex].y = crosshairY - 17 + random(0, 4);
      } else {
        explosions[explosionIndex].x = crosshairX - 17;
        explosions[explosionIndex].y = crosshairY - 15;
      }

      overlaybitmap(explosions[explosionIndex].x, explosions[explosionIndex].y, explosion_bitmaps, 0, 0, 0);
      explosions[explosionIndex].index = 1;
      if (++explosionIndex == MAX_EXPLOSIONS) {
        explosionIndex = 0;
      }
    }
  }


}

void overlaybitmap(int x, int y, const unsigned char * bmp,
                   uint16_t i, uint8_t width, uint8_t lines) {

  uint8_t temp, lshift, rshift, save, xtra;
  uint16_t si = 0;

  rshift = x & 7;
  lshift = 8 - rshift;

  if (width == 0) {
    width = pgm_read_byte((uint32_t)(bmp) + i);
    i++;
  }
  if (lines == 0) {
    lines = pgm_read_byte((uint32_t)(bmp) + i);
    i++;
  }

  if (width & 7) {
    xtra = width & 7;
    width = width / 8;
    width++;
  } else {
    xtra = 8;
    width = width / 8;
  }

  for (uint8_t l = 0; l < lines; l++) {
    if (((y + l) < 0) || ((y + l) >= display.vres)) {
      i += width;
      continue;
    }
    si = (y + l) * display.hres + x / 8;
    if (width == 1)
      temp = 0xff >> rshift + xtra;
    else
      temp = 0;
    temp = pgm_read_byte((uint32_t)(bmp) + i++);
    boolean wrapped = false;
    if (x >= 0) {
      display.screen[si++] |= temp >> rshift;
      for (uint16_t b = i + width - 1; i < b; i++) {
        if (!wrapped) {
          display.screen[si] |= temp << lshift;
          temp = pgm_read_byte((uint32_t)(bmp) + i);
          display.screen[si] |= temp >> rshift;
        }
        if (((si) / display.hres) < ((si + 1) / display.hres)) {
          wrapped = true;
        }
        si++;
      }
      if (!wrapped) {
        display.screen[si] |= temp << lshift;
      }
    } else {
      // x < 0
      if (rshift == 0) {
        si++;
      }
      wrapped = true;
      for (uint16_t b = i + width - 1; i < b; i++) {
        if (wrapped && ((si / display.hres) != ((si - 1) / display.hres))) {
          wrapped = false;
        }
        if (!wrapped) {
          display.screen[si] |= temp << lshift;
          temp = pgm_read_byte((uint32_t)(bmp) + i);
          display.screen[si++] |= temp >> rshift;
        } else {
          si++;
          temp = pgm_read_byte((uint32_t)(bmp) + i);
        }
      }
      display.screen[si] |= temp << lshift;

    }
  }
} // end of overlaybitmap


void erasebitmap(int x, int y, const unsigned char * bmp,
                 uint16_t i, uint8_t width, uint8_t lines) {

  uint8_t temp, lshift, rshift, xtra;
  uint16_t si = 0;

  rshift = x & 7;
  lshift = 8 - rshift;
  if (width == 0) {
    width = pgm_read_byte((uint32_t)(bmp) + i);
    i++;
  }
  if (lines == 0) {
    lines = pgm_read_byte((uint32_t)(bmp) + i);
    i++;
  }

  if (width & 7) {
    xtra = width & 7;
    width = width / 8;
    width++;
  } else {
    xtra = 8;
    width = width / 8;
  }

  for (uint8_t l = 0; l < lines; l++) {
    if (((y + l) < 0) || ((y + l) >= display.vres)) {
      i += width;
      continue;
    }
    si = (y + l) * display.hres + x / 8;
    if (width == 1)
      temp = 0xff >> rshift + xtra;
    else
      temp = 0;
    temp = pgm_read_byte((uint32_t)(bmp) + i++);
    boolean wrapped = false;
    if (x >= 0) {
      display.screen[si++] &= ~(temp >> rshift);
      for (uint16_t b = i + width - 1; i < b; i++) {
        if (!wrapped) {
          display.screen[si] &= ~(temp << lshift);
          temp = pgm_read_byte((uint32_t)(bmp) + i);
          display.screen[si] &= ~(temp >> rshift);
        }
        if (((si) / display.hres) < ((si + 1) / display.hres)) {
          wrapped = true;
        }
        si++;
      }
      if (!wrapped) {
        if (rshift + xtra - 8 > 0)
          display.screen[si] &= ~(temp << lshift);
      }
    } else {
      // x < 0
      if (rshift == 0) {
        si++;
      }
      wrapped = true;
      for (uint16_t b = i + width - 1; i < b; i++) {
        if (wrapped && ((si / display.hres) != ((si - 1) / display.hres))) {
          wrapped = false;
        }
        if (!wrapped) {
          display.screen[si] &= ~(temp << lshift);
          temp = pgm_read_byte((uint32_t)(bmp) + i);
          display.screen[si++] &= ~(temp >> rshift);
        } else {
          si++;
          temp = pgm_read_byte((uint32_t)(bmp) + i);
        }
      }
      if (rshift + xtra - 8 > 0)
        display.screen[si] &= ~(temp << lshift);
    }
  }
} // end of erasebitmap

