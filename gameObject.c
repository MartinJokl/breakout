
#include "gameObject.h"
#include "spriteRenderer.h"

void drawGameObject(GameObject object, SpriteRenderer *renderer) {
    drawSprite(renderer, object.texture, object.position, object.size, object.rotation, object.color);
}