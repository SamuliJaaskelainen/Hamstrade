#pragma bank 2
#include "main.h"
UINT8 bank_SPRITE_GROUND_POOP = 2;
UINT8 bank_SPRITE_AIR_POOP = 2;

#include "SpriteManager.h"

const UINT8 animGroundPoop[] = {3, 0, 1, 2};
const UINT8 animAirPoop[] = {1, 0};

const UINT8 CUSTOM_DATA_LIFETIME = 0;
const UINT8 CUSTOM_DATA_YVEL = 1;
const UINT8 CUSTOM_DATA_GROUNDED = 2;

void Start_SPRITE_GROUND_POOP()
{
    SetSpriteAnim(THIS, animGroundPoop, 7);

    THIS->custom_data[CUSTOM_DATA_LIFETIME] = 24;
}

void Update_SPRITE_GROUND_POOP()
{
    if (--THIS->custom_data[CUSTOM_DATA_LIFETIME] == 0)
    {
        SpriteManagerRemoveSprite(THIS);
    }
}

void Destroy_SPRITE_GROUND_POOP()
{
}

void Start_SPRITE_AIR_POOP()
{
    SetSpriteAnim(THIS, animAirPoop, 2);


    THIS->custom_data[CUSTOM_DATA_LIFETIME] = 12;
    THIS->custom_data[CUSTOM_DATA_YVEL] = 0;
    THIS->custom_data[CUSTOM_DATA_GROUNDED] = 0;
}

void Update_SPRITE_AIR_POOP()
{
    if (!THIS->custom_data[CUSTOM_DATA_GROUNDED])
    {
        ++THIS->custom_data[CUSTOM_DATA_YVEL];

        if (TranslateSprite(THIS, 0, THIS->custom_data[CUSTOM_DATA_YVEL] >> 4))
        {
            THIS->custom_data[CUSTOM_DATA_GROUNDED] = 1;
            
            SetSpriteAnim(THIS, animGroundPoop, 15);
        }
    }
    else
    {
        if (--THIS->custom_data[CUSTOM_DATA_LIFETIME] == 0)
        {
            SpriteManagerRemoveSprite(THIS);
        }
    }
}

void Destroy_SPRITE_AIR_POOP()
{
}