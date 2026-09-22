#include <stdlib.h>
#include <stdio.h>

#include "postProcessor.h"
#include "shader.h"

void initRenderData(PostProcessor *postProcessor);

PostProcessor *createPostProcessor(unsigned int windowWidth, unsigned int windowHeight) {
    PostProcessor *postProcessor = malloc(sizeof(PostProcessor));
    postProcessor->chaos = false;
    postProcessor->confuse = false;
    postProcessor->shake = false;
    postProcessor->width = windowWidth;
    postProcessor->height = windowHeight;

    glGenFramebuffers(1, &postProcessor->MSFBO);
    glGenFramebuffers(1, &postProcessor->FBO);
    glGenRenderbuffers(1, &postProcessor->RBO);
    // initialize renderbuffer storage with a multisampled color buffer (don't need a depth/stencil buffer)
    glBindFramebuffer(GL_FRAMEBUFFER, postProcessor->MSFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, postProcessor->RBO);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_RGB, windowWidth, windowHeight); // allocate storage for render buffer object
    // attach MS render buffer object to framebuffer
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, postProcessor->RBO); 
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        printf("error in postprocessor: Failed to initialize MSFBO");

    // also initialize the FBO/texture to blit multisampled color-buffer to; used for shader operations (for postprocessing effects)
    glBindFramebuffer(GL_FRAMEBUFFER, postProcessor->FBO);

    glGenTextures(1, &postProcessor->textureId);  
    glBindTexture(GL_TEXTURE_2D, postProcessor->textureId);  
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, windowWidth, windowHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    // attach texture to framebuffer as its color attachment
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, postProcessor->textureId, 0); 

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        printf("error in postprocessor: Failed to initialize FBO");

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    initRenderData(postProcessor);

    postProcessor->postProcessingShader = createShaderProgram("shaders/screenQuad.vert", NULL, "shaders/screenQuad.frag");
    glUseProgram(postProcessor->postProcessingShader);
    glUniform1i(glGetUniformLocation(postProcessor->postProcessingShader, "scene"), 0);
    float offset = 1.0f / 300.0f;
    float offsets[9][2] = {
        { -offset,  offset  },  // top-left
        {  0.0f,    offset  },  // top-center
        {  offset,  offset  },  // top-right
        { -offset,  0.0f    },  // center-left
        {  0.0f,    0.0f    },  // center-center
        {  offset,  0.0f    },  // center - right
        { -offset, -offset  },  // bottom-left
        {  0.0f,   -offset  },  // bottom-center
        {  offset, -offset  }   // bottom-right    
    };
    glUniform2fv(glGetUniformLocation(postProcessor->postProcessingShader, "offsets"), 9, (float*)offsets);
    int edge_kernel[9] = {
        -1, -1, -1,
        -1,  8, -1,
        -1, -1, -1
    };
    glUniform1iv(glGetUniformLocation(postProcessor->postProcessingShader, "edge_kernel"), 9, edge_kernel);
    float blur_kernel[9] = {
        0.5f / 16.0f, 1.0f / 16.0f, 0.5f / 16.0f,
        1.0f / 16.0f, 10.0f / 16.0f, 1.0f / 16.0f,
        0.5f / 16.0f, 1.0f / 16.0f, 0.5f / 16.0f
    };
    glUniform1fv(glGetUniformLocation(postProcessor->postProcessingShader, "blur_kernel"), 9, blur_kernel);  

    return postProcessor;
}

void initRenderData(PostProcessor *postProcessor) {
    float vertices[] = {
        // pos        // tex
        -1.0f, -1.0f, 0.0f, 0.0f,
        1.0f, -1.0f, 1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f, 1.0f,
        1.0f,  1.0f, 1.0f, 1.0f,
    };
    glGenVertexArrays(1, &postProcessor->VAO);
    glGenBuffers(1, &postProcessor->VBO);

    glBindVertexArray(postProcessor->VAO);

    glBindBuffer(GL_ARRAY_BUFFER, postProcessor->VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void freePostProcessor(PostProcessor *postProcessor) {
    glDeleteFramebuffers(1, &postProcessor->MSFBO);
    glDeleteFramebuffers(1, &postProcessor->FBO);
    glDeleteRenderbuffers(1, &postProcessor->RBO);
    glDeleteTextures(1, &postProcessor->textureId); 
    glDeleteVertexArrays(1, &postProcessor->VAO);
    glDeleteBuffers(1, &postProcessor->VBO);
    glDeleteProgram(postProcessor->postProcessingShader);

    free(postProcessor);
}

void postProcessorBeginRender(PostProcessor *postProcessor) {
    glBindFramebuffer(GL_FRAMEBUFFER, postProcessor->MSFBO);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void postProcessorEndRender(PostProcessor *postProcessor) {
    // now resolve multisampled color-buffer into intermediate FBO to store to texture
    glBindFramebuffer(GL_READ_FRAMEBUFFER, postProcessor->MSFBO);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, postProcessor->FBO);
    glBlitFramebuffer(
        0, 
        0, 
        postProcessor->width, 
        postProcessor->height, 
        0, 
        0, 
        postProcessor->width, 
        postProcessor->height, 
        GL_COLOR_BUFFER_BIT, 
        GL_NEAREST
    );
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void postProcessorRender(PostProcessor *postProcessor, float time) {
    glUseProgram(postProcessor->postProcessingShader);
    glUniform1f(glGetUniformLocation(postProcessor->postProcessingShader, "time"), time);
    glUniform1i(glGetUniformLocation(postProcessor->postProcessingShader, "confuse"), postProcessor->confuse);
    glUniform1i(glGetUniformLocation(postProcessor->postProcessingShader, "chaos"), postProcessor->chaos);
    glUniform1i(glGetUniformLocation(postProcessor->postProcessingShader, "shake"), postProcessor->shake);
    // render textured quad
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(postProcessor->VAO);
    glBindTexture(GL_TEXTURE_2D, postProcessor->textureId);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}