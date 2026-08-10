#pragma once

#include "texture.h"

typedef struct {
    Texture solidBlock;
    Texture block;
    Texture background;
} TextureManager;

TextureManager *createTextureManager(unsigned int shader);
void freeTextureManager(TextureManager *manager);