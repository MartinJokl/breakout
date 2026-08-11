#include <glad/glad.h>
#include <stdlib.h>
#include <ft2build.h>
#include FT_FREETYPE_H 

#include "text.h"
#include "matrix.h"
#include "shader.h"
#include "vector.h"

void loadFont(TextRenderer *textRenderer, char *path, unsigned int size);

TextRenderer *createTextRenderer(unsigned int width, unsigned int height, char *fontPath, unsigned int fontSize) {
    TextRenderer *textRenderer = malloc(sizeof(TextRenderer));
    if (FT_Init_FreeType(&textRenderer->ft))
    {
        free(textRenderer);
        printf("Could not initialize FreeType Library\n");
        return NULL;
    }

    textRenderer->projection = createOrthographicProjectionMat4(0.0f, width, 0.0f, height);
    glGenVertexArrays(1, &textRenderer->VAO);
    glGenBuffers(1, &textRenderer->VBO);
    glBindVertexArray(textRenderer->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, textRenderer->VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0); 

    textRenderer->shader = createShaderProgram("shaders/character.vert", NULL, "shaders/character.frag");
    glUseProgram(textRenderer->shader); 
    glUniformMatrix4fv(glGetUniformLocation(textRenderer->shader, "projection"), 1, GL_FALSE, textRenderer->projection);

    loadFont(textRenderer, fontPath, fontSize);

    return textRenderer;
}

void freeTextRenderer(TextRenderer *textRenderer) {
    for (int i = 0; i < sizeof(textRenderer->characters) / sizeof(Character); i++) {
        glDeleteTextures(1, &textRenderer->characters[i].textureID);
    }
    glDeleteVertexArrays(1, &textRenderer->VAO);
    glDeleteBuffers(1, &textRenderer->VBO);
    glDeleteProgram(textRenderer->shader);
    free(textRenderer->projection);
    FT_Done_FreeType(textRenderer->ft);
    free(textRenderer);
}

void renderText(TextRenderer *textRenderer, char *text, float x, float y, float scale, Vec3 color) {
    glUseProgram(textRenderer->shader);
    glUniform3f(glGetUniformLocation(textRenderer->shader, "textColor"), color.x, color.y, color.z);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(textRenderer->VAO);
    
    int textLength = strlen(text);
    for (int i = 0; i < textLength; i++)
    {
        Character ch = textRenderer->characters[text[i]];
        
        float xpos = x + ch.bearingX * scale;
        float ypos = y + (textRenderer->characters['H'].bearingY - ch.bearingY) * scale;  
        
        float w = ch.sizeX * scale;
        float h = ch.sizeY * scale;
        
        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 0.0f },
            { xpos,     ypos,       0.0f, 0.0f },

            { xpos,     ypos + h,   0.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 0.0f }
        };
        
        glBindTexture(GL_TEXTURE_2D, ch.textureID);
        
        glBindBuffer(GL_ARRAY_BUFFER, textRenderer->VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); 
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // advance is number of 1/64 pixels)
        x += (ch.advance >> 6) * scale;
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void loadFont(TextRenderer *textRenderer, char *path, unsigned int size) {
    FT_Face face;
    if (FT_New_Face(textRenderer->ft, path, 0, &face))
    {
        printf("Failed to load font\n");  
        return;
    }
    FT_Set_Pixel_Sizes(face, 0, size); 
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction
    for (unsigned char c = 0; c < sizeof(textRenderer->characters) / sizeof(Character); c++)
    {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            printf("Failed to load Glyph char: %d\n", c);
            continue;
        }

        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer
        );
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        textRenderer->characters[c] = (Character){
            .textureID = texture, 
            .sizeX = face->glyph->bitmap.width, 
            .sizeY = face->glyph->bitmap.rows,
            .bearingX = face->glyph->bitmap_left, 
            .bearingY = face->glyph->bitmap_top,
            .advance = face->glyph->advance.x
        };
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    FT_Done_Face(face);
}