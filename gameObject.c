#include <stdlib.h>

#include "gameObject.h"
#include "spriteRenderer.h"

void drawGameObject(GameObject *object, SpriteRenderer *renderer) {
    drawSprite(renderer, object->texture, object->position, object->size, object->rotation, object->color);
}

GameObject *createGameObject(Texture texture, Vec2 position, Vec2 size) {
    GameObject *object = malloc(sizeof(GameObject));

    object->position = position;
    object->size = size;
    object->color = (Vec3){1.0f, 1.0f, 1.0f};
    object->texture = texture;

    object->velocity = (Vec2){0.0f, 0.0f};
    object->destroyed = false;
    object->isSolid = true;
    object->rotation = 0.0f;

    return object;
}