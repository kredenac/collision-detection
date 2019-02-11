#ifndef MISCFUNC_H
#define MISCFUNC_H
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <string.h>
#include "player.h"
#include "draw.h"
#include "blocks.h"
constexpr auto TIMER_UPDATE_ID = 1;

// minimum timer callback time in milliseconds
const int UPDATE_TIMER_INTERVAL = 10;

// expected relative deltatime 
const float UPDATE_INTERVAL = 1 / 60.f;
extern int showFps;

extern float approach(float goal, float curr, float dt);
extern void setColor(Object* op, float r, float g, float b);
extern void set3fWithColor(Color c,float* r1, float* g1, float* b1);
extern Color getColor(Object* o);

extern void loadMap(int defaultMap);
extern void saveMap(void);
extern void resetGame(void);
extern void normalize3f(float* x, float* y, float* z);

#endif
