#pragma once

#include <stdbool.h>

#include "spriteRenderer.h"
#include "textureManager.h"
#include "texture.h"
#include "gameLevel.h"

typedef enum {
    GAME_ACTIVE,
    GAME_MENU,
    GAME_WIN
} GameState;

typedef struct {
    bool keys[1024];
    GameState state;
    unsigned int width, height;
    unsigned int spriteShader;
    float *projectionMatrix;
    SpriteRenderer *spriteRenderer;
    TextureManager *textureManager;
    GameLevel *levels[4];
    unsigned int currentLevel;
} Game;

Game *createGame(unsigned int width, unsigned int height);
void freeGame(Game *game);

void processGameInput(Game *game, float deltaTime);
void updateGame(Game *game, float deltaTime);
void renderGame(Game *game);