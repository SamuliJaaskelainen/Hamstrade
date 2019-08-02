#ifndef ZGBMAIN_H
#define ZGBMAIN_H

#define STATES         \
    _STATE(STATE_GAME) \
    STATE_DEF_END

#define SPRITES                                            \
    _SPRITE_DMG(SPRITE_PLAYER, player, 3, FRAME_16x16, 15) \
    _SPRITE_DMG(SPRITE_FOOD, rock, 3, FRAME_16x16, 1)      \
    _SPRITE_DMG(SPRITE_BIG_FOOD, rock, 3, FRAME_16x16, 1)  \
    _SPRITE_DMG(SPRITE_GOAL, rock, 3, FRAME_16x16, 1)      \
    SPRITE_DEF_END

#include "ZGBMain_Init.h"

#endif