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
#include "postProcessor.h"
#include "powerup.h"
#include "text.h"

const Vec2 playerSize = {225.0f, 45.0f};
const float playerVelocity = 600.0f;

const Vec2 initialBallVelocity = {255.0f, -780.0f};
const float ballRadius = 18.75f;

typedef enum {
    UP = 0b0001,
    RIGHT = 0b0010,
    DOWN = 0b0100,
    LEFT = 0b1000
} Direction; 

typedef struct {
    bool happened;
    Direction direction;
    Vec2 differenceVector;
} Collision;

void doGameCollisions(Game *game);
Collision checkCollision(BallObject *ball, GameObject *object);
bool checkCollisionSquares(GameObject *a, GameObject *b);

Game *createGame(unsigned int width, unsigned int height) {
    Game *game = malloc(sizeof(Game));

    memset(game->keys, false, sizeof(game->keys) / sizeof(bool));
    memset(game->newKeys, false, sizeof(game->newKeys) / sizeof(bool));

    game->state = GAME_MENU;
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
    game->generatedLevel = generateGameLevel(width, height / 2, game->textureManager);


    Vec2 playerPos = {(width - playerSize.x) / 2.0f, height - playerSize.y};
    game->player = createGameObject(game->textureManager->player, playerPos, playerSize);
    game->player->velocity.x = playerVelocity;

    Vec2 ballPos = {playerPos.x + playerSize.x / 2.0f - ballRadius, playerPos.y - ballRadius * 2.0f};
    game->ball = createBallObject(game->textureManager->ball, ballPos, ballRadius, initialBallVelocity);

    game->postProcessor = createPostProcessor(width, height);

    game->powerUpPointers = createList(8, sizeof(PowerUp *));

    game->textRenderer = createTextRenderer(width, height, "assets/fonts/ocraext.ttf", 36);

    game->shakeTime = 0.0f;

    return game;
}

void freeGame(Game *game) {
    glDeleteProgram(game->spriteShader);

    freeTextRenderer(game->textRenderer);

    freePostProcessor(game->postProcessor);
    free(game->ball);
    free(game->player);
    free(game->projectionMatrix);
    freeSpriteRenderer(game->spriteRenderer);
    freeTextureManager(game->textureManager);
    for (int i = 0; i < sizeof(game->levels) / sizeof(GameLevel *); i++) {
        freeGameLevel(game->levels[i]);
    }
    freeGameLevel(game->generatedLevel);

    for (int i = 0; i < game->powerUpPointers->count; i++) {
        free(((PowerUp **)(game->powerUpPointers->data))[i]);
    }
    freeList(game->powerUpPointers);

    free(game);
}

void processGameInput(Game *game, float deltaTime) {
    if (game->state != GAME_ACTIVE) {
        if (game->keys[GLFW_KEY_SPACE])
            game->state = GAME_ACTIVE;
        if (game->keys[GLFW_KEY_W] || game->keys[GLFW_KEY_S]) {
            if (game->newKeys[GLFW_KEY_W])
                game->currentLevel++;
            if (game->newKeys[GLFW_KEY_S])
                game->currentLevel += sizeof(game->levels) / sizeof(GameLevel);
            game->currentLevel %= sizeof(game->levels) / sizeof(GameLevel) + 1;
            game->state = GAME_MENU;
        }
    }
    if (game->state == GAME_ACTIVE) {
        float velocity = game->player->velocity.x * deltaTime;
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
}

void updatePowerUps(Game *game, float deltaTime) {
    for (int i = 0; i < game->powerUpPointers->count; i++) {
        PowerUp *powerUp = ((PowerUp **)(game->powerUpPointers->data))[i];
        if (!powerUp->baseObject.destroyed) {
            powerUp->baseObject.position = vec2Add(powerUp->baseObject.position, scaleVec2(powerUp->baseObject.velocity, deltaTime));
        }
        if (powerUp->activated) {
            powerUp->duration -= deltaTime;
            if (powerUp->duration < 0.0f) {
                unapplyPowerUp(powerUp, game);
            }
        }
    }
}

void resetGame(Game *game) {
    for (int i = 0; i < game->powerUpPointers->count; i++) {
        PowerUp *powerUp = ((PowerUp **)(game->powerUpPointers->data))[i];
        if (powerUp->activated) {
            unapplyPowerUp(powerUp, game);
            free(powerUp);
        }
    }
    game->powerUpPointers->count = 0;
    if (game->currentLevel == sizeof(game->levels) / sizeof(GameLevel)) {
        freeGameLevel(game->generatedLevel);
        game->generatedLevel = generateGameLevel(game->width, game->height / 2, game->textureManager);
    }
    else {
        resetGameLevel(game->levels[game->currentLevel]);
    }
    Vec2 playerPos = {(game->width - playerSize.x) / 2.0f, game->height - playerSize.y};
    game->player->position = playerPos;
    Vec2 ballPos = {playerPos.x + playerSize.x / 2.0f - ballRadius, playerPos.y - ballRadius * 2.0f};
    resetBallObject(game->ball, ballPos, initialBallVelocity);
    game->state = GAME_MENU;
}

void updateGame(Game *game, float deltaTime) {
    if (!game->state == GAME_ACTIVE)
        return;

    ballObjectMove(game->ball, deltaTime, game->width);

    doGameCollisions(game);

    if (game->ball->baseObject.position.y >= game->height) {
        resetGame(game);
    }

    if (game->shakeTime > 0.0f) {
        game->shakeTime -= deltaTime;
        if (game->shakeTime <= 0.0f)
            game->postProcessor->shake = false;
    }

    updatePowerUps(game, deltaTime);
}

void renderGame(Game *game) {
    postProcessorBeginRender(game->postProcessor);

    drawSprite(
        game->spriteRenderer, 
        game->textureManager->background, 
        (Vec2){0.0f, 0.0f}, 
        (Vec2){game->width, game->height}, 
        0.0f, 
        (Vec3){1.0f, 1.0f, 1.0f});

    switch (game->state) {
        case GAME_WIN:
            renderText(game->textRenderer, "You won", 510.0f, game->height / 2 + 142.0f, 1.0f, (Vec3){1.0f, 1.0f, 1.0f});
        case GAME_MENU:
            if (game->currentLevel == sizeof(game->levels) / sizeof(GameLevel))
                renderText(game->textRenderer, "Randomly generated level", 330.0f, game->height / 2 + 52.5f, 1.0f, (Vec3){1.0f, 1.0f, 1.0f});
            else {
                char levelText[8];
                sprintf(levelText, "Level %d", game->currentLevel + 1);
                renderText(game->textRenderer, levelText, 510.0f, game->height / 2 + 52.5f, 1.0f, (Vec3){1.0f, 1.0f, 1.0f});
            }
            renderText(game->textRenderer, "Press SPACE to start", 375.0f, game->height / 2 + 15.0f, 1.0f, (Vec3){1.0f, 1.0f, 1.0f});
            renderText(game->textRenderer, "Press W or S to select level", 364.0f, game->height / 2 + 97.5f, 0.75f, (Vec3){0.6f, 0.6f, 0.6f});
        case GAME_ACTIVE:
            if (game->currentLevel != sizeof(game->levels) / sizeof(GameLevel)) {
                drawGameLevel(game->levels[game->currentLevel], game->spriteRenderer);
            }
            else if (game->state == GAME_ACTIVE) {
                drawGameLevel(game->generatedLevel, game->spriteRenderer);
            }
    
            for (int i = 0; i < game->powerUpPointers->count; i++) {
                PowerUp *powerUp = ((PowerUp **)(game->powerUpPointers->data))[i];
                if (powerUp->baseObject.destroyed)
                    continue;
                drawGameObject((GameObject *)powerUp, game->spriteRenderer);
            }
    
            drawGameObject(game->player, game->spriteRenderer);
            drawGameObject((GameObject *)game->ball, game->spriteRenderer);
            break;
    }
    postProcessorEndRender(game->postProcessor);
    postProcessorRender(game->postProcessor, glfwGetTime());
}


void doGameCollisions(Game *game) {
    for (int i = 0; i < game->levels[game->currentLevel]->brickGameObjectPointers->count; i++) {
        GameObject *brick = ((GameObject **)(game->levels[game->currentLevel]->brickGameObjectPointers->data))[i];
        if (brick->destroyed) {
            continue;
        }
        
        Collision collision = checkCollision(game->ball, brick);
        if (!collision.happened) {
            continue;
        }
        if (brick->isSolid) {
            game->shakeTime = 0.05f;
            game->postProcessor->shake = true;
        }
        else {
            brick->destroyed = true;
            PowerUp * powerUp = trySpawnPowerUp(brick->position, game->textureManager);
            if (powerUp) {
                addToList(game->powerUpPointers, &powerUp);
            }
            if (game->ball->passthrough)
                continue;
        }
        if (isGameLevelCompleted(
            (game->currentLevel == sizeof(game->levels) / sizeof(GameLevel)) 
            ? game->generatedLevel
            : game->levels[game->currentLevel]))
        {
            resetGame(game);
            game->state = GAME_WIN;
        }
        if (collision.direction & (LEFT | RIGHT)) {
            game->ball->baseObject.velocity.x *= -1;

            float penetration = game->ball->radius - abs(collision.differenceVector.x);
            if (collision.direction == RIGHT)
                penetration *= -1;
            game->ball->baseObject.position.x += penetration;
        }
        else {
            game->ball->baseObject.velocity.y *= -1;

            float penetration = game->ball->radius - abs(collision.differenceVector.y);
            if (collision.direction == UP)
                penetration *= -1;
            game->ball->baseObject.position.y += penetration;
        }
    }
    Collision playerCollision = checkCollision(game->ball, game->player);
    if (!game->ball->stuck && playerCollision.happened) {
        float centerBoard = game->player->position.x + game->player->size.x / 2.0f;
        float distance = (game->ball->baseObject.position.x + game->ball->radius) - centerBoard;
        float percentage = distance / (game->player->size.x / 2.0f);

        const float strength = 3.0f;
        Vec2 oldVelocity = game->ball->baseObject.velocity;
        game->ball->baseObject.velocity.x = percentage * strength * initialBallVelocity.x; 
        game->ball->baseObject.velocity.y = -1.0f * abs(game->ball->baseObject.velocity.y);
        game->ball->baseObject.velocity = scaleVec2(vec2ToNormalized(game->ball->baseObject.velocity), vec2Length(oldVelocity));

        game->ball->stuck = game->ball->sticky;
    } 

    for (int i = 0; i < game->powerUpPointers->count; i++) {
        PowerUp *powerUp = ((PowerUp **)(game->powerUpPointers->data))[i];
        if (powerUp->baseObject.destroyed) {
            continue;
        }
        if (powerUp->baseObject.position.y >= game->height) {
            powerUp->baseObject.destroyed = true;
            continue;
        }
        if (checkCollisionSquares(game->player, (GameObject *)powerUp)) {
            applyPowerUp(powerUp, game);
            powerUp->baseObject.destroyed = true;
        }
    }
}


Direction vectorDirection(Vec2 target) {
    Vec2 compass[] = {
        {0.0f, 1.0f},
        {1.0f, 0.0f},
        {0.0f, -1.0f},
        {-1.0f, 0.0f}
    };
    float max = 0.0f;
    unsigned int best_match;
    for (unsigned int i = 0; i < 4; i++)
    {
        float dot_product = vec2Dot(vec2ToNormalized(target), compass[i]);
        if (dot_product > max)
        {
            max = dot_product;
            best_match = i;
        }
    }
    return 1 << best_match;
} 

Collision checkCollision(BallObject *ball, GameObject *object) {
    Vec2 ballCenter = vec2Add(ball->baseObject.position, (Vec2){ball->radius, ball->radius});
    Vec2 brickCenter = vec2Add(object->position, scaleVec2(object->size, 0.5f));

    Vec2 difference = vec2Subtract(ballCenter, brickCenter);

    Vec2 clampedDifference = vec2Clamp(difference, scaleVec2(object->size, -0.5f), scaleVec2(object->size, 0.5f));

    Vec2 closestPoint = vec2Add(brickCenter, clampedDifference);

    Vec2 closestPointdifference = vec2Subtract(closestPoint, ballCenter);

    if (vec2Length(closestPointdifference) < ball->radius) {
        return (Collision){
            .happened = true, 
            .direction = vectorDirection(closestPointdifference), 
            .differenceVector = closestPointdifference
        };
    }
    return (Collision){.happened = false};
} 

bool checkCollisionSquares(GameObject *a, GameObject *b)
{
    bool collisionX = a->position.x + a->size.x >= b->position.x &&
        b->position.x + b->size.x >= a->position.x;

    bool collisionY = a->position.y + a->size.y >= b->position.y &&
        b->position.y + b->size.y >= a->position.y;

    return collisionX && collisionY;
} 