#pragma once
#include "Control.h"
#include <string>
// This class creates buttons used throughout the app. 
class Button : public Control {
public:
    Button(HWND parent, HINSTANCE hInstance, int x, int y, int width, int height, const std::wstring& text, HMENU id);
    void create() override;
    void setFont(HFONT hFont);

private:
    std::wstring text;
};