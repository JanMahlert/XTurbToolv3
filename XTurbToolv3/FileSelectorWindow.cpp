#include "FileSelectorWindow.h"
#include <windows.h>
#include <commctrl.h>
#include <filesystem>
#include "Logger.h"
#include "FileCompressor.h"

bool FileSelectorWindow::classRegistered = false;

FileSelectorWindow::FileSelectorWindow(HINSTANCE hInstance, HWND parent, const std::wstring& directory)
    : Window(), parent(parent), directory(directory), comboBox(nullptr), compressor(nullptr) {
    this->hInstance = hInstance;
    if (!hInstance) {
        Logger::logError(L"Invalid hInstance in FileSelectorWindow constructor");
    }
    if (parent && !IsWindow(parent)) {
        Logger::logError(L"Invalid parent HWND in FileSelectorWindow constructor");
    }
    compressor = new FileCompressor(std::filesystem::path(directory).string());
}

FileSelectorWindow::~FileSelectorWindow() {
    delete compressor;
}

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
            DWORD error = GetLastError();
            Logger::logError(L"Failed to register FileSelectorWindow class. Error code: " + std::to_wstring(error));
            return;
        }
        classRegistered = true;
        Logger::logError(L"FileSelectorWindow class registered successfully");
    }
}

void FileSelectorWindow::create(HINSTANCE hInstance, int nCmdShow) {
    static int windowCount = 0;
    windowCount++;
    int xPos = 100 + (windowCount - 1) * 20;
    int yPos = 100 + (windowCount - 1) * 20;

    // Ensure class is registered
    RegisterClass(hInstance);

    hwnd = CreateWindowW(L"FileSelectorWindowClass", L"Select XTurb Output File",
        WS_OVERLAPPEDWINDOW, xPos, yPos, 415, 200, parent, nullptr, hInstance, this);
    if (!hwnd) {
        DWORD error = GetLastError();
        Logger::logError(L"Failed to create FileSelectorWindow. Error code: " + std::to_wstring(error));
        return;
    }

    // Create ComboBox
    comboBox = CreateWindowW(L"COMBOBOX", L"", CBS_DROPDOWNLIST | WS_CHILD | WS_VISIBLE | WS_VSCROLL,
        10, 10, 380, 100, hwnd, (HMENU)1001, hInstance, nullptr);
    if (!comboBox) {
        Logger::logError(L"Failed to create combo box in FileSelectorWindow");
    }

    // Create Select Button
    HWND selectButton = CreateWindowW(L"BUTTON", L"Select", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        150, 70, 100, 30, hwnd, (HMENU)1002, hInstance, nullptr);
    if (!selectButton) {
        Logger::logError(L"Failed to create select button in FileSelectorWindow");
    }

    // Create Save Output Files Button
    HWND saveButton = CreateWindowW(L"BUTTON", L"Save Output Files", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        150, 110, 100, 30, hwnd, (HMENU)1003, hInstance, nullptr);
    if (!saveButton) {
        Logger::logError(L"Failed to create save output files button in FileSelectorWindow");
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
            LRESULT index = SendMessageW(comboBox, CB_GETCURSEL, 0, 0);
            if (index != CB_ERR && index >= 0 && index < files.size()) {
                selectedFile = directory + L"\\" + files[index];
                Logger::logError(L"Selected file: " + selectedFile);
                PostMessage(parent, WM_USER + 102, 0, (LPARAM)this);
            }
            else {
                Logger::logError(L"No file selected");
            }
        }
        else if (LOWORD(wParam) == 1003 && HIWORD(wParam) == BN_CLICKED) {
            if (compressor && compressor->compressFiles()) {
                Logger::logError(L"Successfully compressed output files");
            }
            else {
                Logger::logError(L"Failed to compress output files");
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