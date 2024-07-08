#include "ImGuiImageRenderer.h"

#include "escapi.h"
#include "imgui.h"

#include "ImGuiWrappers/ImGuiTextOutput.h"

bool ImGuiImageRenderer::gl32AlreadyInitialized = false;

ImGuiImageRenderer::ImGuiImageRenderer()
{
    if (!gl32AlreadyInitialized) {
        gl3wInit();
        gl32AlreadyInitialized = true;
    }
}

ImGuiImageRenderer::~ImGuiImageRenderer()
{
    glDeleteTextures(1, &texture);
}

void ImGuiImageRenderer::assureTextureIsGenerated()
{
    if (!textureGenerated) {
        glGenTextures(1, &texture);
        textureGenerated = true;
    };
}

void ImGuiImageRenderer::renderImage(
    Image& image
)
{
    assureTextureIsGenerated();

    if (image.isValid()) {
        ImGuiTextOutput("image dimensions: " + std::to_string(image.getWidth()) + "x" + std::to_string(image.getHeight()));
        
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.getWidth(), image.getHeight(), 0, GL_BGRA, GL_UNSIGNED_BYTE, (GLvoid*)(image.getMat()->ptr()));

        ImGui::Image((void*)texture, ImVec2(image.getWidth(), image.getHeight()));
    }
    else {
        ImGuiTextOutput("invalid image");
    }
}