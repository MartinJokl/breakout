#include <stdbool.h>
#include <ctype.h>
#include <stdio.h>

#include "gameLevel.h"
#include "stdlib.h"
#include "file.h"
#include "list.h"
#include "vector.h"
#include "gameObject.h"
#include "textureManager.h"

typedef enum {
    SPACE_CHAR,
    NEWLINE_CHAR,
    DIGIT_CHAR,
    UNKNOWN_CHAR
} LevelFileChar; 

void initGameLevel(GameLevel *level, List *tiles, unsigned int levelWidth, unsigned int levelHeight, TextureManager *textureManager);
Vec3 getColorVector(BrickColor color);

GameLevel *loadGameLevel(const char *file, unsigned int levelWidth, unsigned int levelHeight, TextureManager *textureManager) {
    GameLevel *level = malloc(sizeof(GameLevel));
    level->brickGameObjects = createList(16, sizeof(GameObject));

    const char *levelString = readFile(file);

    List *tiles = createList(4, sizeof(List *));

    List *firstRow = createList(4, sizeof(unsigned int));
    addToList(tiles, &firstRow);

    LevelFileChar lastChar = UNKNOWN_CHAR;
    for (int i = 0; levelString[i]; i++) {
        if (levelString[i] == ' ' && lastChar == DIGIT_CHAR) {
            lastChar = SPACE_CHAR;
        }
        else if (levelString[i] == '\n' && lastChar != NEWLINE_CHAR) {
            lastChar = NEWLINE_CHAR;

            List *row = createList(4, sizeof(unsigned int));
            addToList(tiles, &row);
        }
        else if (isdigit(levelString[i])) {
            if (lastChar == DIGIT_CHAR) {
                printf("double digit numbers are not supported in levels!");
                printf("file: %s, line: %d", file, tiles->count);
                continue;
            }
            unsigned int value = levelString[i] - '0';
            addToList(((List **)(tiles->data))[tiles->count - 1], &value);
            lastChar = DIGIT_CHAR;
        }
    }

    initGameLevel(level, tiles, levelWidth, levelHeight, textureManager);

    for (int i = 0; i < tiles->count; i++) {
       freeList(((List **)(tiles->data))[i]);
    }
    freeList(tiles);
    free((char *)levelString);

    return level;
}

void initGameLevel(GameLevel *level, List *tiles, unsigned int levelWidth, unsigned int levelHeight, TextureManager *textureManager) {
    unsigned int rowCount = tiles->count;
    unsigned int columnCount = ((List **)(tiles->data))[0]->count;

    float unitHeight = (float)levelHeight / rowCount;
    float unitWidth = (float)levelWidth / columnCount;

    for (unsigned int y = 0; y < rowCount; y++) {
        List *row = ((List **)(tiles->data))[y];
        unsigned int *rowData = (unsigned int *)row->data;
        for (unsigned int x = 0; x < columnCount; x++) {
            unsigned int value = rowData[x];
            if (value == 1) { // solid
                GameObject object = {
                    .position = {unitWidth * x, unitHeight * y},
                    .size = {unitWidth, unitHeight},
                    .color = {1.0f, 1.0f, 1.0f},
                    .texture = textureManager->solidBlock,

                    .velocity = {0.0f, 0.0f},
                    .destroyed = false,
                    .isSolid = true,
                    .rotation = 0.0f,
                };
                addToList(level->brickGameObjects, &object);
            }
            else if (value > 1) {
                GameObject object = {
                    .position = {unitWidth * x, unitHeight * y},
                    .size = {unitWidth, unitHeight},
                    .color = getColorVector(value),
                    .texture = textureManager->block,

                    .velocity = {0.0f, 0.0f},
                    .destroyed = false,
                    .isSolid = false,
                    .rotation = 0.0f,
                };
                addToList(level->brickGameObjects, &object);
            }
        }
    }
}

Vec3 getColorVector(BrickColor color) {
    switch (color)
    {
        case BRICK_COLOR_BLUE:
            return (Vec3){0.2, 0.2, 1.0};
        case BRICK_COLOR_GREEN:
            return (Vec3){0.2, 1.0, 0.2};
        case BRICK_COLOR_PINK:
            return (Vec3){0.2, 0.2, 1.0};
        case BRICK_COLOR_YELLOW:
            return (Vec3){1.0, 1.0, 0.2};
        case BRICK_COLOR_RED:
            return (Vec3){1.0, 0.2, 0.2};
        default:
            return (Vec3){1.0f, 1.0f, 1.0f};
    }
}

void freeGameLevel(GameLevel *level) {
    freeList(level->brickGameObjects);
    free(level);
}

void drawGameLevel(GameLevel *level, SpriteRenderer *renderer) {
    GameObject *bricks = (GameObject *)(level->brickGameObjects->data);
    for (int i = 0; i < level->brickGameObjects->count; i++) {
        if (bricks[i].destroyed) {
            continue;
        }
        drawGameObject(bricks[i], renderer);
    }
}


bool isGameLevelCompleted() {

}