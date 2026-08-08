#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glad/glad.h>

#include "shader.h"

const char* readFile(char* path);
void checkForErrors(unsigned int shader, char* type);

unsigned int createShaderProgram(char* vertexPath, char *geometryPath, char* fragmentPath) {
    const char* vertexSource = readFile(vertexPath);
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSource, NULL);
    glCompileShader(vertexShader);
    checkForErrors(vertexShader, "VERTEX");
    free((char*)vertexSource);
    
    unsigned int geometryShader = 0;
    if (geometryPath != NULL) {
        const char* geometrySource = readFile(geometryPath);
        geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
        glShaderSource(geometryShader, 1, &geometrySource, NULL);
        glCompileShader(geometryShader);
        checkForErrors(geometryShader, "GEOMETRY");
        free((char*)geometrySource);
    }

    const char* fragmentSource = readFile(fragmentPath);
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
    glCompileShader(fragmentShader);
    checkForErrors(fragmentShader, "FRAGMENT");
    free((char*)fragmentSource);

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    if (geometryShader != 0)
        glAttachShader(shaderProgram, geometryShader);
    glLinkProgram(shaderProgram);
    checkForErrors(shaderProgram, "PROGRAM");

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader); 
    if (geometryShader != 0)
        glDeleteShader(geometryShader);

    return shaderProgram;
}

const char* readFile(char* path) {
    char* text = calloc(1, sizeof(char));
    if (text == NULL) {
        printf("Malloc failed in readFile");
        return NULL;
    }
    int textLength = 0;

    FILE* pFile = fopen(path, "r");
    char buffer[1024] = {0};
    if (pFile == NULL) {
        printf("Could not open file");
        return NULL;
    }
    while (fgets(buffer, sizeof(buffer), pFile) != NULL) {
        textLength += strlen(buffer) * sizeof(char);
        char* temp = realloc(text, textLength + 1);
        if (temp == NULL) {
            printf("Realloc failed in readFile");
            return NULL;
        }
        text = temp;
        strcat(text, buffer);
    }

    return text;
}

void checkForErrors(unsigned int shader, char* type) {
    int  success;
    char infoLog[512];
    if (type == "PROGRAM") {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if(success) {
            return;
        }
        glGetProgramInfoLog(shader, 512, NULL, infoLog);
        printf("shader program failed to link: %s\n", infoLog);
    }
    else {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if(success) {
            return;
        }
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        printf("%s shader failed to compile: %s\n", type, infoLog);
    }
}