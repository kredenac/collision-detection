#ifndef PLAYER_H
#define PLAYER_H
#include <stdio.h>
#include <math.h>

typedef enum {
    WHITE,
    BLUE,
    ORANGE,
    BLACK,
    OTHER
}Color;


const float MAX_ELEVATION = 89.f;
const int MAX_BULLETS = 100;

// used for interpolation
typedef struct{
    float curr;
    float goal;
}Val2f;
// struct for cubes that make the map
typedef struct{
    float posx;
    float posy;
    float posz;
    Val2f vx;
    Val2f vy;
    Val2f vz;
    float length;//x
    float height;//y
    float width;//z
    float color[3];
}Object;

typedef struct{
    int jumping;
    Color fireColor;
    int bigJump;
    int goFast;
    int finishedGame;
    int newGame;
    int buildMode;
    int flying;
}State;


extern State state;
extern const State stateInit;
//relative position of head to center of the body, used for camera
extern float playerHeadHeight;
extern const Object playerInit;
void paintBlock(Object* block, Object* bullet);
void movePlayer(void);
void firePaint(void);
void resetBullets(void);
void moveBullets(void);
void positionCam(void);
void onKeyHold(void);
void checkEvents(void);
void jump(void);
void fireBlackPaint(void);
void flyDown(void);
void toggleBuildMode(void);
void playerOnBlockReact(Object* p);

extern Object player;
extern float GRAVITY;
extern float rotWorld;
extern Val2f viewAzimuth;
extern Val2f viewElevation;
extern float JUMP_V;
extern Object player;
extern int animationOngoing;

extern Val2f viewAzimuth;
extern Val2f viewElevation;
extern float eyex, eyey, eyez;
extern float lookAtx, lookAty, lookAtz;
extern float upx, upy, upz;

extern Object bullets[];
extern int bulletsActive[];

// included here to avoid circular including
#include "miscfunc.h"
#endif
