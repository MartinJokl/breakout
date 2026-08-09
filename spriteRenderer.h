#pragma once

#include "texture.h"
#include "vector.h"

typedef struct {
    unsigned int VAO;
    unsigned int VBO;
    unsigned int shader;
} SpriteRenderer;


SpriteRenderer *createSpriteRenderer(unsigned int shader);
void freeSpriteRenderer(SpriteRenderer *spriteRenderer);

void drawSprite(SpriteRenderer *spriteRenderer, Texture texture, Vec2 position, Vec2 size, float rotate, Vec3 color);