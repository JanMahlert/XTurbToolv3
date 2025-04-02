#pragma once

#include "header.h" 

// Base class for all windows in the application
class Window {
public:
    Window(); 
    virtual ~Window(); 
    virtual void create(HINSTANCE hInstance, int nCmdShow); 
    virtual LRESULT handleMessage(UINT msg, WPARAM wParam, LPARAM lParam); 
    HWND getHwnd() const;

protected:
    HWND hwnd; 
    HINSTANCE hInstance; 
    bool isValid; 
    static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam); 
};