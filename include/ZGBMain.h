#ifndef ZGBMAIN_H
#define ZGBMAIN_H

#define STATES         \
    _STATE(STATE_GAME) \
    STATE_DEF_END

#define SPRITES                           \
    _SPRITE_DMG(SPRITE_PLAYER, player)    \
    _SPRITE_DMG(SPRITE_FOOD, cherry)      \
    _SPRITE_DMG(SPRITE_BIG_FOOD, pear)    \
    _SPRITE_DMG(SPRITE_GOAL, goalbottle)  \
    _SPRITE_DMG(SPRITE_UI_POOP, poop)     \
    _SPRITE_DMG(SPRITE_GROUND_POOP, poop) \
    _SPRITE_DMG(SPRITE_AIR_POOP, poop)    \
    SPRITE_DEF_END

#include "ZGBMain_Init.h"

#endif