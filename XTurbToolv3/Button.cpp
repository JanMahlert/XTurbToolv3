//Written by: Luke Spangler

#include "Button.h"

//---------------------- Constructor ----------------------//

Button::Button(HWND parent, HINSTANCE hInstance, int x, int y, int width, int height, const std::wstring& text, HMENU id)
    : Control(parent, hInstance, x, y, width, height, id), text(text) {
}

//---------------------- Member Functions ----------------------//

//Creates a button when called
void Button::create() {
    hControl = CreateWindowW(L"BUTTON", text.c_str(), WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
        x, y, width, height, parentHwnd, controlId, hInstance, nullptr);

    if (!hControl) {
        Logger::logError(L"Failed to create button!");
        return;
    }
}

//Sets the buttons font when called
void Button::setFont(HFONT hFont) {
    if (hControl && hFont) {
        SendMessage(hControl, WM_SETFONT, (WPARAM)hFont, TRUE);
    }
}