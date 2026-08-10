
#include "textureManager.h"
#include "texture.h"
#include "stdlib.h"

TextureManager *createTextureManager(unsigned int shader) {
    TextureManager *manager = malloc(sizeof(TextureManager));

    manager->block = createTexture("assets/sprites/block.png", shader, 0, "image");
    manager->solidBlock = createTexture("assets/sprites/solidBlock.png", shader, 0, "image");
    manager->background = createTexture("assets/sprites/background.jpg", shader, 0, "image");
    manager->player = createTexture("assets/sprites/paddle.png", shader, 0, "image");
    manager->ball = createTexture("assets/sprites/awesomeface.png", shader, 0, "image");

    return manager;
}

void freeTextureManager(TextureManager *manager) {
    glDeleteTextures(1, &manager->block.id);
    glDeleteTextures(1, &manager->solidBlock.id);
    glDeleteTextures(1, &manager->background.id);
    glDeleteTextures(1, &manager->player.id);
    glDeleteTextures(1, &manager->ball.id);

    free(manager);
}
