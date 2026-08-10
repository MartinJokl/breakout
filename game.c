#include "stdlib.h"
#include <glad/glad.h>
#include "GLFW/glfw3.h"

#include "game.h"
#include "spriteRenderer.h"
#include "texture.h"
#include "matrix.h"
#include "shader.h"
#include "textureManager.h"
#include "gameLevel.h"
#include "gameObject.h"
#include "ballObject.h"

const Vec2 playerSize = {150.0f, 30.0f};
const float playerVelocity = 500.0f;

const Vec2 initialBallVelocity = {100.0f, -350.0f};
const float ballRadius = 12.5f;

Game *createGame(unsigned int width, unsigned int height) {
    Game *game = malloc(sizeof(Game));

    for (int i = 0; i < sizeof(game->keys) / sizeof(bool); i++) {
        game->keys[i] = false;
    }
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


    Vec2 playerPos = {
        (width - playerSize.x) / 2.0f, 
        height - playerSize.y
    };
    game->player = createGameObject(game->textureManager->player, playerPos, playerSize);

    Vec2 ballPos = {playerPos.x + playerSize.x / 2.0f - ballRadius, playerPos.y - ballRadius * 2.0f};
    game->ball = createBallObject(game->textureManager->ball, ballPos, ballRadius, initialBallVelocity);

    return game;
}

void freeGame(Game *game) {
    glDeleteProgram(game->spriteShader);

    free(game->ball);
    free(game->player);
    free(game->projectionMatrix);
    freeSpriteRenderer(game->spriteRenderer);
    freeTextureManager(game->textureManager);
    for (int i = 0; i < sizeof(game->levels) / sizeof(GameLevel *); i++) {
        freeGameLevel(game->levels[i]);
    }

    free(game);
}

void processGameInput(Game *game, float deltaTime) {
    if (game->state != GAME_ACTIVE) {
        return;
    }

    float velocity = playerVelocity * deltaTime;
    if (game->keys[GLFW_KEY_A]) {
        if (game->player->position.x >= 0.0f) {
            game->player->position.x -= velocity;
            if (game->ball->stuck) {
                game->ball->baseObject.position.x -= velocity;
            }
        }
    }
    if (game->keys[GLFW_KEY_D]) {
        if (game->player->position.x <= game->width - game->player->size.x) {
            game->player->position.x += velocity;
            if (game->ball->stuck) {
                game->ball->baseObject.position.x += velocity;
            }
        }
    }
    if (game->keys[GLFW_KEY_SPACE])
        game->ball->stuck = false;
}

void updateGame(Game *game, float deltaTime) {
    ballObjectMove(game->ball, deltaTime, game->width);
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

        drawGameObject(game->player, game->spriteRenderer);
        drawGameObject((GameObject *)game->ball, game->spriteRenderer);
        
        drawGameLevel(game->levels[game->currentLevel], game->spriteRenderer);
    }
}