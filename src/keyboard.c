#include "keyboard.h"

#define ESC 27
static int KEY_W = 0;
static int KEY_S = 0;
static int KEY_A = 0;
static int KEY_D = 0;
static int KEY_Q = 0;
static int KEY_SPACE = 0;
static int fullScreen = 0;
float initWindowHeight = 800;
static float viewAzimuthdt = 5, viewElevationdt = 3;
static float mouseSensitivity=0.01;
float aspectRatio = 16 / 9.0;

void onMouseButton(int button, int pressed, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON) {

        if (pressed == GLUT_DOWN) {
            firePaint();
        }
    }
    if (button == GLUT_RIGHT_BUTTON) {
        if (pressed == GLUT_DOWN) {
            fireBlackPaint();
        }
    }
}

static float prevMouseX = 500;
static float prevMouseY = 500;
/*nakon poziva funkcije glutWarpPointer postavi se kursor u centar.
Zbog toga glutmainloop opet pozove onMouseLook i tako u krug.
Da bi se izignorisalo dejstvo tog poziva prekida se izvrsavanje funkcije
ako je kursor u centru*/
/*indikator da li prozor igre ne dopusta kursoru misa da ga napusti. overly attached*/
static int releaseMouse = 0;
void onMouseLook(int x, int y)
{
    int width = glutGet(GLUT_WINDOW_WIDTH);
    int height = glutGet(GLUT_WINDOW_HEIGHT);
    if (x == width / 2 && y == height / 2)
        return;
    float deltaX;
    float deltaY;
    if (releaseMouse) {
        /*pomeraj u odnosu na proslu poziciju*/
        deltaX = x - prevMouseX;
        deltaY = y - prevMouseY;
    } else {
        /*pomeraj u odnosu na centar*/
        deltaX = x - width / 2;
        deltaY = y - height / 2;
        glutWarpPointer(width / 2, height / 2);
    }
    /*blokira se pomeranje kamere na pocetku igre*/
    if (state.newGame) return;
    prevMouseX = x;
    prevMouseY = y;
    viewAzimuth.curr += deltaX * viewAzimuthdt * mouseSensitivity;
    viewElevation.curr -= deltaY * viewElevationdt * mouseSensitivity;
    /*proveri da l su azimut i elevacija 0 do 360 i -max do max*/
    if (viewAzimuth.curr >= 360) {
        viewAzimuth.curr -= 360;
        viewAzimuth.goal -= 360;
    } else if (viewAzimuth.curr < 0) {
        viewAzimuth.curr += 360;
    }
    if (viewElevation.curr > MAX_ELEVATION)
        viewElevation.curr = MAX_ELEVATION;
    else if (viewElevation.curr < -MAX_ELEVATION)
        viewElevation.curr = -MAX_ELEVATION;
}

/*isto kao i onMouseLook, sem sto se poziva umesto nje
kada je pritisnuto dugme misa*/
void onMousePressedLook(int x, int y)
{
    onMouseLook(x, y);
}

static const float moveSpeed=0.05;
#define NORM_SPEED 0.1
void onKeyHold()
{
    /*ako ide brzo a nije u stanju goFast, tj nije na narandzastom*/
    if (fabsf(player.vz.curr > NORM_SPEED) && !state.goFast)
        return;
    float bonus = (state.goFast) ? NORM_SPEED : 0;
    if (KEY_W)
        player.vz.goal = moveSpeed + bonus;
    if (KEY_S)
        player.vz.goal = -(moveSpeed + bonus);
    if (KEY_A)
        player.vx.goal = -(moveSpeed + bonus);
    if (KEY_D)
        player.vx.goal = moveSpeed + bonus;
    if (state.flying && KEY_SPACE)
        jump();
    if (state.flying && KEY_Q)
        flyDown();
}

/*ne treba mi vise al nek stoji za ukras*/
void onSpecialInputUp(int key, int x, int y)
{
    if (state.newGame){
        return;
    }
    switch (key) {
        default:
            break;
    }
}

void onSpecialInput(int key, int x, int y)
{
    if (state.newGame && key != GLUT_KEY_F10){
        return;
    }

    switch (key) {
        case (GLUT_KEY_F1):
            saveMap();
            break;
        case (GLUT_KEY_F2):
            loadMap(0);
            break;
        case (GLUT_KEY_F5):
            resetGame();
            break;
        case (GLUT_KEY_F10):
            fullScreen = 1;
            glutFullScreen();
            break;
        case (GLUT_KEY_RIGHT):
            viewAzimuth.curr += viewAzimuthdt;
            break;
        case (GLUT_KEY_LEFT):
            viewAzimuth.curr -= viewAzimuthdt;
            break;
        case (GLUT_KEY_UP):
            viewElevation.curr += viewElevationdt;
            if (viewElevation.curr > MAX_ELEVATION)
                viewElevation.curr = MAX_ELEVATION;
            break;
        case (GLUT_KEY_DOWN):
            viewElevation.curr -= viewElevationdt;
            if (viewElevation.curr < -MAX_ELEVATION)
                viewElevation.curr = -MAX_ELEVATION;
            break;
    }
}

void onKeyboardUp(unsigned char key, int x, int y)
{
    switch (key) {
        case ('h'):
            countList(Blocks);
            break;
        case ('a'):
        case ('A'):
            KEY_A = 0;
            player.vx.goal = 0;
            break;
        case ('d'):
        case ('D'):
            KEY_D = 0;
            player.vx.goal = 0;
            break;
        case ('w'):
        case ('W'):
            KEY_W = 0;
            player.vz.goal = 0;
            break;
        case ('s'):
        case ('S'):
            KEY_S = 0;
            player.vz.goal = 0;
            break;
        case (' '):
            KEY_SPACE = 0;
            break;
        case ('q'):
        case ('Q'):
            KEY_Q=0;
            break;
    }

}

void onKeyboard(unsigned char key, int x, int y)
{
    /*na samom pocetku blokira se input dok igrac ne ispali boju*/
    if (state.newGame && key != 'f' && key != 'F' && key != ESC){
        return;
    }
    switch (key) {
        case ('f'):
        case ('F'):
            firePaint();
            break;
        case (' '):
            KEY_SPACE = 1;
            jump();
            break;
        case ('s'):
        case ('S'):
            KEY_S = 1;
            player.vz.goal = -0.05 - state.goFast * 0.15;
            break;
        case ('w'):
        case ('W'):
            KEY_W = 1;
            player.vz.goal = 0.05 + state.goFast * 0.15;
            break;
        case ('a'):
        case ('A'):
            KEY_A = 1;
            player.vx.goal = -0.05 - state.goFast * 0.15;
            break;
        case ('d'):
        case ('D'):
            KEY_D = 1;
            player.vx.goal = 0.05 + state.goFast * 0.15;
            break;
        case ('1'):
            state.fireColor = WHITE;
            break;
        case ('2'):
            state.fireColor = BLUE;
            break;
        case ('3'):
            state.fireColor = ORANGE;
            break;
        case ('4'):
            printf("%f, %f, %f\n", player.posx, player.posy, player.posz);
            showFps = !showFps;
            break;
        case ('5'):
            toggleBuildMode();
            break;
        case ('p'):
        case ('P'):
            if (!releaseMouse) {
                glutSetCursor(GLUT_CURSOR_LEFT_ARROW);
                releaseMouse = 1;
            } else {
                glutSetCursor(GLUT_CURSOR_NONE);
                releaseMouse = 0;
            }
            break;
        case ('q'):
        case ('Q'):
            KEY_Q=1;
            flyDown();
            break;
        case ('+'):
            mouseSensitivity *= 1.2;
            break;
        case ('-'):
            mouseSensitivity *= 0.8;
            break;
        case (ESC):
            exit(0);
            break;
    }
}

void onReshape(int width, int height)
{
    if (!fullScreen) {
        width = aspectRatio * height;
        glutReshapeWindow(width, height);
    }
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, (float) width / height, 0.1, 1000.0);
}
