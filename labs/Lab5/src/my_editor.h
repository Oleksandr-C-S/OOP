#pragma once
#include <windows.h>
#include "shape.h"

class MyTable;

class MyEditor
{
private:
    static MyEditor* p_instance_;

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
    MyTable* table_{ nullptr };

    MyEditor() = default;
    ~MyEditor();

    MyEditor(const MyEditor&) = delete;
    MyEditor& operator=(const MyEditor&) = delete;

    void EnsureCapacity();
    void AddShape(Shape* shape);
    void DrawRubber(HWND hWnd);
    void SetModeTitle();

    void ResetOutputFile();
    void AppendShapeToFile(const Shape& shape) const;

public:
    static MyEditor* GetInstance();
    static void DestroyInstance();

    void AttachTable(MyTable* table) { table_ = table; }
    void Start(Shape* shape);

    void OnCreate(HWND hWnd);
    void OnSize(HWND hWnd);
    void OnNotify(HWND hWnd, WPARAM wParam, LPARAM lParam);

    void OnLBdown(HWND hWnd);
    void OnLBup(HWND hWnd);
    void OnMouseMove(HWND hWnd);
    void OnPaint(HWND hWnd, HDC hdc);

    bool SaveToFile(const char* fileName = "objects.txt") const;
};
