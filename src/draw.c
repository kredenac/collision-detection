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
    GL_LIGHT7
};
float lightPos[MAX_LIGHTS][4];

/*ukljucuju se i pozicioniraju aktivna svetla*/
void lightSetup()
{
    glEnable(GL_LIGHTING);
    int i;
    for (i = 0; i < MAX_LIGHTS; i++) {
        if (lightOn[i]) {
            glEnable(lights[i]);
            glLightfv(lights[i], GL_POSITION, lightPos[i]);
        } else {
            glDisable(lights[i]);
        }
    }
}

/*inicijalna svojstva svetlosti*/
void initLights()
{
    float lightAmbient[] = {0.3, 0.3, 0.3, 1};
    float lightDiffuse[] = {0.7, 0.7, 0.7, 1};
    float lightSpecular[] = {0.2, 0.2, 0.2, 0.0};
    float lightDirection[] = {0, 0, 0};
    int i;
    for (i = 0; i < MAX_LIGHTS; i++) {
        lightOn[i]=0;
        glLightfv(lights[i], GL_SPOT_DIRECTION, lightDirection);
        glLightfv(lights[i], GL_AMBIENT, lightAmbient);
        glLightfv(lights[i], GL_DIFFUSE, lightDiffuse);
        glLightfv(lights[i], GL_SPECULAR, lightSpecular);
        /*opadanje intenziteta svetlosti u zavisnosti od razdaljine*/
        glLightf(lights[i], GL_CONSTANT_ATTENUATION, 0);
        glLightf(lights[i], GL_LINEAR_ATTENUATION, 0.3);
        glLightf(lights[i], GL_QUADRATIC_ATTENUATION, 0.01);
        glLightf(lights[i], GL_SPOT_CUTOFF, 180.0);
    }
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
    GLfloat ambient_coeffs[] = {0, 0, 0, 1};
    GLfloat diffuse_coeffs[] = {0.4, 0.4, 0.4, 1};
    GLfloat specular_coeffs[] = {0, 0, 0, 1};
    GLfloat shininess = 0;

    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient_coeffs);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse_coeffs);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular_coeffs);
    glMaterialf(GL_FRONT, GL_SHININESS, shininess);
}

/*na osnovu boje objekta postavlja materijale pred iscrtavanje*/
void drawWithColor(Object* o)
{
    GLfloat diffuseCoeffs[] = {o->color[0], o->color[1], o->color[2], 1};
    /*potamnjuje ambinet coeffs*/
    float s = 0; //0.3;
    GLfloat ambientCoeffs[] = {o->color[0] * s, o->color[1] * s, o->color[2] * s, 1};
    const float emissionCoeffs[] = {0.5, 0.5, 0.5, 1};
    const float emissionCoeffs2[] = {0, 0, 0, 0};
    if (getColor(o) == WHITE) {
        glMaterialfv(GL_FRONT, GL_EMISSION, emissionCoeffs);
    } else {
        glMaterialfv(GL_FRONT, GL_EMISSION, emissionCoeffs2);
    }
    glMaterialfv(GL_FRONT, GL_AMBIENT, ambientCoeffs);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuseCoeffs);
}

/*postavlja kameru na poziciju glave igraca i usmerava pogled*/
void positionCam(void)
{
    eyex = player.posx;
    eyey = player.posy + playerHeadHeight;
    eyez = player.posz;

    /*azuriranje lookat na osnovu levo-desno rotacije. A3*/
    float cosy = cos(M_PI * viewElevation.curr / 180);
    lookAtx = cos(M_PI * viewAzimuth.curr / 180) * cosy;
    lookAtz = sin(M_PI * viewAzimuth.curr / 180) * cosy;
    /*azuriranje lookat na osnovu gore-dole rotacije*/
    lookAty = sin(M_PI * viewElevation.curr / 180);
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
    float nice = 0.8;
    float displace = 1 - nice;
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
void drawBullets(void) {
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

void drawCube(Object* o) {
    drawWithColor(o);
    glPushMatrix();
    glTranslatef(o->posx, o->posy, o->posz);
    glScalef(o->length, o->height, o->width);
    glutSolidCube(1);
    glPopMatrix();
}
