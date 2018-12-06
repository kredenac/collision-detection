#include <GL/glut.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include "player.h"
#include "keyboard.h"
#include "miscfunc.h"
#include "collision.h"
#include "draw.h"
#include "blocks.h"
int dt;
static int oldDisplayTime;
static void onTimerUpdate(int id);
static void onDisplay(void);
static void updateDeltaTime(void);
static void fps(int print);

int main(int argc, char** argv)
{
    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

    glutInitWindowSize(aspectRatio * initWindowHeight, initWindowHeight);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("Paint the Way");
    glutDisplayFunc(onDisplay);
    glutReshapeFunc(onReshape);
    glutKeyboardFunc(onKeyboard);
    glutKeyboardUpFunc(onKeyboardUp);
    glutSpecialFunc(onSpecialInput);
    glutSpecialUpFunc(onSpecialInputUp);
    glutPassiveMotionFunc(onMouseLook);
    glutMotionFunc(onMousePressedLook);
    glutMouseFunc(onMouseButton);

    glEnable(GL_CULL_FACE);
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
    srand(time(NULL));
    glutTimerFunc(UPDATE_TIMER_INTERVAL, onTimerUpdate,TIMER_UPDATE_ID);
    glutMainLoop();
    return 0;
}

void onDisplay(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    fps(showFps);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    positionCam();

    lightSetup();

    drawMap();
    drawBullets();

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

    glutPostRedisplay();
    glutTimerFunc(UPDATE_TIMER_INTERVAL, onTimerUpdate, TIMER_UPDATE_ID);
}

/*racunanje dt-vremena izmedju 2 poziva onTimerUpdate funkcije*/
/*potrebno je znati delta time posto ce na sporijim racunarima
redje da se poziva Timer. Zbog toga svi pomeraji se vrse srazmerno sa dt*/
#define DT_MAX 100
static int newTime;
static int oldTime = 0;
static int timeSum = 0;

void updateDeltaTime(void)
{
    newTime=glutGet(GLUT_ELAPSED_TIME);
    dt = newTime - oldTime;
    oldTime = newTime;
    timeSum += dt;
    if (dt > DT_MAX)
        dt = DT_MAX;
}

/*racuna frames per second*/
static int newDisplayTime;
#define SECOND 1000

void fps(int print)
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
    frame++;
    if (print && timeSum >= SECOND){
        printf("fps:%.2f minTime=%d maxTime=%d\n",
            frame * SECOND / (float)timeSum, minTime, maxTime);
        timeSum = 0;
        frame = 0;
        maxTime = 0;
        minTime = SECOND;
    }
}
