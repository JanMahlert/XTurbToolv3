#pragma once
#include "Window.h"
#include "OutputData.h"
#include "GraphControl.h"
#include <vector>

// This class displays the output data from XTurb in a new window (so it naturally derives from Window)
class DataDisplayWindow : public Window {
public:
    DataDisplayWindow(HINSTANCE hInstance, HWND parent, const OutputData& data, const std::wstring& fileName);
    ~DataDisplayWindow() override;
    void create(HINSTANCE hInstance, int nCmdShow) override;
    LRESULT handleMessage(UINT msg, WPARAM wParam, LPARAM lParam) override;
    static void RegisterClass(HINSTANCE hInstance); // Static method to register once

private:
    HWND parent;
    OutputData data;
    std::wstring fileName;
    std::vector<GraphControl*> graphs;
    std::vector<Control*> controls;
	static bool classRegistered; // Added to track the Registration; had a bunch of problems with this, good for debugging
};