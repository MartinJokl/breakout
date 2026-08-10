#include <stdio.h>
#include <glad/glad.h>
#include <stdlib.h>

#include "shader.h"
#include "file.h"

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