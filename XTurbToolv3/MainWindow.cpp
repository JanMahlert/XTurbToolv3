#include "MainWindow.h"
#include "XTurbTool.h"

// Constructor: Initialize members
MainWindow::MainWindow() : Window(), container(nullptr) {}

// Create the main window and its container
void MainWindow::create(HINSTANCE hInstance, int nCmdShow) {
    Window::create(hInstance, nCmdShow); // Call base class to create main window

    if (!hwnd) return; // Exit if main window creation failed

    container = new Container(hwnd, hInstance, 50, 20, 800, 600); // Create container at (50,50), 200x200
    container->create(hInstance, nCmdShow); // Initialize the container
}

// Handle messages for the main window
LRESULT MainWindow::handleMessage(UINT msg, WPARAM wParam, LPARAM lParam) {
    if (!isValid) { // Check if the object is still valid
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }

    switch (msg) {
    case WM_PAINT: { // Paint the main window
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1)); // Background from base class
        EndPaint(hwnd, &ps);
        return 0;
    }
    case WM_DESTROY: // Clean up when closing
        delete container; // Delete the container
        container = nullptr;
        return Window::handleMessage(msg, wParam, lParam); // Call base class for PostQuitMessage
    default:
        return Window::handleMessage(msg, wParam, lParam);
    }
    return 0;
}