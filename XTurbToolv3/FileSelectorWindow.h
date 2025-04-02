#pragma once
#include "Window.h"
#include <vector>
#include <string>

// This class creates a Window where you can select the .dat files that XTurb created (akso derived from Window, since it creates a new window)
class FileSelectorWindow : public Window {
public:
    FileSelectorWindow(HINSTANCE hInstance, HWND parent, const std::wstring& directory);
    void create(HINSTANCE hInstance, int nCmdShow) override;
    LRESULT handleMessage(UINT msg, WPARAM wParam, LPARAM lParam) override;
    std::wstring getSelectedFile() const { return selectedFile; }
    void refreshFileList();
    static void RegisterClass(HINSTANCE hInstance); 

private:
    HWND parent;
    std::wstring directory;
    std::vector<std::wstring> files;
    HWND comboBox;
    std::wstring selectedFile;
    static bool classRegistered; // Also tracks the class registration
};