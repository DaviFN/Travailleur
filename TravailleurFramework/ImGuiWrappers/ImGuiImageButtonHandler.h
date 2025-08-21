#ifndef ImGuiImageButtonHandler_H
#define ImGuiImageButtonHandler_H

#include <string>

#include "Common/Image.h"
#include "gl3w.h"

class ImGuiImageButtonHandler
{
public:
    ImGuiImageButtonHandler();
    ~ImGuiImageButtonHandler();

    void renderImageButton(
        const std::string& id,
        Image& image
    );

    bool getWasPressed() const;

    Point2D<int> getPositionPressed() const;

protected:
    static bool gl32AlreadyInitialized;

private:
    void assureTextureIsGenerated();

    bool wasPressed = false;
    Point2D<int> positionPressed;

    GLuint texture = 0;
    bool textureGenerated = false;
};

#endif