#include "DataDisplayWindow.h"
#include "Label.h"
#include "InputField.h"
#include "Logger.h"

bool DataDisplayWindow::classRegistered = false;

void DataDisplayWindow::RegisterClass(HINSTANCE hInstance) {
    if (!classRegistered) {
        WNDCLASSEXW wcex = { 0 };
        wcex.cbSize = sizeof(WNDCLASSEX);
        wcex.style = CS_HREDRAW | CS_VREDRAW;
        wcex.lpfnWndProc = Window::WndProc;
        wcex.hInstance = hInstance;
        wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
        wcex.lpszClassName = L"DataDisplayWindowClass";
        if (!RegisterClassExW(&wcex)) {
            Logger::logError(L"Failed to register DataDisplayWindow class");
            return;
        }
        classRegistered = true;
        Logger::logError(L"DataDisplayWindow class registered successfully");
    }
}

DataDisplayWindow::DataDisplayWindow(HINSTANCE hInstance, HWND parent, const OutputData& data, const std::wstring& fileName)
    : Window(), parent(parent), data(data), fileName(fileName) {
    this->hInstance = hInstance;
    Logger::logError(L"DataDisplayWindow constructed with " + std::to_wstring(data.tables.size()) + L" tables");
}

DataDisplayWindow::~DataDisplayWindow() {
    for (auto graph : graphs) delete graph;
    for (auto control : controls) delete control;
    graphs.clear();
    controls.clear();
}

void DataDisplayWindow::create(HINSTANCE hInstance, int nCmdShow) {
    RegisterClass(hInstance);

    hwnd = CreateWindowW(L"DataDisplayWindowClass", fileName.c_str(),
        WS_OVERLAPPEDWINDOW | WS_VSCROLL, CW_USEDEFAULT, 0, 700, 400, parent, nullptr, hInstance, this);

    if (!hwnd) {
        Logger::logError(L"Failed to create DataDisplayWindow instance");
        return;
    }

    int y = 10;
    const int labelWidth = 200;
    const int inputWidth = 300;
    const int height = 20;
    const int spacing = 10;

    Logger::logError(L"Creating single value controls");
    for (const auto& [key, value] : data.singleValues) {
        Label* label = new Label(hwnd, hInstance, 10, y, labelWidth, height, key + L":");
        label->create();
        controls.push_back(label);

        InputField* field = new InputField(hwnd, hInstance, 10 + labelWidth + 10, y, inputWidth, height, (HMENU)(1000 + y));
        field->create();
        field->setDefaultText(value);
        SendMessageW(field->getHandle(), EM_SETREADONLY, TRUE, 0);
        controls.push_back(field);

        y += height + spacing;
    }

    Logger::logError(L"Creating graphs for " + std::to_wstring(data.tables.size()) + L" tables");
    for (const auto& table : data.tables) {
        if (table.headers.size() < 2) { // Need at least 2 columns for a graph
            Logger::logError(L"Table has fewer than 2 columns, skipping graphs");
            continue;
        }

        // Create a graph for each column (except the first, "r/R") vs. "r/R"
        for (size_t col = 1; col < table.headers.size(); ++col) {
            OutputData::Table graphTable;
            graphTable.headers = { table.headers[0], table.headers[col] }; // e.g., "r/R" vs. "Chord/R"
            for (const auto& row : table.rows) {
                if (row.size() > col) {
                    graphTable.rows.push_back({ row[0], row[col] }); // Pair "r/R" with the current column
                }
            }

            GraphControl* graph = new GraphControl(hwnd, hInstance, 10, y, 580, 200, graphTable);
            graphs.push_back(graph);
            HWND graphHwnd = graph->getHandle();
            if (graphHwnd) {
                Logger::logError(L"GraphControl created at y=" + std::to_wstring(y) + L" with hwnd " + std::to_wstring(reinterpret_cast<LONG_PTR>(graphHwnd)));
                InvalidateRect(graphHwnd, nullptr, TRUE);
                UpdateWindow(graphHwnd);
            }
            else {
                Logger::logError(L"GraphControl creation failed at y=" + std::to_wstring(y));
            }
            y += 210; // Height of graph (200) + spacing (10)
        }
    }

    SCROLLINFO si = { 0 };
    si.cbSize = sizeof(SCROLLINFO);
    si.fMask = SIF_RANGE | SIF_PAGE;
    si.nMin = 0;
    si.nMax = y;
    si.nPage = 400;
    SetScrollInfo(hwnd, SB_VERT, &si, TRUE);

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);
}

LRESULT DataDisplayWindow::handleMessage(UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_VSCROLL: {
        SCROLLINFO si = { 0 };
        si.cbSize = sizeof(SCROLLINFO);
        si.fMask = SIF_ALL;
        GetScrollInfo(hwnd, SB_VERT, &si);
        int oldPos = si.nPos;

        switch (LOWORD(wParam)) {
        case SB_LINEUP: si.nPos -= 10; break;
        case SB_LINEDOWN: si.nPos += 10; break;
        case SB_PAGEUP: si.nPos -= si.nPage; break;
        case SB_PAGEDOWN: si.nPos += si.nPage; break;
        case SB_THUMBTRACK: si.nPos = HIWORD(wParam); break;
        }

        si.nPos = max(si.nMin, min(si.nMax - (int)si.nPage, si.nPos));
        if (si.nPos != oldPos) {
            si.fMask = SIF_POS;
            SetScrollInfo(hwnd, SB_VERT, &si, TRUE);
            ScrollWindow(hwnd, 0, oldPos - si.nPos, nullptr, nullptr);
            UpdateWindow(hwnd);
        }
        return 0;
    }
    case WM_CLOSE:
        DestroyWindow(hwnd);
        break;
    case WM_DESTROY:
        // No PostQuitMessage; cleanup handled by Container
        break;
    default:
        return Window::handleMessage(msg, wParam, lParam);
    }
    return 0;
}