#include "miscfunc.h"
#pragma warning(push)
#pragma warning(disable: 4305)
#pragma warning(disable: 4244)

int showFps = 0;

static int isequal(float a, float b);

// linear interpolation
float approach(float goal, float curr, float dt)
{
    float diff = goal - curr;
    if (diff > dt) {
        return curr + dt;
    } else if (diff < -dt) {
        return curr - dt;
    }
    return goal;
}

void normalize3f(float* x, float* y, float* z)
{
    float n = sqrt(*x * *x +  *y * *y + *z * *z);
    *x = *x / n;
    *y = *y / n;
    *z = *z / n;
}

void setColor(Object* op, float r, float g, float b)
{
    op->color[0] = r;
    op->color[1] = g;
    op->color[2] = b;
}

void set3fWithColor(Color c, float* r1, float* g1, float* b1)
{
    float r, g, b;
    switch (c) {
    case (WHITE):
        r = 1, g = 1, b = 1;
        break;
    case (BLUE):
        r = 0, g = 0, b = 1;
        break;
    case (ORANGE):
        r = 1, g = 0.3, b = 0.1;
        break;
    case (BLACK):
        r=0, g=0, b=0;
        break;
    default:
        r = 1, g = 0.7, b =0.7 ;
        break;
    }
    *r1 = r, *g1 = g, *b1 = b;
}

Color getColor(Object* o)
{
    float col[3];
    col[0] = o->color[0];
    col[1] = o->color[1];
    col[2] = o->color[2];

    if (isequal(col[0], 1) && isequal(col[1], 1) && isequal(col[2], 1)) {
        return WHITE;
    } else if (isequal(col[0], 0) && isequal(col[1], 0) && isequal(col[2], 1)) {
        return BLUE;
    } else if (isequal(col[0], 1) && isequal(col[1], 0.3) && isequal(col[2], 0.1)) {
        return ORANGE;
    } else if (isequal(col[0], 0) && isequal(col[1], 0) && isequal(col[2], 0)){
        return BLACK;
    }

    return OTHER;
}

// float comparison
static const float eps = 0.01;
int isequal(float a, float b)
{
    if (fabsf(a - b) < eps)
        return 1;
    return 0;
}

#define MAX_LINE 100
// saves map to file
void saveMap()
{
    printf("Please enter the name of the new map:\n");

	char path[300];
	char name[200];
	scanf("%s", name);
	printf("Entered name: %s\n", name);
	path[0] = '\0';
#ifdef _WIN32
	strcat(path, "../collision-detection/maps/");
#elif __linux__
	strcat(path, "./src/collision-detection/maps/");
#endif
    strcat(path, name);
    FILE* f = fopen(path, "w");
    if (f == NULL) {
        printf("Error while trying to make a new file.\n");
        exit(EXIT_FAILURE);
    }
    ObjectNode* l;
    float currSizex=0, currSizey=0, currSizez=0;
    int i;
    for (l=Blocks, i=0; l != NULL; l = l->next, i++){
        Object block=*(l->o);
        if (currSizex != block.length || currSizey != block.height || currSizez != block.width){
            currSizex = block.length;
            currSizey = block.height;
            currSizez = block.width;
            fprintf(f, "s %.2f %.2f %.2f\n", currSizex, currSizey, currSizez);
        }
        fprintf(f, "c %.3f %.3f %.3f\n", block.posx / scale, block.posy / scale,
         block.posz / scale);
    }
    fprintf(f, "Numer of blocks in map: %d\n", i);
    fclose(f);
}

static int isDefaultMap=1;
// reads map file with the following format:
// line begins with 's': values are width, height and depth of cubes
// line begins with 'c': values are coordinates of objects
// all other lines are ignored
void loadMap(int defaultMap)
{
    char path[300];
    char name[200];
    path[0]='\0';
#ifdef _WIN32
	strcat(path, "../collision-detection/maps/");
#elif __linux__
	strcat(path, "./src/collision-detection/maps/");
#endif
    FILE* f;
    isDefaultMap = defaultMap;
    if (defaultMap) {
        strcat(path, DEFAULT_MAP);
        f = fopen(path, "r");
    } else {
        printf("Which map to load?\n");
        scanf("%s", name);
		// buling path to new file
        strcat(path, name);
        f = fopen(path, "r");
    }
    if (f == NULL) {
        printf("Error while trying to open .map file.\n");
        exit(EXIT_FAILURE);
    }
    freeList(&Blocks);
    char line[MAX_LINE];
    int count, i=0;
    float x, y, z;
    float currSizex, currSizey, currSizez;
    while (!feof(f)) {
        fgets(line, MAX_LINE, f);
        i++;
        if (line[0] == 's') {
            count = sscanf(&line[1], "%f %f %f", &currSizex, &currSizey, &currSizez);
            if (count != 3) {
                printf("los .map fajl\n");
                exit(EXIT_FAILURE);
            }
            setSizes(currSizex, currSizey, currSizez);
        } else if (line[0] == 'c') {
            count = sscanf(&line[1], "%f %f %f", &x, &y, &z);
            if (count != 3) {
                printf("los .map fajl, ucitano %d br. linija br %d:\n", count, i);
                printf("%s\n",line);
                exit(EXIT_FAILURE);
            }
            addBlocks(x, x, y, y, z, z);
        }
    }
    fclose(f);
    if(!isDefaultMap){
        resetGame();
    }
}

// used to reset and initialize game
void resetGame(void)
{
    resetBullets();
    if (isDefaultMap){
        initBlocks();
    }
    initMaterial();
    initLights();
    state = stateInit;
    player = playerInit;
    viewAzimuth.curr = 270;
    viewElevation.curr = 0;
    eyex = 0, eyey = 1, eyez = 2;
    lookAtx = 0, lookAty = 0, lookAtz = 0;
    int i;
    for (i = 0; i < MAX_LIGHTS; i++)
        lightOn[i] = 0;
}
#pragma warning(pop)
