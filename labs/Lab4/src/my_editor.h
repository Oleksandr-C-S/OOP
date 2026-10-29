#pragma once
#include <windows.h>
#include "shape.h"

class MyEditor
{
private:
    Shape** shapes_{ nullptr };
    int count_{ 0 };
    int capacity_{ 0 };

    Shape* prototype_{ nullptr };
    Shape* working_{ nullptr };

    bool dragging_{ false };
    bool rubberVisible_{ false };

    long xStart_{};
    long yStart_{};
    long xEnd_{};
    long yEnd_{};

    HWND toolbar_{ nullptr };
    HWND hostWindow_{ nullptr };
    HIMAGELIST imageList_{ nullptr };

    void EnsureCapacity();
    void AddShape(Shape* shape);
    void DrawRubber(HWND hWnd);
    void SetModeTitle();

public:
    MyEditor() = default;
    ~MyEditor();

    void Start(Shape* shape);

    void OnCreate(HWND hWnd);
    void OnSize(HWND hWnd);
    void OnNotify(HWND hWnd, WPARAM wParam, LPARAM lParam);

    void OnLBdown(HWND hWnd);
    void OnLBup(HWND hWnd);
    void OnMouseMove(HWND hWnd);
    void OnPaint(HWND hWnd, HDC hdc);
};
