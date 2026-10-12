#pragma once
#include <windows.h>

class Shape;
class ShapeEditor;

class ShapeObjectsEditor
{
private:
    static constexpr int kMaxShapes = 116; // N = Ж + 100 = 16 + 100

    Shape* pcshape_[kMaxShapes]{};          // статичний масив, бо 16 mod 3 != 0
    int shapeCount_{};
    ShapeEditor* currentEditor_{};
    UINT currentCommand_{};

    void ReplaceEditor(ShapeEditor* editor, UINT commandId, HWND hWnd);

public:
    ShapeObjectsEditor();
    ~ShapeObjectsEditor();

    ShapeObjectsEditor(const ShapeObjectsEditor&) = delete;
    ShapeObjectsEditor& operator=(const ShapeObjectsEditor&) = delete;

    void StartPointEditor(HWND hWnd);
    void StartLineEditor(HWND hWnd);
    void StartRectEditor(HWND hWnd);
    void StartEllipseEditor(HWND hWnd);

    void OnLBdown(HWND hWnd);
    void OnLBup(HWND hWnd);
    void OnMouseMove(HWND hWnd);
    void OnPaint(HWND hWnd);
    void OnInitMenuPopup(HWND hWnd, WPARAM wParam);
};
