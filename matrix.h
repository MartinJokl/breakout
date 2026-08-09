#pragma once

#include "vector.h"


float* createIdentityMat4();

float* copyMat4(float* mat4);

void translateMat4(float* mat4, Vec3 translationVec3);
void scaleMat4(float *mat4, Vec3 scaleVec3);
void rotateMat4(float *mat4, Vec3 axisVec3, float angle);

float* createTranslateMat4(Vec3 translationVec3);
float* createScaleMat4(Vec3 scaleVec3);
float* createRotateMat4(Vec3 axisVec3, float angle);

float* multiplyMat4s(float* a, float* b);

void applyMat4Mult(float* a, float* bDestination);
void applyMat4MultAndFree(float* aToFree, float* bDestination);

float* createPerspectiveProjectionMat4(float fovDeg, float aspectRatio, float nearPlane, float farPlane);
float* createOrthographicProjectionMat4(float left, float right, float top, float bottom);

float* createlookAtMat4(Vec3 camerPos, Vec3 cameraTarget, Vec3 up);

void removeTranslationFromMat4(float* mat4);