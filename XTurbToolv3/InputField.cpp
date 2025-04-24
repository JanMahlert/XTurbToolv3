#include "InputField.h"

// Constructor: Initialize with parent, position, and ID
InputField::InputField(HWND parent, HINSTANCE hInstance, int x, int y, int width, int height, HMENU id)
    : Control(parent, hInstance, x, y, width, height, id) {
}

// Create the input field control
void InputField::create() {
    hControl = CreateWindowW(L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL,
        x, y, width, height, parentHwnd, controlId, hInstance, nullptr);

    // Checks if the control handle is valid and logs an error if the creation failed.
    if (!hControl) {
        Logger::logError(L"Failed to create input field!");
        return;
    }
}

// Set a custom font for the input field
void InputField::setFont(HFONT hFont) {
    // Sets the font of the control to hFont if both the control and font handles are valid.
    if (hControl && hFont) {
        SendMessage(hControl, WM_SETFONT, (WPARAM)hFont, TRUE);
    }
}

// Set default text in the input field
void InputField::setDefaultText(const std::wstring& text) {
    // Sets the text of the control to text if the control handle is valid.
    if (hControl) {
        SetWindowTextW(hControl, text.c_str());
    }
}

// Get the current text from the input field
std::wstring InputField::getText() const {
    if (!hControl) return L"";

    int length = GetWindowTextLengthW(hControl) + 1;
    std::wstring text(length, L'\0');
    GetWindowTextW(hControl, &text[0], length);
    return text;
}