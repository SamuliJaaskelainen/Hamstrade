// Game only has one state, this

#pragma bank 2
#include "main.h"
UINT8 bank_STATE_GAME = 2;

#include "..\res\src\tiles.h"
#include "..\res\src\map.h"
#include "..\res\src\map2.h"

#include "Keys.h"
#include "ZGBMain.h"
#include "Scroll.h"
#include "SpriteManager.h"

// Collide with all the solid and hurting tiles
UINT8 collision_tiles[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
						   23, 24, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60,
						   64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 0};

extern UINT8 *poopster_mod_Data[];

UINT8 level = 0;

void Start_STATE_GAME()
{
	// Load all sprites
	UINT8 j;
	SPRITES_8x16;
	for (j = 0; j != N_SPRITE_TYPES; ++j)
	{
		SpriteManagerLoad(j);
	}

	// The binary numbers are in reverse order, with larger number meaning darker color:
	// 00 (white)
	// 01 (light gray)
	// 10 (dark gray)
	// 11 (black)

	// Bg Palette: DarkGray, White, LightGray, Black
	BGP_REG = 0b11010010;

	// Sprite Palette: DarkGray, White, LightGray, Black
	OBP0_REG = 0b11010010;
	OBP1_REG = 0b11010010;

	SHOW_SPRITES;

	// Follow player while scrolling
	scroll_target = SpriteManagerAdd(SPRITE_PLAYER, 40, 96);

	// Load level
	InitScrollTiles(0, &tiles, 3);
	InitScroll(mapWidth, mapHeight, map, collision_tiles, 0, 3);
	SHOW_BKG;

	// Enable audio
	NR52_REG = 0x80;
	NR51_REG = 0xFF;
	NR50_REG = 0x77;

	PlayMusic(poopster_mod_Data, 3, 1);
}

// All game logic is in SpritePlayer
void Update_STATE_GAME()
{
	if (KEY_TICKED(J_SELECT))
	{
		NextLevel();
	}
}

void NextLevel()
{
	UINT8 i;
	struct Sprite *spr;
	SPRITEMANAGER_ITERATE(i, spr)
	{
		if (spr->type != SPRITE_PLAYER)
		{
			SpriteManagerRemoveSprite(spr);
		}
	}

	if (level == 0)
	{
		InitScroll(map2Width, map2Height, map2, collision_tiles, 0, 4);
		level = 1;
	}
	else
	{
		InitScroll(mapWidth, mapHeight, map, collision_tiles, 0, 3);
		level = 0;
	}
}
