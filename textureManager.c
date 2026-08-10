
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

    manager->speedPowerUp = createTexture("assets/sprites/powerups/speed.png", shader, 0, "image");
    manager->stickyPowerUp = createTexture("assets/sprites/powerups/sticky.png", shader, 0, "image");
    manager->passThroughPowerUp = createTexture("assets/sprites/powerups/passthrough.png", shader, 0, "image");
    manager->padSizeIncreasePowerUp = createTexture("assets/sprites/powerups/increase.png", shader, 0, "image");
    manager->confusePowerUp = createTexture("assets/sprites/powerups/confuse.png", shader, 0, "image");
    manager->chaosPowerUp = createTexture("assets/sprites/powerups/chaos.png", shader, 0, "image");

    return manager;
}

void freeTextureManager(TextureManager *manager) {
    glDeleteTextures(1, &manager->block.id);
    glDeleteTextures(1, &manager->solidBlock.id);
    glDeleteTextures(1, &manager->background.id);
    glDeleteTextures(1, &manager->player.id);
    glDeleteTextures(1, &manager->ball.id);

    glDeleteTextures(1, &manager->speedPowerUp.id);
    glDeleteTextures(1, &manager->stickyPowerUp.id);
    glDeleteTextures(1, &manager->passThroughPowerUp.id);
    glDeleteTextures(1, &manager->padSizeIncreasePowerUp.id);
    glDeleteTextures(1, &manager->confusePowerUp.id);
    glDeleteTextures(1, &manager->chaosPowerUp.id);

    free(manager);
}
