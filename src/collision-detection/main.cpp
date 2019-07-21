#include <GL/glut.h>
#include <cstdio>
#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>
#include "player.h"
#include "keyboard.h"
#include "miscfunc.h"
#include "collision.h"
#include "draw.h"

#include "Controller.h"

int dt;
int oldDisplayTime;
void onTimerUpdate(int id);
void onDisplay(void);
void updateDeltaTime(void);
float fps(int print);
void drawAllText(float fpsCount);
void updateCollisions();
void drawCollisions();
void initCollision();

void initCollision()
{
    srand((unsigned)time(NULL));
	auto& control = Controller::get();
	control.setSpeed(0.01f);
	control.doResolution = true;
	control.setCuboidSize(.02f);
}

int main(int argc, char** argv)
{
    initCollision();
    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH /*| GLUT_MULTISAMPLE*/);

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

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT7);
	
	// gl_normalize fixes lighting of scaled objects
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
	float delta = dt / (float)UPDATE_INTERVAL;
	control.setDelta(delta);

	auto& mover = control.mover;
	auto& cuboids = control.cuboids;

	// unmark collisions
	for (auto& cub : cuboids) {
		cub.setColliding(false);
	}
	
    mover.moveItems(cuboids, delta);

	control.resetAlgorithm();
	auto& collisionChecker = control.collisionChecker;

	auto &pairs = control.pairs;
	pairs.resize(0);
	collisionChecker->markCollisions(cuboids, pairs);
	if (control.doResolution) {
		for (auto &pair : pairs) {
			pair.first->response(*pair.second);
		}
	}

	control.numPairs = pairs.size();
	control.numInCollision = std::count_if(cuboids.cbegin(), cuboids.cend(),
		[](const auto &c) {return c.hasCollision();});
	mover.ensureWithinBounds(cuboids, 0);
}

void drawAllText(float fpsCount)
{
	auto& control = Controller::get();
	glDisable(GL_LIGHTING);

	char fPointer[100];
	sprintf(fPointer, "FPS: %.2f", fpsCount);

	drawTextAt(100, 5, fPointer);
	auto& collisionChecker = control.collisionChecker;
    std::string outputText = collisionChecker->getInfo();
	drawTextAt(250, 5, outputText.c_str());

	outputText = control.getInfo();
	drawTextAt(100, 55, outputText.c_str());

	if (control.showControls) {
		const int decrementHeight = 23;
		int height = glutGet(GLUT_WINDOW_HEIGHT);
		
		for (const auto& line : control.getControlsInfo()) {
			height -= decrementHeight;
			drawTextAt(10, height, line.c_str());
		}
	}
	
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
	drawBox(bounds, 0, 0, 0, 0.1f);
	//control.collisionChecker->drawSelf(drawBox);
}

void onDisplay()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    float fpsCount = fps(showFps);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
	
	if (Controller::get().shouldDraw) {
		positionCam();

		lightSetup();

		drawMap();
		drawBullets();

		drawCollisions();
	}

	drawAllText(fpsCount);

    glutSwapBuffers();
}

// updates position of all objects and handles all events
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
    glutTimerFunc(Controller::get().isMeasuring() ? 1 : UPDATE_TIMER_INTERVAL, onTimerUpdate, TIMER_UPDATE_ID);
}

// calculation of delta time betweeen two calls to onTimerUpdate function
// delta time is important because on slower configurations timer is called
// less frequently. Because of it all movements are done proportionally to dt
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

	if (dt > DT_MAX && !Controller::get().isMeasuring()) {
		dt = DT_MAX;
	}
}

// vars for counting frames per second
int newDisplayTime;
int SECOND = 1000;

float fps(int print)
{
    newDisplayTime = glutGet(GLUT_ELAPSED_TIME);
	// elapsed time between two frames
    int diff = newDisplayTime - oldDisplayTime;
    oldDisplayTime = newDisplayTime;
	// max and min time between two frames
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
