#pragma once // Prevents this file from being included multiple times in a single compilation

#include "header.h" // Includes standard Windows headers from the template (renamed from framework.h)

// Base class for all windows in the application
class Window {
public:
    Window(); // Constructor to initialize the window
    virtual ~Window(); // Virtual destructor for proper cleanup in derived classes
    virtual void create(HINSTANCE hInstance, int nCmdShow); // Virtual method to create the window
    virtual LRESULT handleMessage(UINT msg, WPARAM wParam, LPARAM lParam); // Virtual method to process messages
    HWND getHwnd() const; // Returns the window handle

protected:
    HWND hwnd; // Handle to the window, stored here for access
    HINSTANCE hInstance; // Application instance handle, needed for window creation
    bool isValid; // Flag to track if the object is still valid
    static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam); // Static window procedure
};