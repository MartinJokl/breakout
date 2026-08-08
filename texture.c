#include <glad/glad.h>
#include <stb/stb_image.h>
#include <stdbool.h>

#include "texture.h"


TextureB createTexture(char* path, GLenum format, unsigned int shaderProgram, unsigned int unit, char* uniformName) {
    unsigned int texture;
    glGenTextures(1, &texture);  
    glBindTexture(GL_TEXTURE_2D, texture);  
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    stbi_set_flip_vertically_on_load(true);
    int width, height, numberChannels;
    unsigned char *data = stbi_load(path, &width, &height, &numberChannels, 0); 
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    stbi_image_free(data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glUseProgram(shaderProgram);
    glUniform1i(glGetUniformLocation(shaderProgram, uniformName), unit);
    glUseProgram(0);

    return (TextureB){
        .id = texture,
        .unit = unit
    };
}

void useTexture(TextureB texture) {
    glActiveTexture(GL_TEXTURE0 + texture.unit);
    glBindTexture(GL_TEXTURE_2D, texture.id);
}