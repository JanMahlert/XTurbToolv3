#include "FileSelectorWindow.h"
#include <windows.h>
#include <commctrl.h>
#include <filesystem>
#include "Logger.h"

bool FileSelectorWindow::classRegistered = false;

void FileSelectorWindow::RegisterClass(HINSTANCE hInstance) {
    if (!classRegistered) {
        WNDCLASSEXW wcex = { 0 };
        wcex.cbSize = sizeof(WNDCLASSEX);
        wcex.style = CS_HREDRAW | CS_VREDRAW;
        wcex.lpfnWndProc = Window::WndProc;
        wcex.hInstance = hInstance;
        wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
        wcex.lpszClassName = L"FileSelectorWindowClass";
        if (!RegisterClassExW(&wcex)) {
            Logger::logError(L"Failed to register FileSelectorWindow class");
            return;
        }
        classRegistered = true;
        Logger::logError(L"FileSelectorWindow class registered successfully");
    }
}

FileSelectorWindow::FileSelectorWindow(HINSTANCE hInstance, HWND parent, const std::wstring& directory)
    : Window(), parent(parent), directory(directory), comboBox(nullptr) {
    this->hInstance = hInstance;
}

void FileSelectorWindow::create(HINSTANCE hInstance, int nCmdShow) {
    static bool classRegistered = false;
    static int windowCount = 0; // Track number of windows for positioning
    if (!classRegistered) {
        WNDCLASSEXW wcex = { 0 };
        wcex.cbSize = sizeof(WNDCLASSEX);
        wcex.style = CS_HREDRAW | CS_VREDRAW;
        wcex.lpfnWndProc = Window::WndProc;
        wcex.hInstance = hInstance;
        wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
        wcex.lpszClassName = L"FileSelectorWindowClass";
        if (!RegisterClassExW(&wcex)) {
            Logger::logError(L"Failed to register FileSelectorWindow class");
            return;
        }
        classRegistered = true;
        Logger::logError(L"FileSelectorWindow class registered successfully");
    }

    // Increment window count and offset position
    windowCount++;
    int xPos = 100 + (windowCount - 1) * 20;
    int yPos = 100 + (windowCount - 1) * 20;

    hwnd = CreateWindowW(L"FileSelectorWindowClass", L"Select XTurb Output File",
        WS_OVERLAPPEDWINDOW, xPos, yPos, 400, 200, parent, nullptr, hInstance, this);
    if (!hwnd) {
        Logger::logError(L"Failed to create FileSelectorWindow");
        return;
    }

    // Create ComboBox
    comboBox = CreateWindowW(L"COMBOBOX", L"", CBS_DROPDOWNLIST | WS_CHILD | WS_VISIBLE | WS_VSCROLL,
        10, 10, 360, 100, hwnd, (HMENU)1001, hInstance, nullptr);
    if (!comboBox) {
        Logger::logError(L"Failed to create combo box in FileSelectorWindow");
    }

    // Create Select Button
    HWND selectButton = CreateWindowW(L"BUTTON", L"Select", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        150, 50, 80, 30, hwnd, (HMENU)1002, hInstance, nullptr);
    if (!selectButton) {
        Logger::logError(L"Failed to create select button in FileSelectorWindow");
    }

    refreshFileList();
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);
}

void FileSelectorWindow::refreshFileList() {
    if (!comboBox) return;

    SendMessageW(comboBox, CB_RESETCONTENT, 0, 0);
    files.clear();

    for (const auto& entry : std::filesystem::directory_iterator(directory)) {
        std::wstring filename = entry.path().filename().wstring();
        if (filename.find(L"XTurb_Output") == 0 &&
            filename.length() >= 4 && filename.substr(filename.length() - 4) == L".dat") {
            files.push_back(filename);
            Logger::logError(L"Found file: " + filename);
        }
    }

    for (const auto& file : files) {
        SendMessageW(comboBox, CB_ADDSTRING, 0, (LPARAM)file.c_str());
    }
    if (!files.empty()) {
        SendMessageW(comboBox, CB_SETCURSEL, 0, 0);
    }
}

LRESULT FileSelectorWindow::handleMessage(UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_COMMAND:
        if (LOWORD(wParam) == 1002 && HIWORD(wParam) == BN_CLICKED) {
            LRESULT index = SendMessageW(comboBox, CB_GETCURSEL, 0, 0); // Line ~91
            if (index != CB_ERR && index >= 0 && index < files.size()) {
                selectedFile = directory + L"\\" + files[index];
                Logger::logError(L"Selected file: " + selectedFile);
                PostMessage(parent, WM_USER + 102, 0, (LPARAM)this);
            }
            else {
                Logger::logError(L"No file selected");
            }
        }
        break;
    case WM_CLOSE:
        DestroyWindow(hwnd);
        break;
    case WM_DESTROY:
        // No action needed; Container handles cleanup
        break;
    default:
        return Window::handleMessage(msg, wParam, lParam);
    }
    return 0;
}