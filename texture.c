#include <glad/glad.h>
#include <stb/stb_image.h>
#include <stdbool.h>

#include "texture.h"

GLenum getFormatFromNumChannels(int numChannels);

Texture createTexture(char* path, unsigned int shaderProgram, unsigned int unit, char* uniformName) {
    unsigned int texture;
    glGenTextures(1, &texture);  
    glBindTexture(GL_TEXTURE_2D, texture);  
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    int width, height, numberChannels;
    stbi_uc *data = stbi_load(path, &width, &height, &numberChannels, 0); 
    if (data == NULL) {
        printf("Failed to load texture: %s", path);
    }
    GLenum format = getFormatFromNumChannels(numberChannels);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    stbi_image_free(data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glUseProgram(shaderProgram);
    glUniform1i(glGetUniformLocation(shaderProgram, uniformName), unit);
    glUseProgram(0);

    return (Texture){
        .id = texture,
        .unit = unit
    };
}

GLenum getFormatFromNumChannels(int numChannels) {
    switch (numChannels) {
        case 1:
            return GL_RED;
        case 3:
            return GL_RGB;
        case 4:
            return GL_RGBA;
    }
}

void useTexture(Texture texture) {
    glActiveTexture(GL_TEXTURE0 + texture.unit);
    glBindTexture(GL_TEXTURE_2D, texture.id);
}