#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "matrix.h"
#include "vector.h"

float* createIdentityMat4() {
    float* mat4 = calloc(16, sizeof(float));
    if (mat4 == NULL) {
        printf("Allocation failed in createIdentityMat4()");
        return NULL;
    }
    for (int row = 0; row < 4; row++) {
        int col = row;
        mat4[col * 4 + row] = 1;
    }
    return mat4;
}
float* createTranslateMat4(Vec3 translationVec3) {
    float* mat4 = createIdentityMat4();
    int col = 3; 
    for (int row = 0; row < 3; row++) {
        mat4[col * 4 + row] = *indexVec3(&translationVec3, row);
    }
    return mat4;
}
float* createScaleMat4(Vec3 scaleVec3) {
    float* mat4 = createIdentityMat4();
    for (int row = 0; row < 3; row++) {
        int col = row;
        mat4[col * 4 + row] = *indexVec3(&scaleVec3, row);
    }
    return mat4;
}
float* createRotateMat4(Vec3 axisVec3, float angle) {
    float* mat4 = createIdentityMat4();
    Vec3 normalizedAxis = vec3ToNormalized(axisVec3);
    float x = normalizedAxis.x, y = normalizedAxis.y, z = normalizedAxis.z, a = angle;
    //  col     row
    mat4[0 * 4 + 0] = cos(a) + x * x * (1 - cos(a));
    mat4[0 * 4 + 1] = y * x * (1 - cos(a)) + z * sin(a);
    mat4[0 * 4 + 2] = z * x * (1 - cos(a)) - y * sin(a);

    mat4[1 * 4 + 0] = x * y * (1 - cos(a)) - z * sin(a);
    mat4[1 * 4 + 1] = cos(a) + y * y * (1 - cos(a));
    mat4[1 * 4 + 2] = z * y * (1 - cos(a)) + x * sin(a);

    mat4[2 * 4 + 0] = x * z * (1 - cos(a)) + y * sin(a);
    mat4[2 * 4 + 1] = y * z * (1 - cos(a)) - x * (sin(a));
    mat4[2 * 4 + 2] = cos(a) + z * z * (1 - cos(a));
    return mat4;
}
float* copyMat4(float* mat4) {
    float* newMat4 = malloc(16 * sizeof(float));
    if (newMat4 == NULL) {
        printf("Allocation failed in copyMat4()");
        return NULL;
    }
    memcpy(newMat4, mat4, 16 * sizeof(float));
    return newMat4;
}

void translateMat4(float* mat4, Vec3 translationVec3) {
    float* translateMat4 = createTranslateMat4(translationVec3);
    applyMat4MultAndFree(mat4, translateMat4);
}

void scaleMat4(float *mat4, Vec3 scaleVec3) {
    float* scaleMat4 = createScaleMat4(scaleVec3);
    applyMat4MultAndFree(mat4, scaleMat4);
}

void rotateMat4(float *mat4, Vec3 axisVec3, float angle) {
    float* rotateMat4 = createRotateMat4(axisVec3, angle);
    applyMat4MultAndFree(mat4, rotateMat4);
}

void applyMat4Mult(float* bDestination, float* a) {
    float* result = multiplyMat4s(bDestination, a);
    memcpy(bDestination, result, 16 * sizeof(float));
    free(result);
}

void applyMat4MultAndFree(float* bDestination, float* aToFree) {
    applyMat4Mult(bDestination, aToFree);
    free(aToFree);
}

float* multiplyMat4s(float* a, float* b) {
    float* result = calloc(16, sizeof(float));
    for (int col = 0; col < 4; col++) {
        for (int row = 0; row < 4; row++) {
            for (int i = 0; i < 4; i++) {
                result[col * 4 + row] += a[i * 4 + row] * b[col * 4 + i];
            }
        }
    }
    return result;
}

float* createPerspectiveProjectionMat4(float fovDeg, float aspectRatio, float nearPlane, float farPlane) {
    const double PI = 3.14159265358979323846;
    float fovRadians = fovDeg / 180.0f * PI;
    float right = tan(fovRadians / 2.0f) * nearPlane;
    float top = right / aspectRatio;

    float* projectionMatrix = calloc(16, sizeof(float));
    //              col     row
    projectionMatrix[0 * 4 + 0] = nearPlane / right;
    projectionMatrix[1 * 4 + 1] = nearPlane / top;
    projectionMatrix[2 * 4 + 2] = -(farPlane + nearPlane) / (farPlane - nearPlane);
    projectionMatrix[3 * 4 + 2] = -(2.0f * farPlane * nearPlane) / (farPlane - nearPlane);
    projectionMatrix[2 * 4 + 3] = -1.0f;

    return projectionMatrix;
}

float* createOrthographicProjectionMat4(float left, float right, float top, float bottom) {
    float* projectionMatrix = createIdentityMat4();
    //              col     row
    projectionMatrix[0 * 4 + 0] = 2.0f / (right - left);
    projectionMatrix[1 * 4 + 1] = 2.0f / (top - bottom);
    projectionMatrix[3 * 4 + 0] = -((right + left) / (right - left));
    projectionMatrix[3 * 4 + 1] = -((top + bottom) / (top - bottom));

    return projectionMatrix;
}

float* createlookAtMat4(Vec3 cameraPos, Vec3 cameraTarget, Vec3 up) {
    Vec3 cameraDirection = vec3Subtract(cameraTarget, cameraPos);
    Vec3 cameraRight = vec3ToNormalized(vec3Cross(cameraDirection, up));
    Vec3 cameraUp = vec3ToNormalized(vec3Cross(cameraRight, cameraDirection));
    float* translateMatrix = createTranslateMat4(scaleVec3(cameraPos, -1.0f));

    float* rotateMatrix = createIdentityMat4();
    //          col     row
    rotateMatrix[0 * 4 + 0] = cameraRight.x;
    rotateMatrix[0 * 4 + 1] = cameraUp.x;
    rotateMatrix[0 * 4 + 2] = -cameraDirection.x;

    rotateMatrix[1 * 4 + 0] = cameraRight.y;
    rotateMatrix[1 * 4 + 1] = cameraUp.y;
    rotateMatrix[1 * 4 + 2] = -cameraDirection.y;

    rotateMatrix[2 * 4 + 0] = cameraRight.z;
    rotateMatrix[2 * 4 + 1] = cameraUp.z;
    rotateMatrix[2 * 4 + 2] = -cameraDirection.z;

    float* result = multiplyMat4s(rotateMatrix, translateMatrix);
    free(rotateMatrix);
    free(translateMatrix);
    return result;
}

void removeTranslationFromMat4(float* mat4) {
    //  col     row
    mat4[3 * 4 + 0] = 0;
    mat4[3 * 4 + 1] = 0;
    mat4[3 * 4 + 2] = 0;
}