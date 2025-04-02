#pragma once
#include "Window.h"
#include "Container.h"

// This is the main window. Everthing inside this window is a child of this window.
class MainWindow : public Window {
public:
    MainWindow();
    void create(HINSTANCE hInstance, int nCmdShow) override;
    LRESULT handleMessage(UINT msg, WPARAM wParam, LPARAM lParam) override;

private:
    Container* container;
};