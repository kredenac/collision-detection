#ifndef BLOCKS_H
#define BLOCKS_H
#include <stdio.h>
#include <stdlib.h>
#include "player.h"

#define DEFAULT_MAP "hello.map"

typedef struct ObjectNode{
    struct ObjectNode* next;
    Object* o;
}ObjectNode;

extern void addBlocks(float begx, float endx, float begy, float endy, float begz, float endz);
extern void setSizes(float x, float y, float z);
extern void initBlocks(void);
extern void addToList(ObjectNode** listHead, float x, float y, float z);
extern void removeNode(ObjectNode** listHead, ObjectNode* rm);
extern void freeList(ObjectNode** listHead);
extern void countList(ObjectNode* listHead);
extern int NUM_OF_BLOCKS;
extern const float scale;
extern ObjectNode* Blocks;
extern float sizex, sizey, sizez;

#endif
