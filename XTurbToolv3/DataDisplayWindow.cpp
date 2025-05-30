#include "DataDisplayWindow.h"
#include "GraphControl.h"
#include "Label.h"
#include "InputField.h"
#include "Logger.h"
#include <fstream>
#include <sstream>

bool DataDisplayWindow::classRegistered = false;

// Register the window class for DataDisplayWindow if not already registered
void DataDisplayWindow::RegisterClass(HINSTANCE hInstance) {
    if (!classRegistered) {
        WNDCLASSEXW wcex = { 0 };
        wcex.cbSize = sizeof(WNDCLASSEX);
        wcex.style = CS_HREDRAW | CS_VREDRAW;
        wcex.lpfnWndProc = Window::WndProc;
        wcex.hInstance = hInstance;
        wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
        wcex.lpszClassName = L"DataDisplayWindowClass";
        // If window class registration fails, log the error and exit the function
        if (!RegisterClassExW(&wcex)) {
            Logger::logError(L"Failed to register DataDisplayWindow class");
            return;
        }
        classRegistered = true;
        Logger::logError(L"DataDisplayWindow class registered successfully");
    }
}

// Constructor: initialize DataDisplayWindow with parsed data, parent window, and file info, then log table count
DataDisplayWindow::DataDisplayWindow(HINSTANCE hInstance, HWND parent, const OutputData& data, const std::wstring& filePath)
    : Window(), parent(parent), data(data), fileName(filePath) {
    this->hInstance = hInstance;
    Logger::logError(L"DataDisplayWindow constructed with " + std::to_wstring(data.tables.size()) + L" tables");
}

// Destructor: delete all dynamically allocated graphs and controls, then clear the containers
DataDisplayWindow::~DataDisplayWindow() {
    for (auto graph : graphs) delete graph;
    for (auto control : controls) delete control;
    graphs.clear();
    controls.clear();
}

void DataDisplayWindow::create(HINSTANCE hInstance, int nCmdShow) {
    RegisterClass(hInstance);

    // Use only the filename for the window title
    std::wstring title = fileName.substr(fileName.find_last_of(L"\\") + 1);
    hwnd = CreateWindowW(L"DataDisplayWindowClass", title.c_str(),
        WS_OVERLAPPEDWINDOW | WS_VSCROLL, CW_USEDEFAULT, 0, 1000, 600, parent, nullptr, hInstance, this);

    if (!hwnd) {
        Logger::logError(L"Failed to create DataDisplayWindow instance");
        return;
    }

    int y = 10;
    const int spacing = 10;

    // Use the headerText from OutputData
    std::wstring headerText = data.headerText;

    // Remove trailing newline
    if (!headerText.empty() && headerText.back() == L'\n') {
        headerText.pop_back();
        if (!headerText.empty() && headerText.back() == L'\r') {
            headerText.pop_back();
        }
    }

    Logger::logError(L"Creating header text block with content:\n" + headerText);

    // Adjust text field size here (width, height)
    const int textFieldWidth = 900;  // Adjust width as needed
    const int textFieldHeight = 300; // Adjust height as needed (increased from 200 to 250 for more visibility)

    HWND textBox = CreateWindowW(L"EDIT", headerText.c_str(),
        WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_MULTILINE | ES_READONLY | WS_BORDER,
        10, y, textFieldWidth, textFieldHeight, hwnd, (HMENU)1001, hInstance, nullptr);

    // If textBox is created, apply a fixed-width font and add a new input field control to the window layout
    if (!textBox) {
        Logger::logError(L"Failed to create header text box");
    }
    else {
        // Set a fixed-width font with adjustable size
        HFONT hFont = CreateFontW(-11, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, // -10 for ~10pt font (smaller)
            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
            DEFAULT_QUALITY, FIXED_PITCH | FF_DONTCARE, L"Consolas");
        if (hFont) {
            SendMessage(textBox, WM_SETFONT, (WPARAM)hFont, TRUE);
        }
        controls.push_back(new InputField(hwnd, hInstance, 10, y, textFieldWidth, textFieldHeight, (HMENU)1001));
        y += textFieldHeight + spacing;
    }

    // Log how many tables will be used to create graphs
    Logger::logError(L"Creating graphs for " + std::to_wstring(data.tables.size()) + L" tables");
    for (const auto& table : data.tables) {
        // Skip tables that don't have at least 2 columns (X and Y axes)
        if (table.headers.size() < 2) {
            Logger::logError(L"Table has fewer than 2 columns, skipping graphs");
            continue;
        }
        // For each column beyond the first, create a graph of column[0] vs column[n]
        for (size_t col = 1; col < table.headers.size(); ++col) {
            // Build a new table containing only the two relevant columns
            OutputData::Table graphTable;
            graphTable.headers = { table.headers[0], table.headers[col] };
            // Populate rows for the graph with corresponding column data
            for (const auto& row : table.rows) {
                if (row.size() > col) {
                    graphTable.rows.push_back({ row[0], row[col] });
                }
            }

            // Create a GraphControl using the two-column table
            GraphControl* graph = new GraphControl(hwnd, hInstance, 10, y, 700, 400, graphTable);
            graphs.push_back(graph);
            // Retrieve the handle to the graph window
            HWND graphHwnd = graph->getHandle();
            if (graphHwnd) {
                Logger::logError(L"GraphControl created at y=" + std::to_wstring(y) + L" with hwnd " + std::to_wstring(reinterpret_cast<LONG_PTR>(graphHwnd)));
                InvalidateRect(graphHwnd, nullptr, TRUE);
                UpdateWindow(graphHwnd);
            }
            else {
                Logger::logError(L"GraphControl creation failed at y=" + std::to_wstring(y));
            }
            // Increment y position for the next graph to avoid overlap
            y += 410;
        }
    }

    // Set up a vertical scrollbar for the window and make the window visible, updating it to reflect changes
    SCROLLINFO si = { 0 };
    si.cbSize = sizeof(SCROLLINFO);
    si.fMask = SIF_RANGE | SIF_PAGE;
    si.nMin = 0;
    si.nMax = y;
    si.nPage = 600;
    SetScrollInfo(hwnd, SB_VERT, &si, TRUE);

    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);
}

// Handle vertical scroll events, window close, and forward unhandled messages to the base class
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
        break;
    default:
        return Window::handleMessage(msg, wParam, lParam);
    }
    return 0;
}