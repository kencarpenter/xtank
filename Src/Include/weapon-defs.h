/*****************************************************************************\
* weapon-defs.h - part of XTank						      *
* 									      *
* This file contains weapon definitions.				      *
* 									      *
* The definitions are presented in the form of a macro invokation with	      *
* several arguments belonging to various contexts.  This file is #included in *
* different places under differnt definitions of the macro, each of which     *
* selects the argument(s) relevant at that point.  Kind of obscure, but it    *
* keeps all the data in the same place.					      *
* 									      *
* The macro should have the general form:				      *
* 									      *
* #define QQ(sym,type,dam,rng,ammo,tm,spd,wgt,spc,fr,ht,ac,cost) ...	      *
* 									      *
* The args are: internal symbol used in the code, text name, bullet damage,   *
* bullet range, max ammo, reload time, bullet speed, weight, space, frames,   *
* bullet heat, bullet cost, weapon cost, refill speed, height		      *
\*****************************************************************************/

#define BIG ((int) ((unsigned)(~0) >> 1))

/* sym       type              dam rng ammo tm spd  wgt spc  fr  ht  a$  cost refl, hgt*/
QQ(LMG,    "Light Machine Gun", 2, 360, 300, 2, 17,  20, 200, 22, 0,  1,  1000, 1, NORM)
QQ(MG,     "Machine Gun",       3, 360, 250, 3, 17,  50, 225, 22, 1,  2,  2200, 1, NORM)
QQ(HMG,    "Heavy Machine Gun", 4, 360, 200, 3, 17, 100, 250, 22, 2,  3,  3000, 1, NORM)
QQ(LRIFLE, "Light Pulse Rifle", 2, 480, 250, 4, 22,  50, 200, 22, 2,  1,  1500, 1, NORM)
QQ(RIFLE,  "Pulse Rifle",       3, 480, 225, 4, 22, 125, 225, 22, 3,  2,  3300, 1, NORM)
QQ(HRIFLE, "Heavy Pulse Rifle", 4, 480, 200, 4, 22, 300, 250, 22, 4,  3,  4500, 1, NORM)
QQ(LCANNON,"Light Autocannon",  3, 400, 250, 3, 22, 200, 300, 19, 3,  4,  3000, 1, NORM)
QQ(CANNON, "Autocannon",        4, 400, 225, 3, 22, 300, 350, 19, 4,  5,  6000, 1, NORM)
QQ(HCANNON,"Heavy Autocannon",  5, 400, 200, 3, 22, 500, 400, 19, 5,  6, 10000, 1, NORM)
QQ(LROCKET,"Light Rkt Launcher",4, 600, 150, 7, 25, 600, 800, 25, 4,  8,  7000, 2, NORM)
QQ(ROCKET, "Rkt Launcher",      6, 600, 125, 7, 25, 900,1200, 25, 6, 10, 10000, 2, NORM)
QQ(HROCKET,"Heavy Rkt Launcher",8, 600, 100, 7, 25, 900,1600, 25, 8, 12, 15000, 2, NORM)
QQ(ACID,   "Acid Sprayer",      6, 160, 100, 3, 10, 300, 500, 17, 0, 10,  3000, 1, NORM)
QQ(FLAME,  "Flame Thrower",     3, 200, 300, 1, 12, 700, 500, 17, 1,  2,  4000, 1, NORM)
QQ(MINE,   "Mine Layer",        6,  50,  50, 2, 10,1000,1000, 70, 2, 10,  8000, 4, NORM)
QQ(SEEKER, "Heat Seeker",       8,1250,  15,15, 25,1000,1800, 51, 9, 50, 20000, 12, HIGH)

/* Note that weapons you don't want chosen by an outpost must be after SEEKER */

QQ(SLICK,  "Oil Slick",         0,  50,  50, 5, 10, 300, 500, 70, 0, 10,  2000, 1, LOW)
QQ(PROCKET,"Pocket Rocket",     8,1015,  30, 8, 35, 900,1650, 30,12, 23, 17000, 2, NORM)
QQ(UMISSLE,"Unguided Missle",   8,1995,  30, 8, 35,1000,1800, 57,12, 25, 18000, 2, NORM)
#ifdef TAC_NUKES
QQ(NUKE,   "Tactical Nuke",     8, 600,  10, 7, 35, 900,1600, 25,12,999, 15000, 15, NORM)
#endif
QQ(HARM,   "Anti-Radiation",    64, 9999, 1, 1, 25, 1000, 1800, 80, 24, 150, 30000, 192, FLY)

/* If it is to show up in the vdesign menus, it must be before Disc Shooter */

QQ(DISC,   "Disc Shooter",      0, BIG,   1,BIG, 0,   0,   0,BIG, 0,  0,     0, 0, NORM)
#undef BIG
