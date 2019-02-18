#ifndef BLOCKS_H
#define BLOCKS_H
#include <stdio.h>
#include <stdlib.h>
#include "player.h"

#define DEFAULT_MAP "collision.map"

typedef struct ObjectNode{
    struct ObjectNode* next;
    Object* o;
}ObjectNode;

void addBlocks(float begx, float endx, float begy, float endy, float begz, float endz);
void setSizes(float x, float y, float z);
void initBlocks(void);
void addToList(ObjectNode** listHead, float x, float y, float z);
void removeNode(ObjectNode** listHead, ObjectNode* rm);
void freeList(ObjectNode** listHead);
void countList(ObjectNode* listHead);
extern int NUM_OF_BLOCKS;
extern const float scale;
extern ObjectNode* Blocks;
extern float sizex, sizey, sizez;

#endif
