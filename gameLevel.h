#pragma once

#include "gameObject.h"
#include "stdbool.h"
#include "textureManager.h"
#include "list.h"

typedef enum {
    BRICK_COLOR_RED = 2,
    BRICK_COLOR_GREEN,
    BRICK_COLOR_BLUE,
    BRICK_COLOR_YELLOW,
    BRICK_COLOR_PINK,
    BRICK_COLOR_COUNT
} BrickColor;

typedef struct {
    List *brickGameObjectPointers;
} GameLevel;

GameLevel *loadGameLevel(const char *file, unsigned int levelWidth, unsigned int levelHeight, TextureManager *textureManager);
GameLevel *generateGameLevel(unsigned int levelWidth, unsigned int levelHeight, TextureManager *textureManager);
void freeGameLevel(GameLevel *level);

void resetGameLevel(GameLevel *level);

void drawGameLevel(GameLevel *level, SpriteRenderer *renderer);
bool isGameLevelCompleted(GameLevel *level);
