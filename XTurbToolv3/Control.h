#pragma once

#include "header.h"
#include "Logger.h"

// Base class for all UI controls (e.g., buttons, text fields)
class Control {
public:
    Control(HWND parent, HINSTANCE hInstance, int x, int y, int width, int height, HMENU id); // Add ID to constructor
    virtual ~Control();
    virtual void create() = 0;
    HWND getHandle() const;
    HMENU getId() const; // Get the control’s ID

protected:
    HWND parentHwnd;
    HINSTANCE hInstance;
    int x;
    int y;
    int width;
    int height;
    HWND hControl;
    HMENU controlId; // Store the control ID
};