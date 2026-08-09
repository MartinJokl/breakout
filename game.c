#include "stdlib.h"

#include "game.h"
#include "spriteRenderer.h"
#include "texture.h"
#include "matrix.h"
#include "shader.h"

Game *createGame(unsigned int width, unsigned int height) {
    Game *game = malloc(sizeof(Game));

    game->state = GAME_MENU;
    game->width = width;
    game->height = height;

    game->projectionMatrix = createOrthographicProjectionMat4(0.0f, width, 0.0f, height);
    game->spriteShader = createShaderProgram("shaders/sprite.vert", NULL, "shaders/sprite.frag");
    glUseProgram(game->spriteShader);
    glUniformMatrix4fv(glGetUniformLocation(game->spriteShader, "projection"), 1, GL_FALSE, game->projectionMatrix);
    game->spriteRenderer = createSpriteRenderer(game->spriteShader);
    game->face = createTexture("assets/awesomeface.png", GL_RGBA, game->spriteShader, 0, "image");

    return game;
}

void freeGame(Game *game) {
    glDeleteProgram(game->spriteShader);
    glDeleteTextures(1, &game->face.id);

    free(game->projectionMatrix);
    freeSpriteRenderer(game->spriteRenderer);

    free(game);
}

void processGameInput(Game *game, float deltaTime) {

}
void updateGame(Game *game, float deltaTime) {

}
void renderGame(Game *game) {
    drawSprite(game->spriteRenderer, game->face, (Vec2){200.0f, 200.0f}, (Vec2){300.0f, 400.0f}, 3.14 / 4.0f, (Vec3){0.0f, 1.0f, 0.0f});
}