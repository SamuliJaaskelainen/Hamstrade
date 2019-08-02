#pragma bank 2
#include "main.h"
UINT8 bank_SPRITE_FOOD = 2;

#include "SpriteManager.h"

const UINT8 foodAnim[] = {6, 0, 0, 1, 2, 2, 1};

void Start_SPRITE_FOOD()
{
	THIS->coll_x = 4;
	THIS->coll_y = 4;
	THIS->coll_w = 8;
	THIS->coll_h = 8;

	SetSpriteAnim(THIS, foodAnim, 8);
}

void Update_SPRITE_FOOD()
{
}

void Destroy_SPRITE_FOOD()
{
}