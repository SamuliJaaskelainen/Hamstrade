#ifndef ZGBMAIN_H
#define ZGBMAIN_H

#define STATES         \
    _STATE(STATE_GAME) \
    STATE_DEF_END

#define SPRITES                                            \
    _SPRITE_DMG(SPRITE_PLAYER, player) \
    _SPRITE_DMG(SPRITE_FOOD, rock)      \
    _SPRITE_DMG(SPRITE_BIG_FOOD, rock)  \
    _SPRITE_DMG(SPRITE_GOAL, rock)      \
    SPRITE_DEF_END

#include "ZGBMain_Init.h"

#endif