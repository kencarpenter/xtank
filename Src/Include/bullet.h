/* bullet.h - things pertaining to bullets and explosions */

/*
$Author: lidl $
$Id: bullet.h,v 2.7 1992/09/13 07:03:02 lidl Exp $

$Log: bullet.h,v $
 * Revision 2.7  1992/09/13  07:03:02  lidl
 * aaron 1.3e patches
 *
 * Revision 2.6  1992/08/31  01:50:45  lidl
 * changed to use tanktypes.h, instead of types.h
 *
 * Revision 2.5  1992/03/31  21:49:23  lidl
 * Post Aaron-3d patches, camo patches, march patches & misc PIX stuff
 *
 * Revision 2.4  1992/01/29  08:39:11  lidl
 * post aaron patches, seems to mostly work now
 *
 * Revision 2.3  1991/02/10  13:50:11  rpotter
 * bug fixes, display tweaks, non-restart fixes, header reorg.
 *
 * Revision 2.2  91/01/20  09:57:24  rpotter
 * complete rewrite of vehicle death, other tweaks
 * 
 * Revision 2.1  91/01/17  07:11:00  rpotter
 * lint warnings and a fix to update_vector()
 * 
 * Revision 2.0  91/01/17  02:09:08  rpotter
 * small changes
 * 
 * Revision 1.1  90/12/29  21:02:00  aahz
 * Initial revision
 * 
*/

#ifndef _BULLET_H_
#define _BULLET_H_

#include "object.h"
#include "vehicle.h"
#include "tanktypes.h"

typedef struct {
    Vehicle *owner;		/* pointer to vehicle that shot bullet */
    int thrower;		/* color of the guy who thre the frisbee */
    Loc  *loc;			/* pointer to location info */
    Loc  *old_loc;		/* pointer to previous location info */
    Loc   loc1;			/* 1st area for location info */
    Loc   loc2;			/* 2nd area for location info */
    float xspeed;		/* speed of travel in x direction */
    float yspeed;		/* speed of travel in y direction */
    WeaponType type;
    int   life;			/* number of frames left before bullet dies */
    Boolean hurt_owner;	/* whether bullet can hurt owner or not */
    lCoord target;		/* last target update for a smart weapon */
    int state;                  /* what a smart bullet is up to */
    int mode;
} Bullet;

typedef struct {
    int   number;		/* number of bullets */
    Bullet *list[MAX_BULLETS];	/* array of pointers to bullets */
    Bullet array[MAX_BULLETS];	/* array of bullets */
} Bset;

typedef struct {
    int   x, y, z;		/* coords */
    int   screen_x[MAX_TERMINALS];	/* x coord on screen */
    int   screen_y[MAX_TERMINALS];	/* y coord on screen */
    int   old_screen_x[MAX_TERMINALS];	/* previous x coord on screen */
    int   old_screen_y[MAX_TERMINALS];	/* previous y coord on screen */
    int   life;			/* # frames before explosion dies */
    Object *obj;		/* pointer to object for the explosion */
    int   color;
} Exp;

typedef struct {
    int   number;		/* number of explosions */
    Exp  *list[MAX_EXPS];	/* array of pointers to explosions */
    Exp   array[MAX_EXPS];	/* array of explosions */
} Eset;


#endif ndef _BULLET_H_
