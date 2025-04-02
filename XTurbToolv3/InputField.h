#pragma once
#include "Control.h"
#include <string>

// This class creates the input fields (fields to write data to)
class InputField : public Control {
public:
    InputField(HWND parent, HINSTANCE hInstance, int x, int y, int width, int height, HMENU id);
    void create() override;
    void setFont(HFONT hFont);
    void setDefaultText(const std::wstring& text);
    std::wstring getText() const;
};