#pragma once

#include <stdbool.h>

typedef enum {
    GAME_ACTIVE,
    GAME_MENU,
    GAME_WIN
} GameState;

typedef struct {
    GameState state;
    bool keys[1024];
    unsigned int width, height;
} Game;

Game *createGame(unsigned int width, unsigned int height);
void freeGame(Game *game);

void processGameInput(Game *game, float deltaTime);
void updateGame(Game *game, float deltaTime);
void renderGame(Game *game);