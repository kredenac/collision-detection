#ifndef KEYBOARD_H
#define KEYBOARD_H
#include <GL/glut.h>
#include "player.h"
#include "miscfunc.h"

void onReshape(int width, int height);
void onKeyboard(unsigned char key, int x, int y);
void onKeyboardUp(unsigned char key, int x, int y);
void onSpecialInput(int key, int x, int y);
void onSpecialInputUp(int key, int x, int y);
void onMouseLook(int x, int y);
void onMouseButton(int button, int state, int x, int y);
void onMousePressedLook(int x, int y);
void onKeyHold(void);
extern float aspectRatio;
extern int initWindowHeight;
#endif
