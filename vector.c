#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "vector.h"

Vec3 createNormalizedVec3(float x, float y, float z) {
    return vec3ToNormalized((Vec3){
        .x = x,
        .y = y,
        .z = z
    });
}

Vec3 vec3ToNormalized(Vec3 vec3) {
    float length = vec3Length(vec3);
    return scaleVec3(vec3, 1.0f / length);
}

Vec3 scaleVec3(Vec3 vec3, float scale) {
    return (Vec3){
        .x = vec3.x * scale,
        .y = vec3.y * scale,
        .z = vec3.z * scale,
    };
}

float vec3Length(Vec3 vec3) {
    return sqrt(pow(vec3.x, 2) + pow(vec3.y, 2) + pow(vec3.z, 2));
}

Vec3 MultiplyVec3WithMat4(Vec3 vec3, float* mat4) {
    Vec3 result = {0, 0, 0};
    for (int col = 0; col < 4; col++) {
        float vectorValue = col <= 2 ? *indexVec3(&vec3, col) : 1;
        for (int row = 0; row < 3; row++) {
            *indexVec3(&result, row) += mat4[col * 4 + row] * vectorValue;
        }
    }
    return result;
}

Vec3 vec3Add(Vec3 a, Vec3 b) {
    return (Vec3){
        .x = a.x + b.x,
        .y = a.y + b.y,
        .z = a.z + b.z
    };
}
Vec3 vec3Subtract(Vec3 a, Vec3 b) {
    return vec3Add(a, scaleVec3(b, -1.0f));
}

float* indexVec3(Vec3* vec3, int index) {
    switch (index)
    {
        case 0:
            return &(vec3->x);
        case 1:
            return &(vec3->y);
        case 2:
            return &(vec3->z);
    }
}

Vec3 vec3Cross(Vec3 a, Vec3 b) {
    return (Vec3){
        .x = a.y * b.z - a.z * b.y,
        .y = a.z * b.x - a.x * b.z,
        .z = a.x * b.y - a.y * b.x
    };
}
float vec3Dot(Vec3 a, Vec3 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
    
}

Vec2 vec2ToNormalized(Vec2 vec2) {
    float length = vec2Length(vec2);
    return scaleVec2(vec2, 1.0f / length);
}

Vec2 scaleVec2(Vec2 vec2, float scale) {
    return (Vec2){
        .x = vec2.x * scale,
        .y = vec2.y * scale,
    };
}

float vec2Length(Vec2 vec2) {
    return sqrt(pow(vec2.x, 2) + pow(vec2.y, 2));
}

Vec2 vec2Add(Vec2 a, Vec2 b) {
    return (Vec2){
        .x = a.x + b.x,
        .y = a.y + b.y
    };
}

Vec2 vec2Subtract(Vec2 a, Vec2 b) {
    return vec2Add(a, scaleVec2(b, -1.0f));
}

float clamp(float value, float min, float max) {
    if (value < min) {
        value = min;
    }
    else if (value > max) {
        value = max;
    }
    return value;
}

Vec2 vec2Clamp(Vec2 vec2, Vec2 min, Vec2 max) {
    return (Vec2){
        .x = clamp(vec2.x, min.x, max.x),
        .y = clamp(vec2.y, min.y, max.y)
    };
}