#include <stdlib.h>
#include <glad/glad.h>

#include "spriteRenderer.h"
#include "matrix.h"

SpriteRenderer *createSpriteRenderer(unsigned int shader) {
    SpriteRenderer *spriteRenderer = malloc(sizeof(SpriteRenderer));
    spriteRenderer->shader = shader;

    float vertices[] = { 
        // pos      // tex
        0.0f, 0.0f, 0.0f, 0.0f, 
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 1.0f, 1.0f
    };

    glGenVertexArrays(1, &spriteRenderer->VAO);
    glGenBuffers(1, &spriteRenderer->VBO);
    
    glBindBuffer(GL_ARRAY_BUFFER, spriteRenderer->VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(spriteRenderer->VAO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);  
    glBindVertexArray(0);

    return spriteRenderer;
}

void freeSpriteRenderer(SpriteRenderer *spriteRenderer) {
    glDeleteBuffers(1, &spriteRenderer->VBO);
    glDeleteVertexArrays(1, &spriteRenderer->VAO);

    free(spriteRenderer);
}

void drawSprite(SpriteRenderer *spriteRenderer, Texture texture, Vec2 position, Vec2 size, float rotate, Vec3 color) {
    glUseProgram(spriteRenderer->shader);

    float *model = createIdentityMat4();
    translateMat4(model, (Vec3){position.x, position.y, 0.0f});
    translateMat4(model, (Vec3){0.5f * size.x, 0.5f * size.y, 0.0f});
    rotateMat4(model, (Vec3){0.0f, 0.0f, 1.0f}, rotate); 
    translateMat4(model, (Vec3){-0.5f * size.x, -0.5f * size.y, 0.0f});
    scaleMat4(model, (Vec3){size.x, size.y, 1.0f});
  
    glUniformMatrix4fv(glGetUniformLocation(spriteRenderer->shader, "model"), 1, GL_FALSE, model);
    glUniform3f(glGetUniformLocation(spriteRenderer->shader, "spriteColor"), color.x, color.y, color.z);
  
    useTexture(texture);

    glBindVertexArray(spriteRenderer->VAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}