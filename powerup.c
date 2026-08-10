#include <stdlib.h>
#include <stdio.h>

#include "powerup.h"
#include "textureManager.h"
#include "game.h"

const int powerUpChance = 5;
const Vec2 size = {60.0f, 20.0f};
const Vec2 velocity = {0.0f, 150.0f};

const float speedPowerMult = 2.0f;
const float sizePowerMult = 2.0f;

PowerUp *trySpawnPowerUp(Vec2 position, TextureManager *textureManager) {
    if (rand() % powerUpChance != 0) {
        return NULL;
    }
    switch (rand() % POWERUP_TYPE_COUNT) {
        case SPEED_POWERUP:
            return createPowerUp(SPEED_POWERUP, (Vec3){0.5f, 0.5f, 1.0f}, 8.0f, position, textureManager->speedPowerUp);
        case STICKY_POWERUP:
            return createPowerUp(STICKY_POWERUP, (Vec3){1.0f, 0.5f, 1.0f}, 4.0f, position, textureManager->stickyPowerUp);
        case PASS_THROUGH_POWERUP:
            return createPowerUp(PASS_THROUGH_POWERUP, (Vec3){0.5f, 1.0f, 0.5f}, 2.0f, position, textureManager->passThroughPowerUp);
        case PAD_SIZE_INCREASE_POWERUP:
            return createPowerUp(PAD_SIZE_INCREASE_POWERUP, (Vec3){1.0f, 0.6f, 0.4}, 8.0f, position, textureManager->padSizeIncreasePowerUp);
        case CONFUSE_POWERUP:
            return createPowerUp(CONFUSE_POWERUP, (Vec3){1.0f, 0.3f, 0.3f}, 7.0f, position, textureManager->confusePowerUp);
        case CHAOS_POWERUP:
            return createPowerUp(CHAOS_POWERUP, (Vec3){0.9f, 0.25f, 0.25f}, 4.0f, position, textureManager->chaosPowerUp);
    }
}

PowerUp *createPowerUp(PowerUpType type, Vec3 color, float duration, Vec2 position, Texture texture) {
    PowerUp *powerUp = malloc(sizeof(PowerUp));

    powerUp->baseObject.position = position;
    powerUp->baseObject.size = size;
    powerUp->baseObject.color = color;
    powerUp->baseObject.texture = texture;

    powerUp->baseObject.velocity = velocity;
    powerUp->baseObject.destroyed = false;
    powerUp->baseObject.isSolid = true;
    powerUp->baseObject.rotation = 0.0f;

    powerUp->activated = false;
    powerUp->duration = duration;
    powerUp->type = type;

    return powerUp;
}

bool IsOtherPowerUpActive(List *powerUpPointers, PowerUpType type) {
    for (int i = 0; i < powerUpPointers->count; i++) {
        PowerUp *powerUp = ((PowerUp **)(powerUpPointers->data))[i];
        if (powerUp->activated && powerUp->type == type) {
            return true;
        }
    }
    return false;
} 

void applyPowerUp(PowerUp *powerUp, Game *game) {
    if (IsOtherPowerUpActive(game->powerUpPointers, powerUp->type)) {
        return;
    }
    powerUp->activated = true;
    switch (powerUp->type) {
        case SPEED_POWERUP:
            game->player->velocity = scaleVec2(game->player->velocity, speedPowerMult);
            break;
        case STICKY_POWERUP:
            game->ball->sticky = true;
            game->player->color = (Vec3){1.0f, 0.5, 1.0f};
            break;
        case PASS_THROUGH_POWERUP:
            game->ball->passthrough = true;
            game->ball->baseObject.color = (Vec3){1.0f, 0.5, 1.0f};
            break;
        case PAD_SIZE_INCREASE_POWERUP:
            game->player->position.x -= game->player->size.x / 2.0f;
            game->player->size.x *= sizePowerMult;
            break;
        case CONFUSE_POWERUP:
            if (!game->postProcessor->chaos)
                game->postProcessor->confuse = true;
            break;
        case CHAOS_POWERUP:
            game->postProcessor->confuse = false;
            game->postProcessor->chaos = true;
            break;
    }
} 

void unapplyPowerUp(PowerUp *powerUp, Game *game) {
    powerUp->activated = false;
    if (IsOtherPowerUpActive(game->powerUpPointers, powerUp->type)) {
        return;
    }

    switch (powerUp->type) {
        case SPEED_POWERUP:
            game->player->velocity = scaleVec2(game->player->velocity, 1.0f / speedPowerMult);
            break;
        case STICKY_POWERUP:
            game->ball->sticky = false;
            game->player->color = (Vec3){1.0f, 1.0, 1.0f};
            break;
        case PASS_THROUGH_POWERUP:
            game->ball->passthrough = false;
            game->ball->baseObject.color = (Vec3){1.0f, 1.0f, 1.0f};
            break;
        case PAD_SIZE_INCREASE_POWERUP:
            game->player->size.x /= sizePowerMult;
            game->player->position.x += game->player->size.x / 2.0f;
            break;
        case CONFUSE_POWERUP:
                game->postProcessor->confuse = false;
            break;
        case CHAOS_POWERUP:
            game->postProcessor->chaos = false;
            break;
    }
}