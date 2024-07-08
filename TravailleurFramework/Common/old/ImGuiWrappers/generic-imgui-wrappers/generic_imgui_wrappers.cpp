#include "generic_imgui_wrappers.h"

void ImGuiShowColor(int color)
{
    //std::cout << "red: " << (float)((color & 0x00ff0000) >> 16) << std::endl << "green: " << (float)((color & 0x0000ff00) >> 8) << std::endl << "blue: " << std::endl << (float)(color & 0x000000ff) << std::endl; 

    float red = (float)((color & 0x00ff0000) >> 16)/255;
    float green = (float)((color & 0x0000ff00) >> 8)/255;
    float blue = (float)(color & 0x000000ff)/255;
    //printf("red: %lf\ngreen: %lf\nblue: %lf\n", red, green, blue);
    ImVec4 colorVector(red, green, blue, 255.0f);
    ImGui::PushStyleColor(ImGuiCol_Button, colorVector);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, colorVector);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, colorVector);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 2.0f);

    ImGui::Button("", ImVec2(20, 20));

    ImGui::PopStyleVar();
    ImGui::PopStyleColor(3);
};



#include <windows.h>
void ImGuiColor(char* colorName, int* color)
{
    float red = (float)((*color & 0x00ff0000) >> 16)/255;
    float green = (float)((*color & 0x0000ff00) >> 8)/255;
    float blue = (float)(*color & 0x000000ff)/255;
    //std::cout << red << " " << green << " " << blue << std::endl;
    float colorFloats[3];
    colorFloats[0] = (red);
    colorFloats[1] = (green);
    colorFloats[2] = (blue);
    ImGui::ColorEdit3(colorName, colorFloats);
    int redInt = (255*colorFloats[0]);
    redInt = redInt << 16;
    int greenInt = (255*colorFloats[1]);
    greenInt = greenInt << 8;
    int blueInt = (255*colorFloats[2]);
    *color = blueInt;
    *color |= greenInt;
    *color |= redInt;
}


void ImGuiDoublePercentageSlider(char* label, double* value, double range_min, double range_max, double speed)
{
    ImGui::DragScalar(label, ImGuiDataType_Double, value, speed, &range_min, &range_max,"%.2f %%", 1.0f);
}

void ImGuiDoubleSlider(char* label, double* value, double range_min, double range_max, double speed)
{
    ImGui::DragScalar(label, ImGuiDataType_Double, value, speed, &range_min, &range_max,"%.8f", 1.0f);
}

void ImGuiIntSlider(char* label, int* value, int range_min, int range_max, double speed)
{
    ImGui::DragScalar(label, ImGuiDataType_S32, value, speed, &range_min, &range_max, NULL, 1.0f);
}

void ImGuiDirectoryTree()
{
    if (ImGui::TreeNode("Advanced, with Selectable nodes"))
        {
            static ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
            static bool align_label_with_current_x_position = false;
            static bool test_drag_and_drop = false;
            ImGui::CheckboxFlags("ImGuiTreeNodeFlags_OpenOnArrow",       (unsigned int*)&base_flags, ImGuiTreeNodeFlags_OpenOnArrow);
            ImGui::CheckboxFlags("ImGuiTreeNodeFlags_OpenOnDoubleClick", (unsigned int*)&base_flags, ImGuiTreeNodeFlags_OpenOnDoubleClick);
            ImGui::CheckboxFlags("ImGuiTreeNodeFlags_SpanAvailWidth",    (unsigned int*)&base_flags, ImGuiTreeNodeFlags_SpanAvailWidth); ImGui::SameLine();
            ImGui::CheckboxFlags("ImGuiTreeNodeFlags_SpanFullWidth",     (unsigned int*)&base_flags, ImGuiTreeNodeFlags_SpanFullWidth);
            ImGui::Checkbox("Align label with current X position", &align_label_with_current_x_position);
            ImGui::Checkbox("Test tree node as drag source", &test_drag_and_drop);
            ImGui::Text("Hello!");
            if (align_label_with_current_x_position)
                ImGui::Unindent(ImGui::GetTreeNodeToLabelSpacing());

            // 'selection_mask' is dumb representation of what may be user-side selection state.
            //  You may retain selection state inside or outside your objects in whatever format you see fit.
            // 'node_clicked' is temporary storage of what node we have clicked to process selection at the end
            /// of the loop. May be a pointer to your own node type, etc.
            static int selection_mask = (1 << 2);
            int node_clicked = -1;
            for (int i = 0; i < 6; i++)
            {
                // Disable the default "open on single-click behavior" + set Selected flag according to our selection.
                ImGuiTreeNodeFlags node_flags = base_flags;
                const bool is_selected = (selection_mask & (1 << i)) != 0;
                if (is_selected)
                    node_flags |= ImGuiTreeNodeFlags_Selected;
                if (i < 3)
                {
                    // Items 0..2 are Tree Node
                    bool node_open = ImGui::TreeNodeEx((void*)(intptr_t)i, node_flags, "Selectable Node %d", i);
                    if (ImGui::IsItemClicked())
                        node_clicked = i;
                    if (test_drag_and_drop && ImGui::BeginDragDropSource())
                    {
                        ImGui::SetDragDropPayload("_TREENODE", NULL, 0);
                        ImGui::Text("This is a drag and drop source");
                        ImGui::EndDragDropSource();
                    }
                    if (node_open)
                    {
                        ImGui::BulletText("Blah blah\nBlah Blah");
                        ImGui::TreePop();
                    }
                }
                else
                {
                    // Items 3..5 are Tree Leaves
                    // The only reason we use TreeNode at all is to allow selection of the leaf. Otherwise we can
                    // use BulletText() or advance the cursor by GetTreeNodeToLabelSpacing() and call Text().
                    node_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen; // ImGuiTreeNodeFlags_Bullet
                    ImGui::TreeNodeEx((void*)(intptr_t)i, node_flags, "Selectable Leaf %d", i);
                    if (ImGui::IsItemClicked())
                        node_clicked = i;
                    if (test_drag_and_drop && ImGui::BeginDragDropSource())
                    {
                        ImGui::SetDragDropPayload("_TREENODE", NULL, 0);
                        ImGui::Text("This is a drag and drop source");
                        ImGui::EndDragDropSource();
                    }
                }
            }
            if (node_clicked != -1)
            {
                // Update selection state
                // (process outside of tree loop to avoid visual inconsistencies during the clicking frame)
                if (ImGui::GetIO().KeyCtrl)
                    selection_mask ^= (1 << node_clicked);          // CTRL+click to toggle
                else //if (!(selection_mask & (1 << node_clicked))) // Depending on selection behavior you want, may want to preserve selection when clicking on item that is part of the selection
                    selection_mask = (1 << node_clicked);           // Click to single-select
            }
            if (align_label_with_current_x_position)
                ImGui::Indent(ImGui::GetTreeNodeToLabelSpacing());
            ImGui::TreePop();
        }
}

void comboFromStrings(std::string comboName, std::vector<std::string> strings, int* index)
{
    if(strings.empty())
    {
        return;
    }

    CStringVector cStringVector(strings);

    int returnValue;
    ImGui::Combo(comboName.c_str(), index, cStringVector.getBuffer(), cStringVector.getSize());
}

void ImGuiListBox(char* name, int* index, char** items, int howManyItems, int height_in_items)
{
    printf("ImGuiListBox called\n");
    printf("name: %s\n", name);
    printf("*index: %d\n", *index);
    printf("items: %d\n", items);
    printf("howManyItems: %d\n", howManyItems);
    printf("height_in_items: %d\n", height_in_items);
    ImGui::ListBox(name, index, items, howManyItems, height_in_items);
    printf("ImGuiListBox executed\n");
}

void ImGuiTextInput(std::string textInputName, char* text, int bufferSize, int howManyLines)
{
    if(howManyLines < 1)
    {
        howManyLines = 1;
    }
    static ImGuiInputTextFlags flags = ImGuiInputTextFlags_AllowTabInput;
    //HelpMarker("You can use the ImGuiInputTextFlags_CallbackResize facility if you need to wire InputTextMultiline() to a dynamic string type. See misc/cpp/imgui_stdlib.h for an example. (This is not demonstrated in imgui_demo.cpp because we don't want to include <string> in here)");
    //ImGui::CheckboxFlags("ImGuiInputTextFlags_ReadOnly", (unsigned int*)&flags, ImGuiInputTextFlags_ReadOnly);
    //ImGui::CheckboxFlags("ImGuiInputTextFlags_AllowTabInput", (unsigned int*)&flags, ImGuiInputTextFlags_AllowTabInput);
    //ImGui::CheckboxFlags("ImGuiInputTextFlags_CtrlEnterForNewLine", (unsigned int*)&flags, ImGuiInputTextFlags_CtrlEnterForNewLine);
    char preText[2048];
    sprintf(preText, "text input \"%s\":", textInputName.c_str());
    ImGui::TextWrapped(preText);
    ImGui::InputTextMultiline(textInputName.c_str(), text, bufferSize, ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * howManyLines), flags);
}

void ImGuiShowRawImage(RawImage* image)
{
    
}