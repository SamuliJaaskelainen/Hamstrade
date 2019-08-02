// All player and game logic

#pragma bank 2
#include "main.h"
UINT8 bank_SPRITE_PLAYER = 2;

#include "Keys.h"
#include "SpriteManager.h"
#include "Sound.h"
#include "ZGBMain.h"

// Player fatness
UINT8 poopAmount = 3;

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
const UINT8 animIdle[] = {1, 1};
const UINT8 animWalk[] = {2, 2, 3};
const UINT8 animJumpUp[] = {1, 6};
const UINT8 animJumpPeak[] = {1, 5};
const UINT8 animJumpDown[] = {1, 7};
const UINT8 animRespawn[] = {3, 8, 9, 10};

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
INT8 wallJumpFrames = -1;
INT16 wallJumpAccelX;

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
}

// Play stepping sound if we have ran enough from last time
void StepAudio()
{
    if (moveState == GROUNDED && stepSound > 12)
    {
        stepSound = 0;
        PlayFx(CHANNEL_4, 4, 0x05, 0x31, 0x78, 0x80);
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
            wallJumpFrames = -1;
            accelY = -jumpForces[poopAmount];
            jumpPeak = 0;
            moveState = ONAIR;
            PlayFx(CHANNEL_1, 5, 0x71, 0x03, 0x44, 0xc8, 0x80);
        }
        else if (KEY_TICKED(J_B))
        {
            if (poopAmount > 0)
            {
                poopAmount--;
                accelX += 100;
            }
        }
    }
    else if (moveState == ONAIR)
    {
        // Poop jump
        if (KEY_TICKED(J_B) && poopJumped == 0)
        {
            if (poopAmount > 0)
            {
                poopJumped = 1;
                poopAmount--;
                accelY = -poopJumpForces[poopAmount];
                jumpPeak = 0;
                PlayFx(CHANNEL_1, 5, 0x71, 0x03, 0x44, 0xc8, 0x80);
            }
        }

        if (collisionY != 0)
        {
            jumpPeak = 1;
        }

        // Enable wall jump when normal jump press ends
        if (KEY_RELEASED(J_A))
        {
            wallJumpFrames = 0;
        }

        if (jumpPeak == 0 && (KEY_PRESSED(J_A) || poopJumped != 0) && accelY > -350)
        {
            accelY -= 20;
        }
        else if (accelY < 300)
        {
            if (collisionX == 0)
            {
                accelY += 20;
            }
            else
            {
                // Wall slide
                accelY = 80;

                // Wall jump
                if (KEY_PRESSED(J_A) && wallJumpFrames == 0)
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
                    jumpPeak = 0;
                    PlayFx(CHANNEL_1, 5, 0x71, 0x03, 0x44, 0xc8, 0x80);
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
    collisionY = TranslateSprite(THIS, 0, accelY / 100);

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
                PlayFx(CHANNEL_4, 4, 0x32, 0x71, 0x73, 0x80);
                stepSound = 6;
                poopJumped = 0;
                moveState = GROUNDED;
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
                if (poopAmount <= 3)
                {
                    poopAmount++;
                    PlayFx(CHANNEL_4, 5, 0x1c, 0xe5, 0x7a, 0xc0);
                    SpriteManagerRemoveSprite(spr);
                }
            }
        }
        else if (spr->type == SPRITE_BIG_FOOD)
        {
            if (CheckCollision(THIS, spr))
            {
                if (poopAmount <= 3)
                {
                    checkpointX = THIS->x;
                    checkpointY = THIS->y;
                    poopAmount = 3;
                    PlayFx(CHANNEL_4, 5, 0x1c, 0xe5, 0x7a, 0xc0);
                    SpriteManagerRemoveSprite(spr);
                }
            }
        }
        else if (spr->type == SPRITE_GOAL)
        {
            if (CheckCollision(THIS, spr))
            {
            }
        }
    }

    // Animation
    // Play correct animation based on current state & input
    if (moveState == GROUNDED)
    {
        if (accelX < 100 && accelX > -100)
        {
            SetSpriteAnim(THIS, animIdle, 1);
        }
        else
        {
            StepAudio();
            SetSpriteAnim(THIS, animWalk, 15);
        }
    }
    else
    {
        if (accelY > 60)
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
        PlayFx(CHANNEL_1, 5, 0x73, 0x03, 0x4c, 0xfa, 0x80);
    }
}

void Destroy_SPRITE_PLAYER()
{
}