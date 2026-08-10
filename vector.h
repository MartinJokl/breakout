#pragma once

typedef struct {
    float x;
    float y;
    float z;
} Vec3;

typedef struct {
    float x;
    float y;
} Vec2;


Vec3 createNormalizedVec3(float x, float y, float z);

Vec3 vec3ToNormalized(Vec3 vec3);
Vec3 scaleVec3(Vec3 vec3, float scale);

float vec3Length(Vec3 vec3);

Vec3 MultiplyVec3WithMat4(Vec3 vec3, float* mat4);

Vec3 vec3Add(Vec3 a, Vec3 b);
Vec3 vec3Subtract(Vec3 a, Vec3 b);

float* indexVec3(Vec3* vec3, int index);

Vec3 vec3Cross(Vec3 a, Vec3 b);
float vec3Dot(Vec3 a, Vec3 b);


Vec2 vec2ToNormalized(Vec2 vec2);
Vec2 scaleVec2(Vec2 vec2, float scale);

float vec2Length(Vec2 vec2);

Vec2 vec2Add(Vec2 a, Vec2 b);
Vec2 vec2Subtract(Vec2 a, Vec2 b);

Vec2 vec2Clamp(Vec2 vec2, Vec2 min, Vec2 max) {
    return (Vec2){
        .x = clamp(vec2.x, min.x, max.x),
        .y = clamp(vec2.y, min.y, max.y)
    };
}