#include "GraphControl.h"
#include "Logger.h"
#include <algorithm>
#include <limits> // For std::numeric_limits

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
    for (size_t i = 0; i < table.rows.size(); ++i) {
        const auto& row = table.rows[i];
        if (row.size() >= 2) {
            double x = row[0]; // r/R
            double y = row[1]; // e.g., Chord/R
            if (std::isnan(x) || std::isnan(y)) {
                Logger::logError(L"Skipping point due to NaN: row " + std::to_wstring(i));
                continue;
            }
            xData.push_back(x);
            yData.push_back(y);
            Logger::logError(L"Graph point: " + std::to_wstring(x) + L", " + std::to_wstring(y));
        }
    }

    if (xData.empty() || yData.empty()) {
        TextOutW(hdc, rect.left + 10, rect.top + 10, L"Insufficient data", 17);
        Logger::logError(L"Insufficient data for graph");
        return;
    }

    int graphWidth = rect.right - rect.left - 150;
    int graphHeight = rect.bottom - rect.top - 80;
	int graphLeft = rect.left + 100; // Leave space for Y-axis labels and title
	int graphTop = rect.top + 40; // Adjust top to leave space for X-axis labels and title
    int graphBottom = graphTop + graphHeight;
    int graphRight = graphLeft + graphWidth;

    HPEN hPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
    HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
    MoveToEx(hdc, graphLeft, graphTop, nullptr);
    LineTo(hdc, graphLeft, graphBottom);
    LineTo(hdc, graphLeft + graphWidth, graphBottom);

    double minX = *std::min_element(xData.begin(), xData.end());
    double maxX = *std::max_element(xData.begin(), xData.end());
    double minY = *std::min_element(yData.begin(), yData.end());
    double maxY = *std::max_element(yData.begin(), yData.end());

    if (minX >= maxX || minY >= maxY) {
        TextOutW(hdc, rect.left + 10, rect.top + 10, L"Invalid range", 13);
        Logger::logError(L"GraphControl: Invalid range for plotting");
        SelectObject(hdc, hOldPen);
        DeleteObject(hPen);
        return;
    }

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

    // Add min/max labels (mimicking TwistGraph/ChordGraph style)
    wchar_t buffer[32];

    // X-axis labels (r/R)
    SetTextAlign(hdc, TA_LEFT);
    swprintf(buffer, 32, L"%.2f", minX);
    TextOutW(hdc, graphLeft, graphBottom + 5, buffer, wcslen(buffer));
    swprintf(buffer, 32, L"%.2f", maxX);
    TextOutW(hdc, graphRight - 30, graphBottom + 5, buffer, wcslen(buffer));

    // Y-axis labels (e.g., Chord/R)
    SetTextAlign(hdc, TA_RIGHT);
    swprintf(buffer, 32, L"%.2f", maxY);
    TextOutW(hdc, graphLeft - 5, graphTop, buffer, wcslen(buffer));
    swprintf(buffer, 32, L"%.2f", minY);
    TextOutW(hdc, graphLeft - 5, graphBottom - 15, buffer, wcslen(buffer));

    // Axis titles (unchanged)
    SetTextAlign(hdc, TA_CENTER);
    TextOutW(hdc, graphLeft + graphWidth / 2, graphBottom + 10, table.headers[0].c_str(), table.headers[0].length());
    SetTextAlign(hdc, TA_RIGHT);
    TextOutW(hdc, graphLeft - 20, graphTop - 35, table.headers[1].c_str(), table.headers[1].length());

    SelectObject(hdc, hOldPen);
    DeleteObject(hPen);
    DeleteObject(hDataPen);
}