#ifndef TVBLASTER_H
#define TVBLASTER_H

#define SIZEOF_EXPLOSION_BITMAP_RECORD 130
#define W 136
#define H 96
#define D 1
#define MINX 3
#define MAXX (W-4)
#define MINY 3
#define MAXY (H-4)
#define MAX_EXPLOSIONS 5

typedef struct Explosion {
  int x;
  int y;
  byte index; // index into the explosion bitmaps
} Explosion;


#endif
