#include "Label.h"

// Constructor: Initialize with parent, position, and text
Label::Label(HWND parent, HINSTANCE hInstance, int x, int y, int width, int height, const std::wstring& text)
    : Control(parent, hInstance, x, y, width, height, nullptr), text(text) {
}

// Create the label control
void Label::create() {
    hControl = CreateWindowW(L"STATIC", text.c_str(), WS_VISIBLE | WS_CHILD,
        x, y, width, height, parentHwnd, controlId, hInstance, nullptr);

    if (!hControl) {
        Logger::logError(L"Failed to create label!");
        return;
    }
}

// Set a custom font for the label
void Label::setFont(HFONT hFont) {
    if (hControl && hFont) {
        SendMessage(hControl, WM_SETFONT, (WPARAM)hFont, TRUE);
    }
}