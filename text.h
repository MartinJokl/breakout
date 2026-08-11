#pragma once

#include "vector.h"
#include <ft2build.h>
#include FT_FREETYPE_H 

typedef struct {
    unsigned int textureID;

    unsigned int sizeX;
    unsigned int sizeY;

    unsigned int bearingX;
    unsigned int bearingY;

    unsigned int advance;
} Character;

typedef struct {
    Character characters[128];
    FT_Library ft;
    float *projection;
    unsigned int VAO;
    unsigned int VBO;
    unsigned int shader;
} TextRenderer;

TextRenderer *createTextRenderer(unsigned int width, unsigned int height, char *fontPath, unsigned int size);
void freeTextRenderer(TextRenderer *textRenderer);

void renderText(TextRenderer *textRenderer, char *text, float x, float y, float scale, Vec3 color);