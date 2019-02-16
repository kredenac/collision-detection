#ifndef COLLISION_H
#define COLLISION_H
#include <stdio.h>
#include <math.h>
#include "player.h"
#include "miscfunc.h"
#include "draw.h"
#include "blocks.h"
typedef enum {
    TOP,
    BOT,
    FRONT,//z+
    BACK,//z-
    LEFT,//x-
    RIGHT//x+
}Side;
void playerCollision(void);
void bulletCollision(void);
#endif
