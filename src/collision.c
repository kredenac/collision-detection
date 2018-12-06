#include "collision.h"

static int rangeIntersect(float mina, float maxa, float minb, float maxb);
static int hasCollision(Object* a, Object* b);
static int isAbove(Object* ap, Object* bp);
static Side aRelativeTob(Object* ap, Object* bp);
static void bulletCollisionBuildMode(Object* bullet, ObjectNode* blockNode);

/*proverava da li range mina-maxa ima presek sa minb-maxb*/
int rangeIntersect(float mina, float maxa, float minb, float maxb)
{
    return maxa > minb && mina < maxb;
}

/*proverava da li dva objekta imaju presek po svakoj osi,
 * ako imaju to je kolizija*/
int hasCollision(Object* ap, Object* bp)
{
    Object a = *ap, b = *bp;
    float mina, maxa, minb, maxb;
    mina = a.posx - a.length / 2;
    maxa = a.posx + a.length / 2;
    minb = b.posx - b.length / 2;
    maxb = b.posx + b.length / 2;
    int interx = rangeIntersect(mina, maxa, minb, maxb);

    mina = a.posy - a.height / 2;
    maxa = a.posy + a.height / 2;
    minb = b.posy - b.height / 2;
    maxb = b.posy + b.height / 2;
    int intery = rangeIntersect(mina, maxa, minb, maxb);

    mina = a.posz - a.width / 2;
    maxa = a.posz + a.width / 2;
    minb = b.posz - b.width / 2;
    maxb = b.posz + b.width / 2;
    int interz = rangeIntersect(mina, maxa, minb, maxb);

    return interx && intery && interz;
}

/*da li se objekat a nalazi u objektu b, gledajuci samo xz ravan*/
int aIsInb(Object* ap, Object* bp)
{
    Object a = *ap, b = *bp;
    float mina, maxa, minb, maxb;
    mina = a.posx - a.length / 4;
    maxa = a.posx + a.length / 4;
    minb = b.posx - b.length / 2;
    maxb = b.posx + b.length / 2;
    int interx = rangeIntersect(mina, maxa, minb, maxb);

    mina = a.posz - a.width / 4;
    maxa = a.posz + a.width / 4;
    minb = b.posz - b.width / 2;
    maxb = b.posz + b.width / 2;
    int interz = rangeIntersect(mina, maxa, minb, maxb);

    return interx && interz;
}

/*za playera se uzima u obzir i visina kolena - jer do te visine moze da
se popne na objekat iako nije skroz ispod njega*/
static float kneeHeight = 0.25;
/*racuna da li je a iznad b*/
int isAbove(Object* ap, Object* bp)
{
    Object a = *ap, b = *bp;
    float y=a.posy-a.vy.curr;
    /*ako je isti width kao player smatram da je to bas on*/
    float offset=(a.width==player.width) ? kneeHeight : 0;
    return y - offset > b.posy + b.height / 2;
}

/*racuna da li je a ispod b*/
int isBelow(Object* ap, Object* bp)
{
    Object a = *ap, b = *bp;
    if (!aIsInb(ap, bp))
        return 0;
    float y=a.posy-a.vy.curr;
    /*ako je isti width kao player smatram da je to bas on*/
    float offset=(a.width==player.width) ? playerHeadHeight : 0;
    return y - offset+ a.height / 2 < b.posy - b.height / 2;
    // return lastPosy - playerHeadHeight + player.height / 2 < b.posy - b.height / 2;
}

/*kada ima kolizije da se odluci sa koje strane b je*/
Side aRelativeTob(Object* ap, Object* bp)
{
    Object a = *ap, b = *bp;
    Side x, z;
    float ax, az, leftx, rightx, frontz, backz;
    ax = a.posx-a.vx.curr;
    az = a.posz-a.vz.curr;
    leftx = b.posx - b.length / 2 - a.length / 2;
    rightx = b.posx + b.length / 2 + a.length / 2;
    /*da li je blizi levoj ili desnoj strani*/
    if (fabsf(leftx - ax) < fabsf(rightx - ax)) {
        x = LEFT;
        ax = fabsf(leftx - ax);
    } else {
        x = RIGHT;
        ax = fabsf(rightx - ax);
    }
    backz = b.posz - b.width / 2 - a.width / 2;
    frontz = b.posz + b.width / 2 + a.width / 2;
    if (fabsf(backz - az) < fabsf(frontz - az)) {
        z = BACK;
        az = fabsf(backz - az);
    } else {
        z = FRONT;
        az = fabsf(frontz - az);
    }
    /*da li je a blizi b po x ili z osi?*/
    return (ax < az) ? x : z;
}

/*eps sluzi kao mala velicina za koju odaljim igraca od blokova
da ne bi bila kolizija */
static const float eps = 0.0001;
void playerCollision(void)
{
    /*postavlja se jumping na 1, pa ako stoji na necemu bice 0*/
    state.jumping = 1;
    ObjectNode* l;
    for (l = Blocks; l != NULL; l = l->next){
        Object* p = l->o;
        if (hasCollision(&player, p)) {
            /*kolizija sa podom*/
            if (isAbove(&player, p)) {
                /*ako je igrac iznad p, ali nije skroz u njemu, ignorise koliziju*/
                if (aIsInb(&player, p)) {
                    playerOnBlockReact(p);
                }
                /*kolizija sa plafonom*/
            } else if (isBelow(&player, p)) {
                player.posy -= eps;
                if (player.vy.curr > 0) {
                    player.vy.curr = -player.vy.curr / 2;
                    player.vy.goal = -player.vy.goal / 2;
                }
            } else {
                /*ako je kolizija sa strane spreci igraca da ulazi u objekat*/
                Side side = aRelativeTob(&player, p);
                switch (side) {
                case FRONT:
                    player.posz = p->posz + p->width / 2 + player.width / 2 + eps;
                    break;
                case BACK:
                    player.posz = p->posz - p->width / 2 - player.width / 2 - eps;
                    break;
                case LEFT:
                    player.posx = p->posx - p->length / 2 - player.length / 2 - eps;
                    break;
                case RIGHT:
                    player.posx = p->posx + p->length / 2 + player.length / 2 + eps;
                    break;
                default:
                    break;
                }
            }
        }
    }
}

/*prolazi kroz sve slotove, ako je aktivan metak proveri koliziju sa kockama*/
void bulletCollision(void)
{
    int i;
    for (i = 0; i < MAX_BULLETS; i++) {
        if (bulletsActive[i]) {
            ObjectNode* l;
            for (l = Blocks; l != NULL; l = l->next) {
                Object* block = l->o;

                if (hasCollision(block, &bullets[i])) {
                    bulletsActive[i] = 0;
                    paintBlock(block, &bullets[i]);
                    bulletCollisionBuildMode(&bullets[i], l);
                    break;
                }
            }
        }
    }
}

/*u build modu sve boje osim crne stvore novi blok pri sudaru sa postojecim.*/
/*crna boja brise postojece blokove*/
void bulletCollisionBuildMode(Object* bullet, ObjectNode* blockNode)
{
    if (!state.buildMode)
        return;
    Object* block = blockNode->o;
    float x = block->posx, y = block->posy, z = block->posz;
    /*ako je blok pogodjen crnom bojom onda se brise*/
    if (getColor(bullet) == BLACK) {
        removeNode(&Blocks, blockNode);
        return;
    /*inace napravi novi blok sa one strane bloka gde je udario metak.*/
    /**postavi ga iznad*/
    } else if (isAbove(bullet, block)) {
        y = block->posy + block->height / 2 + sizey / 2;
    /**postavi ga ispod*/
    } else if (isBelow(bullet, block)) {
        y = block->posy - block->height /2 - sizey / 2;
    /**postavi ga sa strane gde je metak udario blok*/
    } else {
        Side side = aRelativeTob(bullet, block);
        switch (side) {
        case FRONT:
            z += block->width / 2 + sizez / 2;
            break;
        case BACK:
            z -= block->width / 2 + sizez / 2;
            break;
        case LEFT:
            x -= block->length / 2 + sizex / 2;
            break;
        case RIGHT:
            x += block->length  / 2 + sizex / 2;
            break;
        default:
            break;
        }
    }
    /*pravim test blok*/
    Object test;
    test.posx = x, test.posy = y, test.posz = z;
    test.length = sizex, test.height = sizey, test.width = sizez;
    /*ako on ima koliziju sa igracem ne stvara se*/
    if (hasCollision(&test, &player)) {
        return;
    }
    /*ne stvara se ni ako ima koliziju sa nekim druigm blokom*/
    ObjectNode* l;
    for (l = Blocks; l != NULL; l = l->next){
        Object* p = l->o;
        if (hasCollision(&test, p)) {
            return;
        }
    }
    addToList(&Blocks, x, y, z);
}
