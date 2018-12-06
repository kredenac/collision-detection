#ifndef DRAW_H
#define DRAW_H
#include <GL/glut.h>
#include <math.h>
#include <stdio.h>
#include "player.h"
#include "blocks.h"

extern int lightOn[];
#define MAX_LIGHTS 8

extern void drawCube(Object* o);
extern void drawWithColor(Object* o);
extern void drawBullets(void);
extern void drawMap(void);
extern void lightSetup(void);
extern void initMaterial(void);
extern void psychedelic(int interval);
extern void setLightPos(int n, float x, float y, float z);
extern void initLights();
#endif
