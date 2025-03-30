#include "Window.h" // Include the header for this class
#include "XTurbTool.h" // Include resource definitions (e.g., IDC_XTURBTOOLV3)

// Constructor: Initializes members to safe defaults
Window::Window() : hwnd(nullptr), hInstance(nullptr), isValid(true) {}

// Destructor: Mark the object as invalid and clean up
Window::~Window() {
    isValid = false; // Mark as invalid to prevent message handling after destruction
    if (hwnd) {
        DestroyWindow(hwnd); // Ensure the window is destroyed
        hwnd = nullptr;
    }
}

// Creates and shows the window
void Window::create(HINSTANCE hInstance, int nCmdShow) {
    this->hInstance = hInstance; // Store the instance handle passed from wWinMain

    WNDCLASSEXW wcex = { 0 }; // Structure to define the window class, initialized to zero
    wcex.cbSize = sizeof(WNDCLASSEX); // Size of this structure, required by Windows
    wcex.style = CS_HREDRAW | CS_VREDRAW; // Redraw window if resized horizontally or vertically
    wcex.lpfnWndProc = WndProc; // Pointer to our static window procedure
    wcex.hInstance = hInstance; // Application instance handle
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_XTURBTOOLV3)); // Load the app icon
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW); // Standard arrow cursor
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1); // Light gray background
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_XTURBTOOLV3); // Menu resource from resource.h
    wcex.lpszClassName = L"XTurbWindowClass"; // Unique name for this window class
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL)); // Small icon

    if (!RegisterClassExW(&wcex)) { // Register the window class with Windows
        MessageBoxW(nullptr, L"Failed to register window class!", L"Error", MB_OK | MB_ICONERROR);
        return; // Exit if registration fails
    }

    hwnd = CreateWindowW(L"XTurbWindowClass", L"XTurb Tool", WS_OVERLAPPEDWINDOW, // Create the window
        CW_USEDEFAULT, 0, 1000, 700, nullptr, nullptr, hInstance, this);

    if (!hwnd) { // Check if window creation failed
        MessageBoxW(nullptr, L"Failed to create window!", L"Error", MB_OK | MB_ICONERROR);
        return;
    }

    ShowWindow(hwnd, nCmdShow); // Show the window (nCmdShow from wWinMain)
    UpdateWindow(hwnd); // Force a repaint to display it
}

// Message handler, called by WndProc
LRESULT Window::handleMessage(UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_DESTROY: // Window is closing
        PostQuitMessage(0); // Tell the message loop to exit
        break;
    default: // Handle all other messages with default behavior
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0; // Return 0 for handled messages
}

// Static window procedure to route messages to the instance
LRESULT CALLBACK Window::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    Window* pThis = nullptr; // Pointer to our class instance

    if (msg == WM_NCCREATE) { // Before the window is created
        CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
        pThis = static_cast<Window*>(pCreate->lpCreateParams); // Get instance from creation params
        if (!pThis) { // Check if pThis is null
            MessageBoxW(nullptr, L"Failed to get Window instance in WM_NCCREATE!", L"Error", MB_OK | MB_ICONERROR);
            return FALSE; // Fail window creation
        }
        SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pThis)); // Store instance in window data
        if (GetLastError() != 0) { // Check for errors in SetWindowLongPtr
            MessageBoxW(nullptr, L"Failed to set GWLP_USERDATA in WM_NCCREATE!", L"Error", MB_OK | MB_ICONERROR);
            return FALSE;
        }
        pThis->hwnd = hWnd; // Set the handle in our instance
    }
    else {
        pThis = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA)); // Retrieve instance for other messages
    }

    if (pThis && pThis->isValid) { // Check if pThis is valid and the object hasn’t been destroyed
        return pThis->handleMessage(msg, wParam, lParam); // Call the instance’s message handler
    }
    return DefWindowProc(hWnd, msg, wParam, lParam); // Default handling if no instance or invalid
}

// Getter for the window handle
HWND Window::getHwnd() const {
    return hwnd;
}