#include "collision.h"

static int rangeIntersect(float mina, float maxa, float minb, float maxb);
static int hasCollision(Object* a, Object* b);
static int isAbove(Object* ap, Object* bp);
static Side aRelativeTob(Object* ap, Object* bp);
static void bulletCollisionBuildMode(Object* bullet, ObjectNode* blockNode);


int rangeIntersect(float mina, float maxa, float minb, float maxb)
{
    return maxa > minb && mina < maxb;
}

// box intersection
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

// is a inside of b, looking only at x and z
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

// max height that a player can step onto without jumping
static float kneeHeight = 0.25;

// is a is above b
int isAbove(Object* ap, Object* bp)
{
    Object a = *ap, b = *bp;
    float y=a.posy-a.vy.curr;
	// if same width as a player, it's him
    float offset=(a.width==player.width) ? kneeHeight : 0;
    return y - offset > b.posy + b.height / 2;
}

// is a below b
int isBelow(Object* ap, Object* bp)
{
    Object a = *ap, b = *bp;
    if (!aIsInb(ap, bp))
        return 0;
    float y=a.posy-a.vy.curr;
    float offset=(a.width==player.width) ? playerHeadHeight : 0;
    return y - offset+ a.height / 2 < b.posy - b.height / 2;
}

// which side of the box is the collision
Side aRelativeTob(Object* ap, Object* bp)
{
    Object a = *ap, b = *bp;
    Side x, z;
    float ax, az, leftx, rightx, frontz, backz;
    ax = a.posx-a.vx.curr;
    az = a.posz-a.vz.curr;
    leftx = b.posx - b.length / 2 - a.length / 2;
    rightx = b.posx + b.length / 2 + a.length / 2;
	// is it closer to left or right side
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
	// is it closer to b on x or z axis
    return (ax < az) ? x : z;
}

// distance to move the player to not have collision
static const float eps = 0.0001f;
void playerCollision(void)
{
    // jumping will become 0 if player is standing on something
    state.jumping = 1;
    ObjectNode* l;
    for (l = Blocks; l != NULL; l = l->next){
        Object* p = l->o;
        if (hasCollision(&player, p)) {
            // floor collision
            if (isAbove(&player, p)) {
				// if player is above p but not within it, ignore collision
                if (aIsInb(&player, p)) {
                    playerOnBlockReact(p);
                }
                // ceiling collision
            } else if (isBelow(&player, p)) {
                player.posy -= eps;
                if (player.vy.curr > 0) {
                    player.vy.curr = -player.vy.curr / 2;
                    player.vy.goal = -player.vy.goal / 2;
                }
            } else {
				// prevent object intersection from the side
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

// check collision of active bullets with blocks
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

// in build mode all colors create a new block, except black which deletes them
void bulletCollisionBuildMode(Object* bullet, ObjectNode* blockNode)
{
    if (!state.buildMode)
        return;
    Object* block = blockNode->o;
    float x = block->posx, y = block->posy, z = block->posz;
    if (getColor(bullet) == BLACK) {
        removeNode(&Blocks, blockNode);
        return;
	// create a block on the side of collision
    } else if (isAbove(bullet, block)) {
        y = block->posy + block->height / 2 + sizey / 2;
    } else if (isBelow(bullet, block)) {
        y = block->posy - block->height /2 - sizey / 2;
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
	// if test block has collision with the player, then don't create it
    Object test;
    test.posx = x, test.posy = y, test.posz = z;
    test.length = sizex, test.height = sizey, test.width = sizez;
    if (hasCollision(&test, &player)) {
        return;
    }
	// or if it has collision with other blocks
    ObjectNode* l;
    for (l = Blocks; l != NULL; l = l->next){
        Object* p = l->o;
        if (hasCollision(&test, p)) {
            return;
        }
    }
    addToList(&Blocks, x, y, z);
}
