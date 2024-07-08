#include "ImGuiImageButtonHandler.h"

#include "escapi.h"
#include "imgui.h"

#include "ImGuiWrappers/ImGuiTextOutput.h"

bool ImGuiImageButtonHandler::gl32AlreadyInitialized = false;

ImGuiImageButtonHandler::ImGuiImageButtonHandler()
{
    if (!gl32AlreadyInitialized) {
        gl3wInit();
        gl32AlreadyInitialized = true;
    }
}

ImGuiImageButtonHandler::~ImGuiImageButtonHandler()
{
    glDeleteTextures(1, &texture);
}

void ImGuiImageButtonHandler::assureTextureIsGenerated()
{
    if (!textureGenerated) {
        glGenTextures(1, &texture);
        textureGenerated = true;
    };
}

void ImGuiImageButtonHandler::renderImageButton(
    const std::string& id,
    Image& image
)
{
    assureTextureIsGenerated();

    if (image.isValid()) {
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.getWidth(), image.getHeight(), 0, GL_BGRA, GL_UNSIGNED_BYTE, (GLvoid*)(image.getMat()->ptr()));

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.f, 0.f, 0.f, 0.f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.f, 0.f, 0.f, 0.f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.f, 0.f, 0.f, 0.f));
        
        ImGui::ImageButton(id.c_str(), (void*)texture, ImVec2(image.getWidth(), image.getHeight()));
        if (ImGui::IsItemActive() && ImGui::IsItemHovered()) {
            wasPressed = true;
            positionPressed.x = (ImGui::GetMousePos().x - ImGui::GetItemRectMin().x);
            positionPressed.y = (ImGui::GetMousePos().y - ImGui::GetItemRectMin().y);
        }
        else {
            positionPressed = Point2D<int>(0, 0);
            wasPressed = false;
        }
        ImGui::PopStyleColor(3);
    }
    else {
        ImGuiTextOutput("invalid image");
    }
}

bool ImGuiImageButtonHandler::getWasPressed() const
{
    return wasPressed;
}

Point2D<int> ImGuiImageButtonHandler::getPositionPressed() const
{
    return positionPressed;
}