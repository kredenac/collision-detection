#include <GL/glut.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include "player.h"
#include "keyboard.h"
#include "miscfunc.h"
#include "collision.h"
#include "draw.h"
#include "blocks.h"

#include "Cuboid.h"
#include "BasicCollision.h"
#include "Octree.h"
#include "Mover.h"
#include "Controller.h"

int dt;
static int oldDisplayTime;
static void onTimerUpdate(int id);
static void onDisplay(void);
static void updateDeltaTime(void);
static float fps(int print);
void drawAllText(float fpsCount);
void updateCollisions();
void drawCollisions();
void initCollision();

void initCollision()
{
    srand((unsigned)time(NULL));
	// delete above
	printf("hi\n");
	auto& control = Controller::get();
	control.moreElements(1000);
	printf("carry on\n");
}

int main(int argc, char** argv)
{
    initCollision();
    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

    glutInitWindowSize((int)(aspectRatio * initWindowHeight), initWindowHeight);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("Collision detection");
    glutDisplayFunc(onDisplay);
    glutReshapeFunc(onReshape);
    glutKeyboardFunc(onKeyboard);
    glutKeyboardUpFunc(onKeyboardUp);
    glutSpecialFunc(onSpecialInput);
    glutSpecialUpFunc(onSpecialInputUp);
    glutPassiveMotionFunc(onMouseLook);
    glutMotionFunc(onMousePressedLook);
    glutMouseFunc(onMouseButton);

	// to make transparency work
	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	// --
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT7);
	
    /*prilikom skaliranja se poremeti osvetljenje pa me gl_normalize spasi*/
    glEnable(GL_NORMALIZE);
    glutSetCursor(GLUT_CURSOR_NONE);
    glutIgnoreKeyRepeat(GL_TRUE);
    glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);
    glEnable(GL_SMOOTH);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
    glClearColor(0, 0, 0, 0);
    resetGame();
    oldDisplayTime=dt=glutGet(GLUT_ELAPSED_TIME);
    glutTimerFunc(UPDATE_TIMER_INTERVAL, onTimerUpdate, TIMER_UPDATE_ID);
    glutMainLoop();
    return 0;
}

void updateCollisions()
{
	auto& control = Controller::get();
	auto& mover = control.mover;
	auto& cuboids = control.cuboids;

	for (auto& cub : cuboids) {
		// unmark collisions
		cub.setColliding(false);
	}
	float delta = dt / (float)UPDATE_INTERVAL;
	mover.moveItems(cuboids, delta);

	auto bounds = mover.getBounds();
	control.resetAlgorithm();
	auto& collisionChecker = control.collisionChecker;
	collisionChecker->markCollisions(cuboids);
}

void drawAllText(float fpsCount)
{
	glDisable(GL_LIGHTING);

	char fPointer[55] = "fps: ";
	sprintf(fPointer, "%f", fpsCount);

	drawTextAt(100, 5, fPointer);
	auto& collisionChecker = Controller::get().collisionChecker;
    std::string outputText = collisionChecker->getInfo();
	drawTextAt(300, 5, outputText.c_str());
	
	glEnable(GL_LIGHTING);
}

void drawCollisions()
{
	auto& control = Controller::get();
	auto& mover = control.mover;
	for (auto& cub : control.cuboids) {
		drawCuboid(cub);
	}
	auto bounds = mover.getBounds();
	drawCuboid(bounds, 0.1f);
	//control.collisionChecker->drawSelf(drawBox);
}

void onDisplay()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    float fpsCount = fps(showFps);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    positionCam();

    lightSetup();

    drawMap();
    drawBullets();

	drawCollisions();

	drawAllText(fpsCount);

    glutSwapBuffers();
}

/*funkcija za azuriranje polozaja objekata i obradu svih dogadjaja*/
void onTimerUpdate(int id)
{
    if (TIMER_UPDATE_ID != id){
        return;
    }
    updateDeltaTime();
    movePlayer();
    playerCollision();
    bulletCollision();
    moveBullets();
    checkEvents();

	updateCollisions();

    glutPostRedisplay();
    glutTimerFunc(UPDATE_TIMER_INTERVAL, onTimerUpdate, TIMER_UPDATE_ID);
}

/*racunanje dt-vremena izmedju 2 poziva onTimerUpdate funkcije*/
/*potrebno je znati delta time posto ce na sporijim racunarima
redje da se poziva Timer. Zbog toga svi pomeraji se vrse srazmerno sa dt*/
int DT_MAX = 100;
int newTime;
int oldTime = 0;
int timeSum = 0;

void updateDeltaTime()
{
    newTime=glutGet(GLUT_ELAPSED_TIME);
    dt = newTime - oldTime;
    oldTime = newTime;
    timeSum += dt;
	if (dt > DT_MAX) {
		dt = DT_MAX;
	}
}

/*for frames per second*/
int newDisplayTime;
int SECOND = 1000;

float fps(int print)
{
    newDisplayTime = glutGet(GLUT_ELAPSED_TIME);
    /*proteklo vreme izmedju 2 iscrtavanja*/
    int diff = newDisplayTime - oldDisplayTime;
    oldDisplayTime = newDisplayTime;
    /*max i min vreme izmedju 2 frejma*/
    static int maxTime = 0, minTime = SECOND;
    maxTime = diff>maxTime ? diff : maxTime;
    minTime = diff<minTime ? diff : minTime;
    static int frame = 0;
    static float fps = 0;
    frame++;
    if (timeSum >= SECOND){
        fps = frame * SECOND / (float)timeSum;
        if (print){
            printf("fps:%.2f minTime=%d maxTime=%d\n", fps, minTime, maxTime);
        }
        timeSum = 0;
        frame = 0;
        maxTime = 0;
        minTime = SECOND;
    }
    return fps;
}
