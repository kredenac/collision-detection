#include "player.h"

const Object playerInit = {
	0.f, 0.f, 4.f,
	{0.f, 0.f},
	{0.f, 0.f},
	{0.f, 0.f},
	0.4f, 1.4f, 0.4f,
	{0.f, 0.f, 0.f}
};

const Object bulletInit = {
	0.f, 0.f, 0.f,
	{0.f, 0.f},
	{0.f, 0.f},
	{0.f, 0.f},
	0.2f, 0.2f, 0.2f,
	{1.f, 1.f, 1.f}
};

State state;
const State stateInit = {
	0, WHITE, 0,
	0, 0, 1,
	0, 0
};

Object player;
extern int dt;

float GRAVITY = -1.f;
Val2f viewAzimuth;
Val2f viewElevation;
float eyex, eyey, eyez;
float lookAtx, lookAty, lookAtz;
float playerHeadHeight = 0.25f;
float JUMP_V = 0.1f;

float* moveRightCam(void);
float* moveForwardCam(int yZero);
int firstFreeLight(void);

// updating player position and speed
void movePlayer()
{
	float d = dt / (float)UPDATE_INTERVAL;
	onKeyHold();
	// only if the player is in the air is gravity needed
	GRAVITY = (state.jumping && !state.flying) ? -1.f : 0.f;
	float* r = moveRightCam();
	float* f = moveForwardCam(1);
	// gradual speed change
	float step = dt / (float)5000;
	player.vz.curr = approach(player.vz.goal, player.vz.curr, step);
	player.vx.curr = approach(player.vx.goal, player.vx.curr, step);
	player.vy.curr = approach(player.vy.goal, player.vy.curr, step * 10);
	// goal speed decreases with time if no additional force is applied
	player.vz.goal = approach(0, player.vz.goal, step / 10);
	player.vx.goal = approach(0, player.vx.goal, step / 10);
	player.vy.goal = approach(GRAVITY, player.vy.goal, step);
	// movements relative to camera
	player.posx += r[0] * player.vx.curr * d;
	player.posz += r[2] * player.vx.curr * d;
	player.posz += f[2] * player.vz.curr * d;
	player.posx += f[0] * player.vz.curr * d;
	player.posy += player.vy.curr * d;
}

// checks if player is in a spot to trigger an event
void checkEvents()
{
	if (player.posy >= 11.6 && player.posy <= 12 && player.posx > -0.8 &&
		player.posx < 0.8 && player.posz <= -17.2 && player.posz >= -18.8) {
		state.finishedGame = 1;
	}
	if (state.finishedGame) {
		psychedelic(600 / dt);
	}
}

// left-right movement relative to camera
static float* moveRightCam(void)
{
	static float v[3];
	float ax = lookAtx - eyex;
	float az = lookAtz - eyez;

	v[0] = -az;
	v[1] = 0;
	v[2] = ax;
	normalize3f(v, v + 1, v + 2);
	return v;
}

// forward-backward movement relative to camera
// setZero - should y coordinate be set to zero
static float* moveForwardCam(int setYZero)
{
	static float v[3];
	v[0] = lookAtx - eyex;
	v[1] = setYZero ? 0 : lookAty - eyey;
	v[2] = lookAtz - eyez;
	normalize3f(v, v + 1, v + 2);
	return v;
}

Object bullets[MAX_BULLETS];
int bulletsActive[MAX_BULLETS];

#define BULLET_SPEED 5
// firing bullet
void firePaint()
{
	int i = 0;
	float r, g, b;
	for (i = 0; i < MAX_BULLETS; i++) {
		if (!bulletsActive[i]) {

			bulletsActive[i] = 1;
			bullets[i] = bulletInit;
			bullets[i].posx = player.posx;
			bullets[i].posy = player.posy;
			bullets[i].posz = player.posz;
			set3fWithColor(state.fireColor, &r, &g, &b);
			setColor(&bullets[i], r, g, b);
			float* v = moveForwardCam(0);
			bullets[i].vx.curr = v[0] / BULLET_SPEED;
			bullets[i].vy.curr = v[1] / BULLET_SPEED;
			bullets[i].vz.curr = v[2] / BULLET_SPEED;
			break;
		}
	}

	state.newGame = 0;
}

// bullets[i].vs.goal is used as a timed counter for bullet lifespan
static const float MAX_BULLET_LIFE = 10000;
void moveBullets(void)
{
	float d = dt / (float)17;
	int i;
	for (i = 0; i < MAX_BULLETS; i++)
		if (bulletsActive[i]) {
			bullets[i].posx += bullets[i].vx.curr * d;
			bullets[i].posy += bullets[i].vy.curr * d;
			bullets[i].posz += bullets[i].vz.curr * d;
			bullets[i].vx.goal += dt;
			if (bullets[i].vx.goal > MAX_BULLET_LIFE)
				bulletsActive[i] = 0;
		}
}

// deactivates all bullets
void resetBullets()
{
	int i;
	for (i = 0; i < MAX_BULLETS; i++) {
		bulletsActive[i] = 0;
	}
}

static Object* lightBlock[MAX_LIGHTS];
static int lightAge[MAX_LIGHTS];
// changes properties and color of a block based on bullet color
void paintBlock(Object* block, Object* bullet)
{
	// if it's a white block then turn off it's light
	if (getColor(block) == WHITE) {
		int i;
		for (i = 0; i < MAX_LIGHTS; i++) {
			if (lightBlock[i] == block) {
				lightOn[i] = 0;
				lightAge[i] = 0;
			}
		}
	}
	// if bullet is white then add a light to the block
	if (getColor(bullet) == WHITE) {
		int n = firstFreeLight();
		lightBlock[n] = block;
		lightOn[n] = 1;
		// age is the time of turning on this light
		lightAge[n] = glutGet(GLUT_ELAPSED_TIME);
		setLightPos(n, bullet->posx, bullet->posy, bullet->posz);
		psychedelic(1);
	}

	setColor(block, bullet->color[0], bullet->color[1], bullet->color[2]);
}

// take first free light slot, or the one which is set the earliest 
int firstFreeLight(void)
{
	int i;
	int oldest = glutGet(GLUT_ELAPSED_TIME) + 1;
	int oldestIndex = 0;
	for (i = 0; i < MAX_LIGHTS; i++) {
		if (lightOn[i] == 0) {
			return i;
		}
		if (oldest > lightAge[i]) {
			oldest = lightAge[i];
			oldestIndex = i;
		}
	}

	setColor(lightBlock[oldestIndex], 0, 0, 0);
	return oldestIndex;
}

// build mode jump and flying
void jump(void)
{
	if (!state.jumping) {
		player.vy.goal = JUMP_V + state.bigJump * 0.1f;
		state.bigJump = 0;
		state.jumping = 1;
	}
	else if (state.buildMode) {
		state.flying = 1;
		player.vy.goal = JUMP_V + state.bigJump * 0.1f;
	}
}

void flyDown(void)
{
	if (state.buildMode && state.flying) {
		player.vy.goal = -(JUMP_V + state.bigJump * 0.1f);
	}
}

void fireBlackPaint(void)
{
	if (!state.buildMode)
		return;
	Color last = state.fireColor;
	state.fireColor = BLACK;
	firePaint();
	state.fireColor = last;
}

void toggleBuildMode(void)
{
	state.buildMode = !state.buildMode;
	printf("%s build mode.\n", state.buildMode ? "Turned on" : "Turned off");
	if (!state.buildMode) {
		state.flying = 0;
	}
}

// player reaction to blocks based on their color
static const float eps = 0.001f;
void playerOnBlockReact(Object* p)
{
	player.posy = p->posy + p->height / 2 + player.height / 2 - eps;
	state.jumping = 0;
	state.flying = 0;
	player.vy.curr = 0;

	Color c = getColor(p);
	switch (c) {
	case (BLUE):
		state.bigJump = 1;
		state.goFast = 0;
		break;
	case (ORANGE):
		state.goFast = 1;
		state.bigJump = 0;
		break;
	case (WHITE):
	case (BLACK):
	case (OTHER):
	default:
		state.bigJump = 0;
		state.goFast = 0;
		break;
	}
}
