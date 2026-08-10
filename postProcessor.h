#pragma once

#include "stdbool.h"

#include "texture.h"

typedef struct {
    unsigned int postProcessingShader;
    unsigned int textureId;
    unsigned int width, height;
    
    bool confuse, chaos, shake;
    
    
    unsigned int MSFBO, FBO; // MSFBO = Multisampled FBO. FBO is regular, used for blitting MS color-buffer to texture
    unsigned int RBO;
    unsigned int VAO, VBO;
} PostProcessor;


PostProcessor *createPostProcessor(unsigned int width, unsigned int height);
void freePostProcessor(PostProcessor *postProcessor);

// prepares the postprocessor's framebuffer operations before rendering the game
void postProcessorBeginRender(PostProcessor *postProcessor);
// should be called after rendering the game, so it stores all the rendered data into a texture object
void postProcessorEndRender(PostProcessor *postProcessor);
// renders the PostProcessor texture quad (as a screen-encompassing large sprite)
void postProcessorRender(PostProcessor *postProcessor, float time);