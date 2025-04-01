#pragma once

#include <windows.h>
#include <vector>
#include "InputField.h"
// Removed Control.h include since Graph doesn't inherit from Control in your version

class Graph {
public:
    Graph(HWND parent, HINSTANCE hInstance, int x, int y, int width, int height);
    virtual ~Graph() = default;
    virtual void create(); // Already virtual in your version
    HWND getHandle() const { return hwnd; }
    virtual void draw(HDC hdc, RECT rect) = 0; // Pure virtual method for drawing

protected:
    HWND hwnd;
    HWND parent;
    HINSTANCE hInstance;
    int x;
    int y;
    int width;
    int height;
};

class TwistGraph : public Graph {
public:
    TwistGraph(HWND parent, HINSTANCE hInstance, int x, int y, int width, int height,
        InputField* rtwistInput, InputField* dtwistInput);
    void draw(HDC hdc, RECT rect) override;

private:
    InputField* rtwistInput;
    InputField* dtwistInput;
};

class ChordGraph : public Graph {
public:
    ChordGraph(HWND parent, HINSTANCE hInstance, int x, int y, int width, int height,
        InputField* rtaperInput, InputField* ctaperInput);
    void draw(HDC hdc, RECT rect) override;

private:
    InputField* rtaperInput;
    InputField* ctaperInput;
};