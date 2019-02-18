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
const int TIMER_UPDATE_ID = 1;

// minimum timer callback time in milliseconds
const int UPDATE_TIMER_INTERVAL = 10;

// expected relative deltatime 
const float UPDATE_INTERVAL = 1000.f / 60.f;
extern int showFps;

float approach(float goal, float curr, float dt);
void setColor(Object* op, float r, float g, float b);
void set3fWithColor(Color c,float* r1, float* g1, float* b1);
Color getColor(Object* o);

void loadMap(int defaultMap);
void saveMap(void);
void resetGame(void);
void normalize3f(float* x, float* y, float* z);

#endif
