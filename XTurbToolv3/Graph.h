#pragma once
#include <windows.h>
#include <vector>
#include "InputField.h"

// This is the base class for all graph types. Used in the main window and the DataDisplayWindow.
class Graph {
public:
    Graph(HWND parent, HINSTANCE hInstance, int x, int y, int width, int height);
    virtual ~Graph() = default;
    virtual void create();
    HWND getHandle() const { return hwnd; }
    virtual void draw(HDC hdc, RECT rect) = 0; // Pure virtual method since it depends an what needs to be drawn

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