#ifndef ImGuiImageRenderer_H
#define ImGuiImageRenderer_H

#include "Common\Image.h"
#include "gl3w.h"

class ImGuiImageRenderer
{
public:
    ImGuiImageRenderer();
    ~ImGuiImageRenderer();

    void renderImage(
        Image& image
    );

protected:
    static bool gl32AlreadyInitialized;

private:
    void assureTextureIsGenerated();

    GLuint texture = 0;
    bool textureGenerated = false;
};

#endif