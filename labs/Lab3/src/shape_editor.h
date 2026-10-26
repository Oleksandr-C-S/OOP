#pragma once
#include <windows.h>

class Shape;
class ShapeEditor;

class ShapeObjectsEditor
{
private:
    static constexpr int kMaxShapes = 117; // N = Ж + 100 = 17 + 100

    Shape* pcshape_[kMaxShapes]{};          // статичний масив, бо 17 mod 3 != 0
    int shapeCount_{};
    ShapeEditor* currentEditor_{};
    UINT currentCommand_{};
    HWND toolbar_{};

    void ReplaceEditor(ShapeEditor* editor, UINT commandId, HWND hWnd, const wchar_t* modeName);
    void SetModeTitle(HWND hWnd, const wchar_t* modeName);

public:
    ShapeObjectsEditor();
    ~ShapeObjectsEditor();

    ShapeObjectsEditor(const ShapeObjectsEditor&) = delete;
    ShapeObjectsEditor& operator=(const ShapeObjectsEditor&) = delete;

    void OnCreate(HWND hWnd);
    void OnSize(HWND hWnd);
    void OnNotify(HWND hWnd, WPARAM wParam, LPARAM lParam);

    void StartPointEditor(HWND hWnd);
    void StartLineEditor(HWND hWnd);
    void StartRectEditor(HWND hWnd);
    void StartEllipseEditor(HWND hWnd);

    void OnLBdown(HWND hWnd);
    void OnLBup(HWND hWnd);
    void OnMouseMove(HWND hWnd);
    void OnPaint(HWND hWnd);
};
