#pragma once
#include "header.h"
#include "Logger.h"

// Ultimate Base class for all UI controls (buttons, text fields, etc.)
class Control {
public:
    Control(HWND parent, HINSTANCE hInstance, int x, int y, int width, int height, HMENU id); // ID is part of the constructor, so there are no doubles
    virtual ~Control();
    virtual void create() = 0;
    HWND getHandle() const;
    HMENU getId() const; // Get the control’s ID to perform actions with it. 

protected:
    HWND parentHwnd;
    HINSTANCE hInstance;
    int x;
    int y;
    int width;
    int height;
    HWND hControl;
	HMENU controlId;
};