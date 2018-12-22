#include "draw.h"

int lightOn[MAX_LIGHTS];
int lights[] = {
    GL_LIGHT0,
    GL_LIGHT1,
    GL_LIGHT2,
    GL_LIGHT3,
    GL_LIGHT4,
    GL_LIGHT5,
    GL_LIGHT6,
};
float lightPos[MAX_LIGHTS][4];

/*ukljucuju se i pozicioniraju aktivna svetla*/
void lightSetup()
{
    int i;
    for (i = 0; i < MAX_LIGHTS; i++) {
        if (lightOn[i]) {
            glEnable(lights[i]);
            glLightfv(lights[i], GL_POSITION, lightPos[i]);
        } else {
            glDisable(lights[i]);
        }
    }
	const float globalLightPos[] = { 0., 1.f, -1.9f, 1};
	glLightfv(GL_LIGHT7, GL_POSITION, globalLightPos);
}

/*inicijalna svojstva svetlosti*/
void initLights()
{
    float lightAmbient[] = {0.3f, 0.3f, 0.3f, 1.f};
    float lightDiffuse[] = {0.7f, 0.7f, 0.7f, 1.f};
    float lightSpecular[] = {0.2f, 0.2f, 0.2f, 0.0f};
    float lightDirection[] = {0.f, 0.f, 0.f};
    int i;
    for (i = 0; i < MAX_LIGHTS; i++) {
        lightOn[i]=0;
        glLightfv(lights[i], GL_SPOT_DIRECTION, lightDirection);
        glLightfv(lights[i], GL_AMBIENT, lightAmbient);
        glLightfv(lights[i], GL_DIFFUSE, lightDiffuse);
        glLightfv(lights[i], GL_SPECULAR, lightSpecular);
        /*opadanje intenziteta svetlosti u zavisnosti od razdaljine*/
        glLightf(lights[i], GL_CONSTANT_ATTENUATION, 0.f);
        glLightf(lights[i], GL_LINEAR_ATTENUATION, 0.3f);
        glLightf(lights[i], GL_QUADRATIC_ATTENUATION, 0.01f);
        glLightf(lights[i], GL_SPOT_CUTOFF, 180.0f);
    }
	// global light
	float globalAmbient[] = { 0.3f, 0.3f, 0.3f, 1.f };
	float globalDiffuse[] = { 0.7f, 0.7f, 0.7f, 1.f };
	float globalSpecular[] = { 0.2f, 0.2f, 0.2f, 0.0f };
	float globalDirection[] = { 0.f, 0.f, 0.f };
	glLightfv(GL_LIGHT7, GL_SPOT_DIRECTION, globalDirection);
	glLightfv(GL_LIGHT7, GL_AMBIENT, globalAmbient);
	glLightfv(GL_LIGHT7, GL_DIFFUSE, globalDiffuse);
	glLightfv(GL_LIGHT7, GL_SPECULAR, globalSpecular);
	glLightf(GL_LIGHT7, GL_CONSTANT_ATTENUATION, 0.f);
	glLightf(GL_LIGHT7, GL_LINEAR_ATTENUATION, 0.3f);
	glLightf(GL_LIGHT7, GL_QUADRATIC_ATTENUATION, 0.01f);
	glLightf(GL_LIGHT7, GL_SPOT_CUTOFF, 360.0f);
}

/*postavlja poziciju n-tog GL_LIGHT*/
void setLightPos(int n, float x, float y, float z)
{
    lightPos[n][0] = x;
    lightPos[n][1] = y;
    lightPos[n][2] = z;
    lightPos[n][3] = 1;
}

/*inicijalna svojstva materijala*/
void initMaterial()
{
    float ambient_coeffs[] = {0.f, 0.f, 0.f, 1.f };
    float diffuse_coeffs[] = {0.4f, 0.4f, 0.4f, 1.f };
    float specular_coeffs[] = {0.f, 0.f, 0.f, 1.f };
    float shininess = 0;

    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient_coeffs);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse_coeffs);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular_coeffs);
    glMaterialf(GL_FRONT, GL_SHININESS, shininess);
}

/*na osnovu boje objekta postavlja materijale pred iscrtavanje*/
void drawWithColor(Object* o)
{
	GLfloat diffuseCoeffs[] = { o->color[0], o->color[1], o->color[2], 1 };
	/*potamnjuje ambinet coeffs*/
	float s = 1.f;
	GLfloat ambientCoeffs[] = { o->color[0] * s, o->color[1] * s, o->color[2] * s, 1 };
	const float emissionCoeffs[] = { 0.5f, 0.5f, 0.5f, 1.f };
	const float emissionCoeffs2[] = { 0.f, 0.f, 0.f, 0.f };
	if (getColor(o) == WHITE) {
		glMaterialfv(GL_FRONT, GL_EMISSION, emissionCoeffs);
	}
	else {
		glMaterialfv(GL_FRONT, GL_EMISSION, emissionCoeffs2);
	}
	glMaterialfv(GL_FRONT, GL_AMBIENT, ambientCoeffs);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuseCoeffs);
}

void drawWithColor(float r, float g, float b, float a)
{
	GLfloat diffuseCoeffs[] = { r, g, b, a};
	/*potamnjuje ambinet coeffs*/
	float s = 0.6f;
	GLfloat ambientCoeffs[] = { r * s, g * s, b * s, a };
	const float emissionCoeffs2[] = { 0.f, 0.f, 0.f, 0.f };
	glMaterialfv(GL_FRONT, GL_EMISSION, emissionCoeffs2);
	glMaterialfv(GL_FRONT, GL_AMBIENT, ambientCoeffs);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuseCoeffs);

}


//https://www.opengl.org/archives/resources/features/KilgardTechniques/oglpitfall/
// sets up gl for writing text
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
	glViewport((int)x - 1, (int)y - 1, 2, 2);
	/* Set the raster (window) position. */
	fx = x - (int)x;
	fy = y - (int)y;
	glRasterPos4f(fx, fy, 0.0, w);
	/* Restore matrices, viewport and matrix mode. */
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glPopAttrib();
}

// wrapper
void glWindowPos2fMESAemulate(float x, float y)
{
	glWindowPos4fMESAemulate(x, y, 0, 1);
}

void drawTextAt(float x, float y, const char *string)
{
	glWindowPos2fMESAemulate(x, y);
	int len = (int)strlen(string);
	glColor3f(1.0f, 0.2f, 0.5f);
	//loop to display character by character
	int i;
	for (i = 0; i < len; i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, string[i]);
	}
};

/*postavlja kameru na poziciju glave igraca i usmerava pogled*/
void positionCam()
{
    eyex = player.posx;
    eyey = player.posy + playerHeadHeight;
    eyez = player.posz;

    /*azuriranje lookat na osnovu levo-desno rotacije. A3*/
    float cosy = cosf((float)M_PI * viewElevation.curr / 180);
    lookAtx = cosf((float)M_PI * viewAzimuth.curr / 180) * cosy;
    lookAtz = sinf((float)M_PI * viewAzimuth.curr / 180) * cosy;
    /*azuriranje lookat na osnovu gore-dole rotacije*/
    lookAty = sinf((float)M_PI * viewElevation.curr / 180);
    /*usmerava se pogled relativno od pozicije kamere*/
    lookAtx = eyex + lookAtx;
    lookAtz = eyez + lookAtz;
    lookAty = eyey + lookAty;

    gluLookAt(eyex, eyey, eyez,
        lookAtx, lookAty, lookAtz,
        upx, upy, upz);
}

/*iscrtava sve blokove koji cine mapu*/
void drawMap()
{
    ObjectNode* l;
    for (l = Blocks; l != NULL; l = l->next){
        drawCube(l->o);
    }
}

/*boji sve kocke random bojama. interval je broj poziva funkciji potreban da se promene boje*/
void psychedelic(int interval)
{
    static int c = 0;
    c++;
    if (c < interval)
        return;
    c = 0;
    int i;
    /*poput gradijenta-pocetni se boje pretezno start bojom, a poslednji end bojom*/
    float start[] = {
        rand() / (float)(RAND_MAX),
        rand() / (float)(RAND_MAX),
        rand() / (float)(RAND_MAX)
    };
    float end[] = {
        rand() / (float)(RAND_MAX),
        rand() / (float)(RAND_MAX),
        rand() / (float)(RAND_MAX)
    };
    /*ipak, nece biti cist gradijent nego odstupanja.
    nice je deo boje od gradijenta, a displace deo od rand odstupanja*/
    float nice = 0.8f;
    float displace = 1.f - nice;
    float r, g, b;
    /*s je deo start boje, a e je deo end boje*/
    ObjectNode* l;
    i=0;
    for (l=Blocks; l!=NULL; l=l->next, i++){

        float e = (float) i / NUM_OF_BLOCKS;
        float s = 1 - e;
        if (getColor(l->o) != WHITE) {
            r = start[0] * s + end[0] * e;
            g = start[1] * s + end[1] * e;
            b = start[2] * s + end[2] * e;
            r = r * nice + displace * rand() / (float)(RAND_MAX);
            g = g * nice + displace * rand() / (float)(RAND_MAX);
            b = b * nice + displace * rand() / (float)(RAND_MAX);
            setColor(l->o, r, g, b);
        }
    }
}

/*iscrtava sve aktivne metke*/
void drawBullets() {
    float x, y, z;
    int i;
    for (i = 0; i < MAX_BULLETS; i++) {
        if (bulletsActive[i]) {
            x = bullets[i].posx, y = bullets[i].posy, z = bullets[i].posz;
            glPushMatrix();
            glTranslatef(x, y, z);
            drawWithColor(&bullets[i]);
            glutSolidSphere(bullets[i].length / 2, 20, 10);
            glPopMatrix();
        }
    }
}

void drawCube(Object* o) 
{
    drawWithColor(o);
    glPushMatrix();
    glTranslatef(o->posx, o->posy, o->posz);
    glScalef(o->length, o->height, o->width);
    glutSolidCube(1);
    glPopMatrix();
}

void drawBox(const Box & c, float r, float g, float b, float alpha)
{
	drawWithColor(r, g, b, alpha);
	glPushMatrix();
	glTranslatef(c.pos.x, c.pos.y, c.pos.z);
	glScalef(c.size.x, c.size.y, c.size.z);
	glutSolidCube(1);
	glPopMatrix();
}

void drawCuboid(const Cuboid & c, float alpha) 
{
	if (c.hasCollision()) {
		drawBox(c, 1, 0, 0, alpha);
	}
	else {
		drawBox(c, 0, 1, 0, alpha);
	}
}