#include "Graph.h"
#include "HelperFunctions.h"
#include <algorithm>

// Window procedure for graph windows
static LRESULT CALLBACK GraphWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    Graph* pGraph = (Graph*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
    if (!pGraph) {
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }

    switch (msg) {
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        RECT rect;
        GetClientRect(hwnd, &rect);
        pGraph->draw(hdc, rect);
        EndPaint(hwnd, &ps);
        return 0;
    }
    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
}

// Graph base class implementation
Graph::Graph(HWND parent, HINSTANCE hInstance, int x, int y, int width, int height)
    : hwnd(nullptr), parent(parent), hInstance(hInstance), x(x), y(y), width(width), height(height) {
}

void Graph::create() {
    WNDCLASSEXW wcex = { 0 };
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = GraphWndProc;
    wcex.hInstance = hInstance;
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszClassName = L"GraphClass";

    RegisterClassExW(&wcex);

    hwnd = CreateWindowW(L"GraphClass", L"", WS_CHILD | WS_VISIBLE | WS_BORDER,
        x, y, width, height, parent, nullptr, hInstance, this);
    if (hwnd) {
        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)this);
    }
}

// TwistGraph implementation
TwistGraph::TwistGraph(HWND parent, HINSTANCE hInstance, int x, int y, int width, int height,
    InputField* rtwistInput, InputField* dtwistInput)
    : Graph(parent, hInstance, x, y, width, height), rtwistInput(rtwistInput), dtwistInput(dtwistInput) {
}

void TwistGraph::draw(HDC hdc, RECT rect) {
    // Get the data
    std::vector<double> rtwistValues = rtwistInput ? parseCommaSeparatedDoubles(rtwistInput->getText()) : std::vector<double>();
    std::vector<double> dtwistValues = dtwistInput ? parseCommaSeparatedDoubles(dtwistInput->getText()) : std::vector<double>();

    if (rtwistValues.empty() || dtwistValues.empty() || rtwistValues.size() != dtwistValues.size()) {
        TextOutW(hdc, rect.left + 10, rect.top + 10, L"No valid data to plot", 20);
        return;
    }

    // Graph dimensions
    int graphWidth = rect.right - rect.left - 80;
    int graphHeight = rect.bottom - rect.top - 80;
    int graphLeft = rect.left + 60;
    int graphTop = rect.top + 30;
    int graphRight = graphLeft + graphWidth;
    int graphBottom = graphTop + graphHeight;

    // Draw axes
    HPEN hPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
    HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
    MoveToEx(hdc, graphLeft, graphTop, nullptr);
    LineTo(hdc, graphLeft, graphBottom); // Y-axis
    LineTo(hdc, graphRight, graphBottom); // X-axis

    // Find min and max values for scaling
    double minR = *std::min_element(rtwistValues.begin(), rtwistValues.end());
    double maxR = *std::max_element(rtwistValues.begin(), rtwistValues.end());
    double minTwist = *std::min_element(dtwistValues.begin(), dtwistValues.end());
    double maxTwist = *std::max_element(dtwistValues.begin(), dtwistValues.end());

    if (minR == maxR || minTwist == maxTwist) {
        TextOutW(hdc, rect.left + 10, rect.top + 10, L"Data range too small to plot", 28);
        SelectObject(hdc, hOldPen);
        DeleteObject(hPen);
        return;
    }

    // Scale the data to fit the graph
    double xScale = graphWidth / (maxR - minR);
    double yScale = graphHeight / (maxTwist - minTwist);

    // Draw the data points and connect them
    HPEN hDataPen = CreatePen(PS_SOLID, 2, RGB(0, 0, 255));
    SelectObject(hdc, hDataPen);

    for (size_t i = 0; i < rtwistValues.size(); ++i) {
        int x = graphLeft + static_cast<int>((rtwistValues[i] - minR) * xScale);
        int y = graphBottom - static_cast<int>((dtwistValues[i] - minTwist) * yScale);

        if (i == 0) {
            MoveToEx(hdc, x, y, nullptr);
        }
        else {
            LineTo(hdc, x, y);
        }

        // Draw a small dot at each data point
        Ellipse(hdc, x - 3, y - 3, x + 3, y + 3);
    }

    // Draw axis labels
    SetTextAlign(hdc, TA_CENTER);
    TextOutW(hdc, (graphLeft + graphRight) / 2, graphBottom + 20, L"Radial Position (r/R)", 21);
    SetTextAlign(hdc, TA_RIGHT);
    TextOutW(hdc, graphLeft - 15, graphTop -20 , L"Twist", 5);

    // Draw numerical labels on the axes
    SetTextAlign(hdc, TA_LEFT);
    wchar_t buffer[32];
    swprintf(buffer, 32, L"%.2f", minR);
    TextOutW(hdc, graphLeft, graphBottom + 5, buffer, wcslen(buffer));
    swprintf(buffer, 32, L"%.2f", maxR);
    TextOutW(hdc, graphRight - 30, graphBottom + 5, buffer, wcslen(buffer));

    SetTextAlign(hdc, TA_RIGHT);
    swprintf(buffer, 32, L"%.2f", maxTwist);
    TextOutW(hdc, graphLeft - 5, graphTop, buffer, wcslen(buffer));
    swprintf(buffer, 32, L"%.2f", minTwist);
    TextOutW(hdc, graphLeft - 5, graphBottom - 15, buffer, wcslen(buffer));

    // Clean up
    SelectObject(hdc, hOldPen);
    DeleteObject(hPen);
    DeleteObject(hDataPen);
}

// ChordGraph implementation
ChordGraph::ChordGraph(HWND parent, HINSTANCE hInstance, int x, int y, int width, int height,
    InputField* rtaperInput, InputField* ctaperInput)
    : Graph(parent, hInstance, x, y, width, height), rtaperInput(rtaperInput), ctaperInput(ctaperInput) {
}

void ChordGraph::draw(HDC hdc, RECT rect) {
    // Get the data
    std::vector<double> rtaperValues = rtaperInput ? parseCommaSeparatedDoubles(rtaperInput->getText()) : std::vector<double>();
    std::vector<double> ctaperValues = ctaperInput ? parseCommaSeparatedDoubles(ctaperInput->getText()) : std::vector<double>();

    if (rtaperValues.empty() || ctaperValues.empty() || rtaperValues.size() != ctaperValues.size()) {
        TextOutW(hdc, rect.left + 10, rect.top + 10, L"No valid data to plot", 20);
        return;
    }

    // Graph dimensions
    int graphWidth = rect.right - rect.left - 80;
    int graphHeight = rect.bottom - rect.top - 80;
    int graphLeft = rect.left + 60;
    int graphTop = rect.top + 30;
    int graphRight = graphLeft + graphWidth;
    int graphBottom = graphTop + graphHeight;

    // Draw axes
    HPEN hPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
    HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
    MoveToEx(hdc, graphLeft, graphTop, nullptr);
    LineTo(hdc, graphLeft, graphBottom); // Y-axis
    LineTo(hdc, graphRight, graphBottom); // X-axis

    // Find min and max values for scaling
    double minR = *std::min_element(rtaperValues.begin(), rtaperValues.end());
    double maxR = *std::max_element(rtaperValues.begin(), rtaperValues.end());
    double minChord = *std::min_element(ctaperValues.begin(), ctaperValues.end());
    double maxChord = *std::max_element(ctaperValues.begin(), ctaperValues.end());

    if (minR == maxR || minChord == maxChord) {
        TextOutW(hdc, rect.left + 10, rect.top + 10, L"Data range too small to plot", 28);
        SelectObject(hdc, hOldPen);
        DeleteObject(hPen);
        return;
    }

    // Scale the data to fit the graph
    double xScale = graphWidth / (maxR - minR);
    double yScale = graphHeight / (maxChord - minChord);

    // Draw the data points and connect them
    HPEN hDataPen = CreatePen(PS_SOLID, 2, RGB(255, 0, 0));
    SelectObject(hdc, hDataPen);

    for (size_t i = 0; i < rtaperValues.size(); ++i) {
        int x = graphLeft + static_cast<int>((rtaperValues[i] - minR) * xScale);
        int y = graphBottom - static_cast<int>((ctaperValues[i] - minChord) * yScale);

        if (i == 0) {
            MoveToEx(hdc, x, y, nullptr);
        }
        else {
            LineTo(hdc, x, y);
        }

        // Draw a small dot at each data point
        Ellipse(hdc, x - 3, y - 3, x + 3, y + 3);
    }

    // Draw axis labels
    SetTextAlign(hdc, TA_CENTER);
    TextOutW(hdc, (graphLeft + graphRight) / 2, graphBottom + 20, L"Radial Position (r/R)", 21);
    SetTextAlign(hdc, TA_RIGHT);
    TextOutW(hdc, graphLeft - 15, graphTop - 20, L"Chord", 5);

    // Draw numerical labels on the axes
    SetTextAlign(hdc, TA_LEFT);
    wchar_t buffer[32];
    swprintf(buffer, 32, L"%.2f", minR);
    TextOutW(hdc, graphLeft, graphBottom + 5, buffer, wcslen(buffer));
    swprintf(buffer, 32, L"%.2f", maxR);
    TextOutW(hdc, graphRight - 30, graphBottom + 5, buffer, wcslen(buffer));

    SetTextAlign(hdc, TA_RIGHT);
    swprintf(buffer, 32, L"%.2f", maxChord);
    TextOutW(hdc, graphLeft - 5, graphTop, buffer, wcslen(buffer));
    swprintf(buffer, 32, L"%.2f", minChord);
    TextOutW(hdc, graphLeft - 5, graphBottom - 15, buffer, wcslen(buffer));

    // Clean up
    SelectObject(hdc, hOldPen);
    DeleteObject(hPen);
    DeleteObject(hDataPen);
}