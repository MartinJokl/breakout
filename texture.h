#pragma once
#include <glad/glad.h>

typedef struct {
    unsigned int id;
    unsigned int unit;
} Texture;

Texture createTexture(char* path, GLenum format, unsigned int shader, unsigned int unit, char* uniformName);
void useTexture(Texture texture);