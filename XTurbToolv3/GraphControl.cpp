#include "GraphControl.h"
#include "Logger.h"
#include <algorithm>

GraphControl::GraphControl(HWND parent, HINSTANCE hInstance, int x, int y, int width, int height, const OutputData::Table& table)
    : Graph(parent, hInstance, x, y, width, height), table(table) {
    Logger::logError(L"GraphControl constructed with " + std::to_wstring(table.rows.size()) + L" rows");
    create(); // Explicitly call create()
    if (getHandle()) {
        Logger::logError(L"GraphControl hwnd initialized: " + std::to_wstring(reinterpret_cast<LONG_PTR>(getHandle())));
    }
    else {
        Logger::logError(L"GraphControl hwnd initialization failed");
    }
}

void GraphControl::draw(HDC hdc, RECT rect) {
    Logger::logError(L"GraphControl::draw called for hwnd " + std::to_wstring(reinterpret_cast<LONG_PTR>(getHandle())));
    if (table.headers.empty() || table.rows.empty()) {
        TextOutW(hdc, rect.left + 10, rect.top + 10, L"No data to plot", 14);
        Logger::logError(L"No data to plot in GraphControl");
        return;
    }

    Logger::logError(L"GraphControl plotting " + std::to_wstring(table.rows.size()) + L" rows");
    std::vector<double> xData;
    std::vector<double> yData;
    for (const auto& row : table.rows) {
        if (row.size() >= 2) {
            xData.push_back(row[0]); // r/R
            yData.push_back(row[1]); // Chord/R
            Logger::logError(L"Graph point: " + std::to_wstring(row[0]) + L", " + std::to_wstring(row[1]));
        }
    }

    if (xData.empty() || yData.empty()) {
        TextOutW(hdc, rect.left + 10, rect.top + 10, L"Insufficient data", 17);
        Logger::logError(L"Insufficient data for graph");
        return;
    }

    int graphWidth = rect.right - rect.left - 80;
    int graphHeight = rect.bottom - rect.top - 80;
    int graphLeft = rect.left + 60;
    int graphTop = rect.top + 20;
    int graphBottom = graphTop + graphHeight;

    HPEN hPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
    HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
    MoveToEx(hdc, graphLeft, graphTop, nullptr);
    LineTo(hdc, graphLeft, graphBottom);
    LineTo(hdc, graphLeft + graphWidth, graphBottom);

    double minX = *std::min_element(xData.begin(), xData.end());
    double maxX = *std::max_element(xData.begin(), xData.end());
    double minY = *std::min_element(yData.begin(), yData.end());
    double maxY = *std::max_element(yData.begin(), yData.end());

    double xScale = graphWidth / (maxX - minX);
    double yScale = graphHeight / (maxY - minY);

    HPEN hDataPen = CreatePen(PS_SOLID, 2, RGB(0, 0, 255));
    SelectObject(hdc, hDataPen);
    for (size_t i = 0; i < xData.size(); ++i) {
        int x = graphLeft + static_cast<int>((xData[i] - minX) * xScale);
        int y = graphBottom - static_cast<int>((yData[i] - minY) * yScale);
        if (i == 0) MoveToEx(hdc, x, y, nullptr);
        else LineTo(hdc, x, y);
    }

    SetTextAlign(hdc, TA_CENTER);
    TextOutW(hdc, graphLeft + graphWidth / 2, graphBottom + 10, table.headers[0].c_str(), table.headers[0].length());
    SetTextAlign(hdc, TA_RIGHT);
    TextOutW(hdc, graphLeft - 10, graphTop - 10, table.headers[1].c_str(), table.headers[1].length());

    SelectObject(hdc, hOldPen);
    DeleteObject(hPen);
    DeleteObject(hDataPen);
}