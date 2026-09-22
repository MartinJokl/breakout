#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "game.h"
#include "matrix.h"
#include "shader.h"
#include "spriteRenderer.h"

void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);


const unsigned int gameWidth = 1200;
const unsigned int gameHeight = 900;

// Separate to make it easy to change window size without 
// changing the ball velocity and things like that
const unsigned int windowWidth = 1200;
const unsigned int windowHeight = 900;

Game *game;

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    glfwWindowHint(GLFW_RESIZABLE, false);

    GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "Breakout", NULL, NULL);
    glfwMakeContextCurrent(window);

    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    glfwSetKeyCallback(window, keyCallback);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

    glViewport(0, 0, windowWidth, windowHeight);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    game = createGame(gameWidth, gameHeight, windowWidth, windowHeight);

    float lastFrame = 0.0f;
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        glfwPollEvents();

        processGameInput(game, deltaTime);
        memset(game->newKeys, false, sizeof(game->newKeys) / sizeof(bool));

        updateGame(game, deltaTime);

        renderGame(game);

        glfwSwapBuffers(window);
    }

    freeGame(game);
    glfwTerminate();
    return 0;
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (key >= 0 && key < 1024) {
        if (action == GLFW_PRESS) {
            game->newKeys[key] = true;
            game->keys[key] = true;
        }
        else if (action == GLFW_RELEASE) {
            game->keys[key] = false;
        }
    }
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}