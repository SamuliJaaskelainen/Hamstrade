// All player and game logic

#pragma bank 2
#include "main.h"
UINT8 bank_SPRITE_PLAYER = 2;

void NextLevel();

#include "Keys.h"
#include "SpriteManager.h"
#include "Sound.h"
#include "ZGBMain.h"

// Player fatness
UINT8 poopAmount = 3;

struct Sprite *uiPoop[4];

// Movement values for tweaking platforming feel
const UINT8 walkSpeeds[] = {200, 190, 130, 110};
const UINT8 jumpForces[] = {100, 120, 180, 200};
const UINT8 poopJumpForces[] = {200, 180, 120, 100};
const UINT16 wallJumpForcesX[] = {260, 250, 150, 150};
const UINT16 wallJumpForcesY[] = {360, 350, 100, 100};
#define walkIncrease 50
#define slowDownSpeed 8
#define respawnMoveSpeed 3

// Animation frames (number of frames followed by all the frames to play)
const UINT8 animIdle[] = {2, 0, 1};
const UINT8 animWalk[] = {2, 2, 0};
const UINT8 animWallSlide[] = {2, 3, 4};
const UINT8 animJumpUp[] = {1, 5};
const UINT8 animJumpPeak[] = {1, 6};
const UINT8 animJumpDown[] = {1, 7};
const UINT8 animRespawn[] = {4, 8, 9, 10, 6};

// UI poop animation frames
const UINT8 animUIPoop[] = {1, 1};
const UINT8 animUIPoopPerish[] = {3, 1, 2, 3};
const UINT8 animUIPoopGone[] = {1, 3};

typedef enum
{
    GROUNDED, // When player is on ground
    ONAIR,    // When player is on air
    RESPAWN   // When player is moving to respawn point after hurt
} MOVE_STATE;

// Start from air to drop the player on the stage
MOVE_STATE moveState = ONAIR;

// Player movement physics
INT16 accelY;
INT16 accelX;
UINT8 jumpPeak;
UINT8 poopJumped = 0;
INT8 wallJumpFrames = 0;
INT8 canWallJump = 0;
INT16 wallJumpAccelX;
UINT8 wallSlide = 0;

// Storing collisions around player after movement
UINT8 collisionX;
UINT8 collisionY;
UINT8 groundCollision;

// Plase to respawn when hurt
INT16 checkpointX = 10;
INT16 checkpointY = 96;

// Last checkpoint
UINT8 checkpointIndex = 0;

// When do we reach next checkpoint
INT16 nextCheckpoint = 376;

// Timer for playing step sounds
UINT8 stepSound = 0;

// General helpers
UINT8 i;
struct Sprite *spr;

// Call to reset player's state back to default
void ResetState()
{
    accelY = 0;
    accelX = 0;
    collisionX = 0;
    collisionY = 0;
    groundCollision = 0;
    jumpPeak = 0;
    poopAmount = 3;
    moveState = ONAIR;
}

// Set starting position and collider size
void Start_SPRITE_PLAYER()
{
    THIS->coll_x = 2;
    THIS->coll_y = 2;
    THIS->coll_w = 12;
    THIS->coll_h = 14;
    ResetState();

    for (i = 0; i < 3; ++i)
    {
        uiPoop[i] = SpriteManagerAdd(SPRITE_UI_POOP, 16, 0);
        SetSpriteAnim(uiPoop[i], animUIPoop, 1);
    }
}

// Play stepping sound if we have ran enough from last time
void StepAudio()
{
    if (moveState == GROUNDED && stepSound > 12)
    {
        stepSound = 0;
        //PlayFx(CHANNEL_4, 4, 0x05, 0x31, 0x78, 0x80);
    }
}

void UpdateUI()
{
    // UI
    for (i = 0; i < 3; ++i)
    {
        uiPoop[i]->x = (THIS->x - 16) + (16 * i);

        if (poopAmount > i)
        {
            SetSpriteAnim(uiPoop[i], animUIPoop, 1);
            //uiPoop[i]->y = 0;
        }
        else
        {
            if (uiPoop[i]->anim_data == animUIPoopGone ||
                uiPoop[i]->current_frame == animUIPoopPerish[0]-1)
            {
                SetSpriteAnim(uiPoop[i], animUIPoopGone, 1);
            }
            else
            {
                SetSpriteAnim(uiPoop[i], animUIPoopPerish, 15);
            }
            
            //uiPoop[i]->y = -8;
        }
    }
}

void SpawnPoop(UINT8 poopType)
{
    poopAmount--;

    // POOP SOUND
    PlayFx(CHANNEL_1, 30, 0x49, 0x28, 0x39, 0x07, 0xc6);
    //PlayFx(CHANNEL_1, 5, 0x71, 0x03, 0x44, 0xc8, 0x80);

    if (THIS->flags == S_FLIPX)
    {
        SpriteManagerAdd(poopType, THIS->x + 8, THIS->y);
    }
    else
    {
        SpriteManagerAdd(poopType, THIS->x - 8, THIS->y);
    }
}

void Update_SPRITE_PLAYER()
{
    // Respawn
    // Move player to last checkpoint and reset them
    if (moveState == RESPAWN)
    {
        if (THIS->x > checkpointX)
        {
            if ((THIS->x - respawnMoveSpeed) > checkpointX)
            {
                THIS->x -= respawnMoveSpeed;
            }
            else
            {
                THIS->x--;
            }
        }
        else if (THIS->x < checkpointX)
        {
            if ((THIS->x + respawnMoveSpeed) < checkpointX)
            {
                THIS->x += respawnMoveSpeed;
            }
            else
            {
                THIS->x++;
            }
        }

        if (THIS->y > checkpointY)
        {
            if ((THIS->y - respawnMoveSpeed) > checkpointY)
            {
                THIS->y -= respawnMoveSpeed;
            }
            else
            {
                THIS->y--;
            }
        }
        else if (THIS->y < checkpointY)
        {
            if ((THIS->y + respawnMoveSpeed) < checkpointY)
            {
                THIS->y += respawnMoveSpeed;
            }
            else
            {
                THIS->y++;
            }
        }

        UpdateUI();

        if (THIS->x == checkpointX && THIS->y == checkpointY)
        {
            ResetState();
        }
        return;
    }

    // Directional input
    // Flip sprite to correct direction
    // Increase x acceleration to pressed direction
    // Clamp running speed when grounded
    if (wallJumpFrames > 0)
    {
        accelX = wallJumpAccelX;
        wallJumpFrames--;
    }
    else if (KEY_PRESSED(J_LEFT))
    {
        THIS->flags = S_FLIPX;

        stepSound++;
        accelX -= walkIncrease;
        if (accelX < -walkSpeeds[poopAmount] && moveState == GROUNDED)
            accelX = -walkSpeeds[poopAmount];
    }
    else if (KEY_PRESSED(J_RIGHT))
    {
        THIS->flags = 0;

        stepSound++;
        accelX += walkIncrease;
        if (accelX > walkSpeeds[poopAmount] && moveState == GROUNDED)
            accelX = walkSpeeds[poopAmount];
    }

    // Jump
    // If we are on ground and A is pressed, jump
    // Jump sound is played on every successful jump
    // Jump is controlled by reaching high enought acceleration
    // or hitting a wall with head
    // In both cases, we set peak to true and start falling
    if (moveState == GROUNDED)
    {
        if (KEY_TICKED(J_A))
        {
            canWallJump = 0;
            accelY = -jumpForces[poopAmount];
            jumpPeak = 0;
            moveState = ONAIR;
            // JUMP SOUND
            PlayFx(CHANNEL_1, 30, 0x77, 0x00, 0xab, 0xc4, 0xc6);
            //PlayFx(CHANNEL_1, 5, 0x71, 0x03, 0x44, 0xc8, 0x80);
        }
        else if (KEY_TICKED(J_B))
        {
            if (poopAmount > 0)
            {
                if (THIS->flags == S_FLIPX)
                {
                    accelX -= 100;
                }
                else
                {
                    accelX += 100;
                }

                SpawnPoop(SPRITE_GROUND_POOP);
            }
        }
    }
    else if (moveState == ONAIR)
    {
        if (canWallJump == 0 && KEY_RELEASED(J_A))
        {
            canWallJump = 1;
        }

        // Poop jump
        if (KEY_TICKED(J_B) && poopJumped == 0)
        {
            if (poopAmount > 0)
            {
                poopJumped = 1;
                accelY = -poopJumpForces[poopAmount];
                jumpPeak = 0;

                SpawnPoop(SPRITE_AIR_POOP);
            }
        }

        if (collisionY != 0)
        {
            jumpPeak = 1;
        }

        if (jumpPeak == 0 && (KEY_PRESSED(J_A) || poopJumped != 0) && accelY > -350)
        {
            accelY -= 20;
        }
        else if (accelY < 300)
        {
            poopJumped = 0;

            if (collisionX == 0)
            {
                accelY += 20;

                if (wallSlide > 0)
                {
                    --wallSlide;
                }
            }
            else
            {
                // Wall slide
                wallSlide = 2;
                accelY = 80;

                // Wall jump
                if (KEY_PRESSED(J_A) && wallJumpFrames == 0 && canWallJump == 1)
                {
                    accelY = -wallJumpForcesY[poopAmount];

                    if (KEY_PRESSED(J_LEFT))
                    {
                        wallJumpAccelX = wallJumpForcesX[poopAmount];
                    }
                    else if (KEY_PRESSED(J_RIGHT))
                    {
                        wallJumpAccelX = -wallJumpForcesX[poopAmount];
                    }
                    wallJumpFrames = 10;
                    canWallJump = 0;
                    jumpPeak = 0;

                    // JUMP SOUND
                    PlayFx(CHANNEL_1, 30, 0x77, 0x00, 0xab, 0xc4, 0xc6);

                    //PlayFx(CHANNEL_1, 5, 0x71, 0x03, 0x44, 0xc8, 0x80);
                }
            }

            jumpPeak = 1;
        }

        if (accelX < -walkSpeeds[poopAmount])
            accelX = -walkSpeeds[poopAmount];
        if (accelX > walkSpeeds[poopAmount])
            accelX = walkSpeeds[poopAmount];
    }

    // Move player and check for collisions
    // Do two movements to get colliders from both directions
    collisionX = TranslateSprite(THIS, accelX / 100, 0);
    if ((INT16)THIS->y < -accelY / 100)
    {
        collisionY = TranslateSprite(THIS, 0, -THIS->y);
    }
    else
    {
        collisionY = TranslateSprite(THIS, 0, accelY / 100);
    }

    // X physics
    // Stop movement if we hit something
    // Otherwise drag
    if (collisionX != 0 && wallJumpFrames < 1)
    {
        accelX = 0;
    }
    else if (!KEY_PRESSED(J_LEFT) && !KEY_PRESSED(J_RIGHT) && wallJumpFrames < 1)
    {
        if (accelX > 0)
        {
            if (accelX > slowDownSpeed)
            {
                accelX -= slowDownSpeed;
            }
            else
            {
                accelX = 0;
            }
        }
        else if (accelX < 0)
        {
            if (accelX < -slowDownSpeed)
            {
                accelX += slowDownSpeed;
            }
            else
            {
                accelX = 0;
            }
        }
    }

    // Y physics
    // Drop down if we don't have ground under
    // Play audio on land
    // Start from step sound 6 on land to have start playing stepping audio when we walk quickly
    if (accelY > 0)
    {
        groundCollision = collisionY;

        if (groundCollision == 0)
        {
            moveState = ONAIR;
        }
        else
        {
            if (moveState == ONAIR)
            {
                stepSound = 6;
                poopJumped = 0;
                moveState = GROUNDED;

                // LAND SOUND
                PlayFx(CHANNEL_4, 30, 0x0e, 0x21, 0x00, 0xc0);
                //PlayFx(CHANNEL_4, 4, 0x32, 0x71, 0x73, 0x80);
            }
            accelY = 100;
        }
    }
    else
    {
        groundCollision = 0;
        moveState = ONAIR;
    }

    SPRITEMANAGER_ITERATE(i, spr)
    {
        if (spr->type == SPRITE_FOOD)
        {
            if (CheckCollision(THIS, spr))
            {
                if (poopAmount < 3)
                {
                    poopAmount++;
                    SpriteManagerRemoveSprite(spr);

                    // EAT SOUND
                    PlayFx(CHANNEL_1, 30, 0x4c, 0x1a, 0x6a, 0xb0, 0xc6);
                    //PlayFx(CHANNEL_4, 5, 0x1c, 0xe5, 0x7a, 0xc0);
                }
            }
        }
        else if (spr->type == SPRITE_BIG_FOOD)
        {
            if (CheckCollision(THIS, spr))
            {
                // You can always collect the big food because it doubles as a checkpoint.
                //if (poopAmount < 3)
                {
                    checkpointX = THIS->x;
                    checkpointY = THIS->y;
                    poopAmount = 3;
                    SpriteManagerRemoveSprite(spr);

                    // EAT SOUND
                    PlayFx(CHANNEL_1, 30, 0x4c, 0x1a, 0x6a, 0xb0, 0xc6);
                    //PlayFx(CHANNEL_4, 5, 0x1c, 0xe5, 0x7a, 0xc0);
                }
            }
        }
        else if (spr->type == SPRITE_GOAL)
        {
            if (CheckCollision(THIS, spr))
            {
                NextLevel();
                return;
            }
        }
    }

    // Animation
    // Play correct animation based on current state & input
    if (moveState == GROUNDED)
    {
        if (accelX < 100 && accelX > -100)
        {
            SetSpriteAnim(THIS, animIdle, 5);
        }
        else
        {
            StepAudio();
            SetSpriteAnim(THIS, animWalk, 15);
        }
    }
    else
    {
        if (wallSlide)
        {
            SetSpriteAnim(THIS, animWallSlide, 5);
        }
        else if (accelY > 60)
        {
            SetSpriteAnim(THIS, animJumpDown, 1);
        }
        else if (accelY < -60)
        {
            SetSpriteAnim(THIS, animJumpUp, 1);
        }
        else
        {
            SetSpriteAnim(THIS, animJumpPeak, 1);
        }
    }

    // Hurt
    // When certain tiles are hit or player falls off the screen, play sound and respawn them
    // Note that all hurting tiles are in sequence for easier check here
    if (THIS->y > 126 || KEY_TICKED(J_SELECT) || (collisionY > 46 && collisionY < 56) || (collisionX > 46 && collisionX < 56))
    {
        moveState = RESPAWN;
        THIS->flags = 0;
        SetSpriteAnim(THIS, animRespawn, 15);

        // HURT SOUND
        PlayFx(CHANNEL_4, 30, 0x00, 0x39, 0x18, 0xc0);
        //PlayFx(CHANNEL_1, 5, 0x73, 0x03, 0x4c, 0xfa, 0x80);
    }

    // Update Ui after all the movement
    UpdateUI();
}

void Destroy_SPRITE_PLAYER()
{
}