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

#include "Comm.h"

int dt;
static int oldDisplayTime;
static void onTimerUpdate(int id);
static void onDisplay(void);
static void updateDeltaTime(void);
static float fps(int print);

int main(int argc, char** argv)
{
	duck* ducky = new_duck(5);
	printf("DA LI RADI: %d\n", test());
	duck_quack(ducky, 1.f);
	delete_duck(ducky);

	auto d = Duck(33);
	d.quack(777);


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

//https://www.opengl.org/archives/resources/features/KilgardTechniques/oglpitfall/
// pokusaj da postavim poziciju za pisanje
void glWindowPos4fMESAemulate(float x, float y, float z, float w) {
   float fx, fy;

  /* Push current matrix mode and viewport attributes. */
  glPushAttrib(GL_TRANSFORM_BIT | GL_VIEWPORT_BIT);

    /* Setup projection parameters. */
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
      glLoadIdentity();
      glMatrixMode(GL_MODELVIEW);
      glPushMatrix();
        glLoadIdentity();
        glDepthRange(z, z);
        glViewport((int) x - 1, (int) y - 1, 2, 2);
        /* Set the raster (window) position. */
        fx = x - (int) x;
        fy = y - (int) y;
        glRasterPos4f(fx, fy, 0.0, w);
      /* Restore matrices, viewport and matrix mode. */
      glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
  glPopAttrib();
}

// wrapepr
void glWindowPos2fMESAemulate(float x, float y)
{
  glWindowPos4fMESAemulate(x, y, 0, 1);
}

void textToScreenPos(float x, float y, const char *string)
{
    glWindowPos2fMESAemulate(x, y);
    int len = (int) strlen(string);
    glColor3f(1.0f, 0.2f, 0.5f);
    //loop to display character by character
    int i;
    for (i = 0; i < len; i++)
    {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, string[i]);
    }
};

void onDisplay(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    float fpsCount = fps(showFps);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    positionCam();

    lightSetup();

    drawMap();
    drawBullets();

    // begin
    glDisable(GL_LIGHTING);

    textToScreenPos(0, 0, "zero zero");
    textToScreenPos(0, 500, "zero 500");
    textToScreenPos(500, 0, "500 zero");
    textToScreenPos(500, 500, "500 500");

    char fPointer[55] = "fps: ";
    sprintf(fPointer, "%f", fpsCount);

    textToScreenPos(100, 0, fPointer);

    glEnable(GL_LIGHTING);
    // end

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
