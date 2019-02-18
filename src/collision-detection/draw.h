#ifndef DRAW_H
#define DRAW_H
#include <GL/glut.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <stdio.h>
#include "player.h"
#include "blocks.h"
#include "Cuboid.h"

int lightOn[];
const int MAX_LIGHTS = 7;

void drawCube(Object* o);
void drawWithColor(Object* o);
void drawBullets();
void drawMap();
void lightSetup();
void initMaterial();
void psychedelic(int interval);
void setLightPos(int n, float x, float y, float z);
void initLights();
void drawCuboid(const Cuboid &c, float alpha = 1.0f);
void drawBox(const Box & c, float r, float g, float b, float alpha);
void drawTextAt(float x, float y, const char *string);
#endif
