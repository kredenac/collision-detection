#include "keyboard.h"
#include "Controller.h"
#pragma warning(push)
#pragma warning(disable: 4305)
#pragma warning(disable: 4244)
#include <cctype>

const int ESC = 27;
static int KEY_W = 0;
static int KEY_S = 0;
static int KEY_A = 0;
static int KEY_D = 0;
static int KEY_Q = 0;
static int KEY_SPACE = 0;
static int fullScreen = 0;
static bool KEY_LESS = false;
static bool KEY_MORE = false;
static bool KEY_BIGGER = false;
static bool KEY_SMALLER = false;
static bool KEY_CONTAINER_BIGGER = false;
static bool KEY_CONTAINER_SMALLER = false;
int initWindowHeight = 800;
static float viewAzimuthdt = 5, viewElevationdt = 3;
static float mouseSensitivity = 0.01f;

float aspectRatio = 16 / 9.0f;

void onMouseButton(int button, int pressed, int x, int y)
{
	(void)x; (void)y;
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


// if mouse can leave the game window
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
		deltaX = x - prevMouseX;
		deltaY = y - prevMouseY;
	}
	else {
		deltaX = x - width / 2;
		deltaY = y - height / 2;
		glutWarpPointer(width / 2, height / 2);
	}
	prevMouseX = x;
	prevMouseY = y;
	viewAzimuth.curr += deltaX * viewAzimuthdt * mouseSensitivity;
	viewElevation.curr -= deltaY * viewElevationdt * mouseSensitivity;
	if (viewAzimuth.curr >= 360) {
		viewAzimuth.curr -= 360;
		viewAzimuth.goal -= 360;
	}
	else if (viewAzimuth.curr < 0) {
		viewAzimuth.curr += 360;
	}
	if (viewElevation.curr > MAX_ELEVATION)
		viewElevation.curr = MAX_ELEVATION;
	else if (viewElevation.curr < -MAX_ELEVATION)
		viewElevation.curr = -MAX_ELEVATION;
}

// mouse callback when a key is pressed
void onMousePressedLook(int x, int y)
{
	onMouseLook(x, y);
}

const float moveSpeed = 0.05f;
const float NORM_SPEED = 0.1f;
void onKeyHold()
{
	auto& ctl = Controller::get();
	if (KEY_LESS) {
		float percentLess = ctl.cuboids.size() * 0.01f;
		size_t less = (size_t)ceilf(percentLess * ctl.delta());
		ctl.lessElements(less);
	}
	if (KEY_MORE) { 
		float percentMore = fmaxf(ctl.cuboids.size() * 0.01f, 1.f);
		size_t more = (size_t)ceilf(percentMore * ctl.delta());
		ctl.moreElements(more);
	}
	if (KEY_SMALLER || KEY_BIGGER) {
		float currSize = ctl.cuboidSize();
		float diff = fmaxf(currSize * 0.01f, 1e-5) * ctl.delta();
		diff *= KEY_BIGGER ? 1.f : -1.f;
		float newSize = currSize + diff;
		ctl.setCuboidSize(newSize);
	}
	if (KEY_CONTAINER_BIGGER || KEY_CONTAINER_SMALLER) {
		ctl.changeContainerSize(KEY_CONTAINER_BIGGER);
	}
	// if going fast but not in gofast state (not on orange block)
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

void onSpecialInputUp(int key, int x, int y)
{
	(void)key; (void)x; (void)y;
}

void onSpecialInput(int key, int x, int y)
{
	(void)x; (void)y;
	switch (key) {
	case (GLUT_KEY_F1):
		saveMap();
		break;
	case (GLUT_KEY_F2):
		loadMap(0);
		break;
	case (GLUT_KEY_F5):
		Controller::get().resetToDefault();
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
	(void)x; (void)y;
	switch (std::tolower(key)) {
	case ('r'):
		KEY_CONTAINER_SMALLER = false;
		break;
	case ('t'):
		KEY_CONTAINER_BIGGER = false;
		break;
	case ('v'):
		KEY_SMALLER = false;
		break;
	case ('b'):
		KEY_BIGGER = false;
		break;
	case ('n'):
		KEY_LESS = false;
		break;
	case ('m'):
		KEY_MORE = false;
		break;
	case ('h'):
		countList(Blocks);
		break;
	case ('a'):
		KEY_A = 0;
		player.vx.goal = 0;
		break;
	case ('d'):
		KEY_D = 0;
		player.vx.goal = 0;
		break;
	case ('w'):
		KEY_W = 0;
		player.vz.goal = 0;
		break;
	case ('s'):
		KEY_S = 0;
		player.vz.goal = 0;
		break;
	case (' '):
		KEY_SPACE = 0;
		break;
	case ('q'):
		KEY_Q = 0;
		break;
	}

}

void onKeyboard(unsigned char key, int x, int y)
{
	(void)x; (void)y;
	auto& control = Controller::get();
	switch (std::tolower(key)) {
	case ('r'):
		KEY_CONTAINER_SMALLER = true;
		break;
	case ('t'):
		KEY_CONTAINER_BIGGER = true;
		break;
	case ('z'):
		control.decreaseSpeed();
		break;
	case ('x'):
		control.increaseSpeed();
		break;
	case ('c'):
		control.doResolution = !control.doResolution;
		break;
	case ('7'):
		control.changeOcteeMaxDepth(-1);
		break;
	case ('8'):
		control.changeOcteeMaxDepth(1);
		break;
	case ('9'):
		control.changeOctreeMaxElements(-1);
		break;
	case ('0'):
		control.changeOctreeMaxElements(1);
		break;
	case ('i'):
		control.joltTowards(player.posx, player.posy, player.posz);
		break;
	case ('o'):
		control.shouldDraw = !control.shouldDraw;
		break;
	case ('v'):
		KEY_SMALLER = true;
		break;
	case ('b'):
		KEY_BIGGER = true;
		break;
	case ('n'):
		KEY_LESS = true;
		break;
	case ('m'):
		KEY_MORE = true;
		break;
	case ('j'):
		control.prevAlgorithm();
		break;
	case ('k'):
		control.nextAlgorithm();
		break;
	case ('f'):
		firePaint();
		break;
	case ('g'):
		control.startMeasurement();
		break;
	case ('h'):
		control.endMeasurement();
		break;
	case (' '):
		KEY_SPACE = 1;
		jump();
		break;
	case ('s'):
		KEY_S = 1;
		player.vz.goal = -0.05 - state.goFast * 0.15;
		break;
	case ('w'):
		KEY_W = 1;
		player.vz.goal = 0.05 + state.goFast * 0.15;
		break;
	case ('a'):
		KEY_A = 1;
		player.vx.goal = -0.05 - state.goFast * 0.15;
		break;
	case ('d'):
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
		if (!releaseMouse) {
			glutSetCursor(GLUT_CURSOR_LEFT_ARROW);
			releaseMouse = 1;
		}
		else {
			glutSetCursor(GLUT_CURSOR_NONE);
			releaseMouse = 0;
		}
		break;
	case ('q'):
		KEY_Q = 1;
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
	// glutReshapeWindow would again call this callback, 
	// so this prevents it
	static bool preventMultiple = false;
	if (!fullScreen) {
		if (!preventMultiple) {
			width = aspectRatio * height;
			glutReshapeWindow(width, height);
		}
		preventMultiple = !preventMultiple;
	}
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, (float)width / height, 0.1, 1000.0);
}
#pragma warning(pop)
