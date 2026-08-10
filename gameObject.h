#pragma once

#include <stdbool.h>

#include "vector.h"
#include "texture.h"
#include "spriteRenderer.h"

typedef struct {
    Texture texture;
    Vec2 position, size, velocity;
    Vec3 color;

    float rotation;
    bool isSolid;
    bool destroyed;

} GameObject;

void drawGameObject(GameObject *object, SpriteRenderer *renderer);

GameObject *createGameObject(Texture texture, Vec2 position, Vec2 size);