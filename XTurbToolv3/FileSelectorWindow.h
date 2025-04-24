#pragma once
#include "Window.h"
#include <vector>
#include <string>

// Forward declaration of FileCompressor
class FileCompressor;

class FileSelectorWindow : public Window {
public:
    FileSelectorWindow(HINSTANCE hInstance, HWND parent, const std::wstring& directory);
    ~FileSelectorWindow();
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
    static bool classRegistered;
    FileCompressor* compressor;
};