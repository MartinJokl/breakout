#pragma once

#include "texture.h"

typedef struct {
    Texture solidBlock;
    Texture block;
    Texture background;
    Texture player;
    Texture ball;

    Texture speedPowerUp;
    Texture stickyPowerUp;
    Texture passThroughPowerUp;
    Texture padSizeIncreasePowerUp;
    Texture confusePowerUp;
    Texture chaosPowerUp;
} TextureManager;

TextureManager *createTextureManager(unsigned int shader);
void freeTextureManager(TextureManager *manager);