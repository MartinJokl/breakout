#pragma once

#include "gameObject.h"
#include "stdbool.h"
#include "textureManager.h"
#include "list.h"

typedef enum {
    BRICK_COLOR_RED = 2,
    BRICK_COLOR_GREEN = 3,
    BRICK_COLOR_BLUE = 4,
    BRICK_COLOR_YELLOW = 5,
    BRICK_COLOR_PINK = 6,
} BrickColor;

typedef struct {
    List *brickGameObjectPointers;
} GameLevel;

GameLevel *loadGameLevel(const char *file, unsigned int levelWidth, unsigned int levelHeight, TextureManager *textureManager);
void freeGameLevel(GameLevel *level);

void drawGameLevel(GameLevel *level, SpriteRenderer *renderer);
bool isGameLevelCompleted();
