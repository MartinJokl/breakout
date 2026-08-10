#pragma once

#include "gameObject.h"

typedef struct {
    GameObject baseObject;
    bool stuck;
} BallObject;

BallObject *createBallObject(Texture texture, Vec2 position, float radius, Vec2 velocity);

Vec2 ballObjectMove(BallObject *ball, float deltaTime, unsigned int windowWidth);
void resetBallObject(BallObject *ball);