#include "Button.h"

// Constructor: Initialize with parent, position, text, and ID
Button::Button(HWND parent, HINSTANCE hInstance, int x, int y, int width, int height, const std::wstring& text, HMENU id)
    : Control(parent, hInstance, x, y, width, height, id), text(text) {
}

// Create the button control
void Button::create() {
    hControl = CreateWindowW(L"BUTTON", text.c_str(), WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
        x, y, width, height, parentHwnd, controlId, hInstance, nullptr);

    //Check if the button creation succeeded; log an error and exit if it failed.
    if (!hControl) {
        Logger::logError(L"Failed to create button!");
        return;
    }
}

// Set a custom font for the button
void Button::setFont(HFONT hFont) {
    if (hControl && hFont) {
        SendMessage(hControl, WM_SETFONT, (WPARAM)hFont, TRUE);
    }
}