#ifndef DRAW_H
#define DRAW_H
#include <GL/glut.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <stdio.h>
#include "player.h"
#include "blocks.h"
#include "Cuboid.h"

extern int lightOn[];
#define MAX_LIGHTS 7

extern void drawCube(Object* o);
extern void drawWithColor(Object* o);
extern void drawBullets(void);
extern void drawMap(void);
extern void lightSetup(void);
extern void initMaterial(void);
extern void psychedelic(int interval);
extern void setLightPos(int n, float x, float y, float z);
extern void initLights();
void drawCuboid(const Cuboid &c, float alpha = 1.0f);
void drawBox(const Box & c, float r, float g, float b, float alpha);
void drawTextAt(float x, float y, const char *string);
#endif
