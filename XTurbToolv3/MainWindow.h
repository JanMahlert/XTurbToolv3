#pragma once

#include "Window.h"
#include "Container.h"

class MainWindow : public Window {
public:
    MainWindow();
    void create(HINSTANCE hInstance, int nCmdShow) override;
    LRESULT handleMessage(UINT msg, WPARAM wParam, LPARAM lParam) override;

private:
    Container* container;
};