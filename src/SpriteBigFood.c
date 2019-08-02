#pragma bank 2
#include "main.h"
UINT8 bank_SPRITE_BIG_FOOD = 2;

#include "SpriteManager.h"

const UINT8 bigFoodAnim[] = {4, 0, 1, 2, 3};

void Start_SPRITE_BIG_FOOD()
{
	THIS->coll_x = 0;
	THIS->coll_y = 0;
	THIS->coll_w = 16;
	THIS->coll_h = 16;

	SetSpriteAnim(THIS, bigFoodAnim, 10);
}

void Update_SPRITE_BIG_FOOD()
{
}

void Destroy_SPRITE_BIG_FOOD()
{
}