#include "stdlib.h"

#include "game.h"
#include "spriteRenderer.h"
#include "texture.h"
#include "matrix.h"
#include "shader.h"
#include "textureManager.h"
#include "gameLevel.h"

Game *createGame(unsigned int width, unsigned int height) {
    Game *game = malloc(sizeof(Game));

    game->state = GAME_ACTIVE;
    game->width = width;
    game->height = height;

    game->projectionMatrix = createOrthographicProjectionMat4(0.0f, width, 0.0f, height);
    game->spriteShader = createShaderProgram("shaders/sprite.vert", NULL, "shaders/sprite.frag");
    glUseProgram(game->spriteShader);
    glUniformMatrix4fv(glGetUniformLocation(game->spriteShader, "projection"), 1, GL_FALSE, game->projectionMatrix);
    game->spriteRenderer = createSpriteRenderer(game->spriteShader);
    game->textureManager = createTextureManager(game->spriteShader);

    game->levels[0] = loadGameLevel("assets/levels/1.txt", width, height / 2, game->textureManager);
    game->levels[1] = loadGameLevel("assets/levels/2.txt", width, height / 2, game->textureManager);
    game->levels[2] = loadGameLevel("assets/levels/3.txt", width, height / 2, game->textureManager);
    game->levels[3] = loadGameLevel("assets/levels/4.txt", width, height / 2, game->textureManager);
    game->currentLevel = 0;

    return game;
}

void freeGame(Game *game) {
    glDeleteProgram(game->spriteShader);

    free(game->projectionMatrix);
    freeSpriteRenderer(game->spriteRenderer);
    freeTextureManager(game->textureManager);
    for (int i = 0; i < sizeof(game->levels) / sizeof(GameLevel *); i++) {
        freeGameLevel(game->levels[i]);
    }

    free(game);
}

void processGameInput(Game *game, float deltaTime) {

}
void updateGame(Game *game, float deltaTime) {

}
void renderGame(Game *game) {
    if (game->state == GAME_ACTIVE) {
        drawSprite(
            game->spriteRenderer, 
            game->textureManager->background, 
            (Vec2){0.0f, 0.0f}, 
            (Vec2){game->width, game->height}, 
            0.0f, 
            (Vec3){1.0f, 1.0f, 1.0f});
        
        drawGameLevel(game->levels[game->currentLevel], game->spriteRenderer);
    }
}