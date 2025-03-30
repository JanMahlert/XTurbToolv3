#include "Control.h"

// Constructor: Initialize members
Control::Control(HWND parent, HINSTANCE hInstance, int x, int y, int width, int height, HMENU id)
    : parentHwnd(parent), hInstance(hInstance), x(x), y(y), width(width), height(height), hControl(nullptr), controlId(id) {
}

// Destructor: Clean up the control
Control::~Control() {
    if (hControl) {
        DestroyWindow(hControl);
        hControl = nullptr;
    }
}

// Getter for the control handle
HWND Control::getHandle() const {
    return hControl;
}

// Getter for the control ID
HMENU Control::getId() const {
    return controlId;
}