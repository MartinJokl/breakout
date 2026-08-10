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

const Vec2 initialBallVelocity = {150.0f, -450.0f};
const float ballRadius = 12.5f;

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


    Vec2 playerPos = {(width - playerSize.x) / 2.0f, height - playerSize.y};
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

    doGameCollisions(game);

    if (game->ball->baseObject.position.y >= game->height)
    {
        resetGameLevel(game->levels[game->currentLevel]);
        Vec2 playerPos = {(game->width - playerSize.x) / 2.0f, game->height - playerSize.y};
        game->player->position = playerPos;
        Vec2 ballPos = {playerPos.x + playerSize.x / 2.0f - ballRadius, playerPos.y - ballRadius * 2.0f};
        resetBallObject(game->ball, ballPos, initialBallVelocity);
    }
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
        if (!brick->isSolid) {
            brick->destroyed = true;
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
    } 

}


Direction vectorDirection(Vec2 target)
{
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