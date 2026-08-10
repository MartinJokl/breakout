#pragma once
#include <glad/glad.h>

typedef struct {
    unsigned int id;
    unsigned int unit;
} Texture;

Texture createTexture(char* path, unsigned int shader, unsigned int unit, char* uniformName);
void useTexture(Texture texture);