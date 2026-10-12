#pragma once
#include <windows.h>

class Shape;

class Editor
{
public:
    virtual ~Editor() = default;
    virtual void OnLBdown(HWND hWnd) = 0;
    virtual void OnLBup(HWND hWnd) = 0;
    virtual void OnMouseMove(HWND hWnd) = 0;
    virtual void OnPaint(HWND hWnd) = 0;
};

class ShapeEditor : public Editor
{
protected:
    Shape** shapes_{};
    int* shapeCount_{};
    int capacity_{};

    bool dragging_{};
    long xStart_{};
    long yStart_{};
    long xEnd_{};
    long yEnd_{};

    ShapeEditor(Shape** shapes, int* shapeCount, int capacity);

    void AddShape(Shape* shape);
    void DrawRubber(HWND hWnd);

    virtual Shape* CreateShape(long x1, long y1, long x2, long y2) = 0;
    virtual void DrawRubberShape(HDC hdc, long x1, long y1, long x2, long y2) = 0;

public:
    ~ShapeEditor() override = default;

    void OnLBdown(HWND hWnd) override;
    void OnLBup(HWND hWnd) override;
    void OnMouseMove(HWND hWnd) override;
    void OnPaint(HWND hWnd) override;
};

class PointEditor final : public ShapeEditor
{
protected:
    Shape* CreateShape(long x1, long y1, long x2, long y2) override;
    void DrawRubberShape(HDC hdc, long x1, long y1, long x2, long y2) override;

public:
    PointEditor(Shape** shapes, int* shapeCount, int capacity);
    void OnLBdown(HWND hWnd) override;
    void OnLBup(HWND hWnd) override;
    void OnMouseMove(HWND hWnd) override;
};

class LineEditor final : public ShapeEditor
{
protected:
    Shape* CreateShape(long x1, long y1, long x2, long y2) override;
    void DrawRubberShape(HDC hdc, long x1, long y1, long x2, long y2) override;

public:
    LineEditor(Shape** shapes, int* shapeCount, int capacity);
};

class RectEditor final : public ShapeEditor
{
protected:
    Shape* CreateShape(long x1, long y1, long x2, long y2) override;
    void DrawRubberShape(HDC hdc, long x1, long y1, long x2, long y2) override;

public:
    RectEditor(Shape** shapes, int* shapeCount, int capacity);
};

class EllipseEditor final : public ShapeEditor
{
protected:
    Shape* CreateShape(long x1, long y1, long x2, long y2) override;
    void DrawRubberShape(HDC hdc, long x1, long y1, long x2, long y2) override;

public:
    EllipseEditor(Shape** shapes, int* shapeCount, int capacity);
};
