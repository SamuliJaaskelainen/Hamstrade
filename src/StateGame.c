// Game only has one state, this

#pragma bank 2
#include "main.h"
UINT8 bank_STATE_GAME = 2;

#include "..\res\src\tiles.h"
#include "..\res\src\map.h"
#include "..\res\src\titlescreen.h"
#include "..\res\src\titlescreen_tileset.h"

#include "Keys.h"
#include "ZGBMain.h"
#include "Scroll.h"
#include "SpriteManager.h"

// Collide with all the solid and hurting tiles
const UINT8 collision_tiles[] = {1, 2, 3, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
								 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 37, 38, 39,
								 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59,
								 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79,
								 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99,
								 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119,
								 120, 0};

extern UINT8 *poopster_mod_Data[];
extern UINT8 *tunnari_mod_Data[];

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
	BGP_REG = 0b11100100;

	// Sprite Palette: DarkGray, White, LightGray, Black
	OBP0_REG = 0b11010010;
	OBP1_REG = 0b11010010;

	SHOW_SPRITES;

	// Load level
	InitScrollTiles(0, &titlescreen_tileset, 4);
	InitScroll(titlescreenWidth, titlescreenHeight, titlescreen, NULL, 0, 4);
	SHOW_BKG;

	// Enable audio
	NR52_REG = 0x80;
	NR51_REG = 0xFF;
	NR50_REG = 0x77;

	PlayMusic(tunnari_mod_Data, 3, 1);
}

// All game logic is in SpritePlayer
void Update_STATE_GAME()
{
	if (level == 0 && KEY_PRESSED(J_START))
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
		//if (spr->type != SPRITE_PLAYER)
		{
			SpriteManagerRemoveSprite(spr);
		}
	}

	if (level == 0)
	{
		InitScrollTiles(0, &tiles, 3);
		InitScroll(mapWidth, mapHeight, map, collision_tiles, 0, 3);
		level = 1;
		
		// Follow player while scrolling
		scroll_target = SpriteManagerAdd(SPRITE_PLAYER, 40, 96);
		
		// Play in-game music
		PlayMusic(poopster_mod_Data, 3, 1);
	}
	else
	{
		InitScrollTiles(0, &titlescreen_tileset, 4);
		InitScroll(titlescreenWidth, titlescreenHeight, titlescreen, NULL, 0, 4);
		level = 0;
		
		PlayMusic(tunnari_mod_Data, 3, 1);
	}
}
