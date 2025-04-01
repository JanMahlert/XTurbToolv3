#pragma once
#include "Graph.h"
#include "OutputData.h"
#include <cmath>

class GraphControl : public Graph {
public:
    GraphControl(HWND parent, HINSTANCE hInstance, int x, int y, int width, int height, const OutputData::Table& table);
    void draw(HDC hdc, RECT rect) override;

private:
    OutputData::Table table;
};