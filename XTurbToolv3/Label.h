#pragma once

#include "Control.h"
#include <string>

// This class creates labels (for display only)
class Label : public Control {
public:
    Label(HWND parent, HINSTANCE hInstance, int x, int y, int width, int height, const std::wstring& text);
    void create() override;
    void setFont(HFONT hFont);

private:
    std::wstring text;
};