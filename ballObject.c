#include <stdlib.h>

#include "ballObject.h"
#include "gameObject.h"

BallObject *createBallObject(Texture texture, Vec2 position, float radius, Vec2 velocity) {
    BallObject *ball = malloc(sizeof(BallObject));

    ball->baseObject.position = position;
    ball->baseObject.size = (Vec2){radius * 2, radius * 2};
    ball->baseObject.color = (Vec3){1.0f, 1.0f, 1.0f};
    ball->baseObject.texture = texture;

    ball->baseObject.velocity = velocity;
    ball->baseObject.destroyed = false;
    ball->baseObject.isSolid = true;
    ball->baseObject.rotation = 0.0f;

    ball->stuck = true;

    return ball;
}

Vec2 ballObjectMove(BallObject *ball, float deltaTime, unsigned int windowWidth) {
    if (ball->stuck) {
        return ball->baseObject.position;
    }

    ball->baseObject.position.x += ball->baseObject.velocity.x * deltaTime;
    ball->baseObject.position.y += ball->baseObject.velocity.y * deltaTime;

    if (ball->baseObject.position.x <= 0.0f)
    {
        ball->baseObject.velocity.x *= -1;
        ball->baseObject.position.x = 0.0f;
    }
    else if (ball->baseObject.position.x + ball->baseObject.size.x >= windowWidth)
    {
        ball->baseObject.velocity.x *= -1;
        ball->baseObject.position.x = windowWidth - ball->baseObject.size.x;
    }
    if (ball->baseObject.position.y <= 0.0f)
    {
        ball->baseObject.velocity.y *= -1;
        ball->baseObject.position.y = 0.0f;
    }
    
    return ball->baseObject.position;
}

void resetBallObject(BallObject *ball) {

}
