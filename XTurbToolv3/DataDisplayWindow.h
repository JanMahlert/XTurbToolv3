#pragma once
#include "Window.h"
#include "OutputData.h"
#include "GraphControl.h"
#include <vector>

class DataDisplayWindow : public Window {
public:
    DataDisplayWindow(HINSTANCE hInstance, HWND parent, const OutputData& data);
    ~DataDisplayWindow() override;
    void create(HINSTANCE hInstance, int nCmdShow) override;
    LRESULT handleMessage(UINT msg, WPARAM wParam, LPARAM lParam) override;
    static void RegisterClass(HINSTANCE hInstance); // Static method to register once

private:
    HWND parent;
    OutputData data;
    std::vector<GraphControl*> graphs;
    std::vector<Control*> controls;
    static bool classRegistered; // Track registration state
};