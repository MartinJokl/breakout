#pragma once

#include "vector.h"
#include "gameObject.h"
#include "texture.h"
#include "textureManager.h"
#include "game.h"

typedef enum {
    SPEED_POWERUP,
    STICKY_POWERUP,
    PASS_THROUGH_POWERUP,
    PAD_SIZE_INCREASE_POWERUP,
    CONFUSE_POWERUP,
    CHAOS_POWERUP,
    POWERUP_TYPE_COUNT
} PowerUpType;

typedef struct {
    GameObject baseObject;
    PowerUpType type;
    float duration;
    bool activated;
} PowerUp;


PowerUp *createPowerUp(PowerUpType type, Vec3 color, float duration, Vec2 position, Texture texture);
PowerUp *trySpawnPowerUp(Vec2 position, TextureManager *textureManager);
void applyPowerUp(PowerUp *powerUp, Game *game);
void unapplyPowerUp(PowerUp *powerUp, Game *game);