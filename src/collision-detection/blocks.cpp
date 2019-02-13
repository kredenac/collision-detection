#include "blocks.h"

Object* createBlock();

const float scale = 2;
int NUM_OF_BLOCKS=0;
// list of all blocks, list because in build mode they can be added/removed
ObjectNode* Blocks=NULL;

// craetes new block at given coordinates
ObjectNode* createNode(float x, float y, float z)
{
    ObjectNode* newBlock= (ObjectNode*) malloc(sizeof(ObjectNode));
    if (newBlock == NULL){
        printf("Greska pri alokaciji novog ObjectNode\n");
        exit(EXIT_FAILURE);
    }
    newBlock->next = NULL;
    Object* obj = createBlock();
    obj->posx = x;
    obj->posy = y;
    obj->posz = z;
    newBlock->o = obj;
    return newBlock;
}

void countList(ObjectNode* listHead)
{
    int i=0;
    while (listHead != NULL) {
        listHead = listHead->next;
        i++;
    }
    printf("Broj elemenata liste je %d\n",i);
}

void freeList(ObjectNode** listHead)
{
    ObjectNode* p = NULL;
    while (*listHead != NULL) {
        p = (*listHead)->next;
        free((*listHead)->o);
        free(*listHead);

        *listHead = p;
    }
    NUM_OF_BLOCKS=0;
}

void addToList(ObjectNode** listHead, float x, float y, float z)
{
    ObjectNode* newBlock = createNode(x,y,z);
    newBlock->next = *listHead;
    *listHead = newBlock;
    NUM_OF_BLOCKS++;
}

void removeNode(ObjectNode** listHead, ObjectNode* rm)
{
    if (*listHead == rm ){
        (*listHead)=rm->next;
        free(rm->o);
        free(rm);
        NUM_OF_BLOCKS--;
        return;
    }
    ObjectNode* l=*listHead;
    while (l != NULL){
        if (l->next == rm ){
            l->next = rm->next;
            free(rm->o);
            free(rm);
            NUM_OF_BLOCKS--;
            return;
        }
        l = l->next;
    }
}

float sizex, sizey, sizez;
// sets the size of newly added blocks
void setSizes(float x, float y, float z)
{
    sizex = x;
    sizey = y;
    sizez = z;
}

// creates a new block with set sizes
Object* createBlock()
{
    Object* novi = (Object*) malloc(sizeof(Object));
    if (novi == NULL){
        printf("Greska pri alokaciji novog Object-a\n");
        exit(EXIT_FAILURE);
    }
    novi->length = sizex;
    novi->height = sizey;
    novi->width = sizez;

    novi->color[0] = 0.5;
    novi->color[1] = 0.5;
    novi->color[2] = 0.5;
    return novi;
}

#define SWAP(x, y, T) do {T SWAP = x; x = y; y = SWAP;} while (0)
// fills the given volume with cubes
void addBlocks(float begx, float endx, float begy, float endy, float begz, float endz)
{
    if (begx > endx) SWAP(begx, endx, float);
    if (begy > endy) SWAP(begy, endy, float);
    if (begz > endz) SWAP(begz, endz, float);
    int count = 0;
    begx *= scale, endx *= scale, begy *= scale;
    endy *= scale, begz *= scale, endz *= scale;
    float x, y, z;
    for (x = begx; x <= endx; x += sizex) {
        for (y = begy; y <= endy; y += sizey) {
            for (z = begz; z <= endz; z += sizez) {
                count++;
                addToList(&Blocks, x, y, z);
            }
        }
    }
}

// loads the default map
void initBlocks()
{
    freeList(&Blocks);
    loadMap(1);
}
