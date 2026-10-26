#include "editor.h"
#include "shape.h"
#include <algorithm>
#include <cstdlib>

namespace
{
    POINT CurrentClientPoint(HWND hWnd)
    {
        POINT pt{};
        GetCursorPos(&pt);
        ScreenToClient(hWnd, &pt);
        return pt;
    }

    RECT NormalizeRect(long x1, long y1, long x2, long y2)
    {
        RECT r{};
        r.left = static_cast<LONG>((std::min)(x1, x2));
        r.top = static_cast<LONG>((std::min)(y1, y2));
        r.right = static_cast<LONG>((std::max)(x1, x2));
        r.bottom = static_cast<LONG>((std::max)(y1, y2));
        if (r.right == r.left) ++r.right;
        if (r.bottom == r.top) ++r.bottom;
        return r;
    }

    RECT EllipseBoundsFromCenter(long cx, long cy, long x, long y)
    {
        const long dx = std::labs(x - cx);
        const long dy = std::labs(y - cy);

        RECT r{};
        r.left = static_cast<LONG>(cx - dx);
        r.top = static_cast<LONG>(cy - dy);
        r.right = static_cast<LONG>(cx + dx);
        r.bottom = static_cast<LONG>(cy + dy);
        if (r.right == r.left) ++r.right;
        if (r.bottom == r.top) ++r.bottom;
        return r;
    }
}

ShapeEditor::ShapeEditor(Shape** shapes, int* shapeCount, int capacity)
    : shapes_(shapes), shapeCount_(shapeCount), capacity_(capacity)
{
}

void ShapeEditor::AddShape(Shape* shape)
{
    if (shape == nullptr)
        return;

    if (shapeCount_ != nullptr && shapes_ != nullptr && *shapeCount_ < capacity_)
    {
        shapes_[*shapeCount_] = shape;
        ++(*shapeCount_);
    }
    else
    {
        delete shape;
        MessageBeep(MB_ICONWARNING);
    }
}

void ShapeEditor::DrawRubber(HWND hWnd)
{
    HDC hdc = GetDC(hWnd);
    if (hdc == nullptr)
        return;

    const int oldRop = SetROP2(hdc, R2_NOTXORPEN);

    // Варіант Ж = 16: Ж mod 4 = 0 -> суцільна чорна лінія.
    HPEN pen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
    HPEN oldPen = static_cast<HPEN>(SelectObject(hdc, pen));
    HBRUSH oldBrush = static_cast<HBRUSH>(SelectObject(hdc, GetStockObject(HOLLOW_BRUSH)));

    DrawRubberShape(hdc, xStart_, yStart_, xEnd_, yEnd_);

    SelectObject(hdc, oldBrush);
    SelectObject(hdc, oldPen);
    SetROP2(hdc, oldRop);
    DeleteObject(pen);
    ReleaseDC(hWnd, hdc);
}

void ShapeEditor::OnLBdown(HWND hWnd)
{
    const POINT pt = CurrentClientPoint(hWnd);
    xStart_ = xEnd_ = pt.x;
    yStart_ = yEnd_ = pt.y;
    dragging_ = true;
    SetCapture(hWnd);
}

void ShapeEditor::OnMouseMove(HWND hWnd)
{
    if (!dragging_)
        return;

    DrawRubber(hWnd); // стерти попередній слід

    const POINT pt = CurrentClientPoint(hWnd);
    xEnd_ = pt.x;
    yEnd_ = pt.y;

    DrawRubber(hWnd); // намалювати новий слід
}

void ShapeEditor::OnLBup(HWND hWnd)
{
    if (!dragging_)
        return;

    DrawRubber(hWnd); // стерти останній гумовий слід

    const POINT pt = CurrentClientPoint(hWnd);
    xEnd_ = pt.x;
    yEnd_ = pt.y;

    dragging_ = false;
    ReleaseCapture();

    AddShape(CreateShape(xStart_, yStart_, xEnd_, yEnd_));
    InvalidateRect(hWnd, nullptr, TRUE);
}

void ShapeEditor::OnPaint(HWND hWnd)
{
    PAINTSTRUCT ps{};
    HDC hdc = BeginPaint(hWnd, &ps);

    if (shapes_ != nullptr && shapeCount_ != nullptr)
    {
        for (int i = 0; i < *shapeCount_; ++i)
        {
            if (shapes_[i] != nullptr)
                shapes_[i]->Show(hdc);
        }
    }

    EndPaint(hWnd, &ps);
}

PointEditor::PointEditor(Shape** shapes, int* shapeCount, int capacity)
    : ShapeEditor(shapes, shapeCount, capacity)
{
}

Shape* PointEditor::CreateShape(long x1, long y1, long, long)
{
    return new PointShape(x1, y1);
}

void PointEditor::DrawRubberShape(HDC, long, long, long, long)
{
}

void PointEditor::OnLBdown(HWND hWnd)
{
    const POINT pt = CurrentClientPoint(hWnd);
    AddShape(new PointShape(pt.x, pt.y));
    InvalidateRect(hWnd, nullptr, TRUE);
}

void PointEditor::OnLBup(HWND)
{
}

void PointEditor::OnMouseMove(HWND)
{
}

LineEditor::LineEditor(Shape** shapes, int* shapeCount, int capacity)
    : ShapeEditor(shapes, shapeCount, capacity)
{
}

Shape* LineEditor::CreateShape(long x1, long y1, long x2, long y2)
{
    return new LineShape(x1, y1, x2, y2);
}

void LineEditor::DrawRubberShape(HDC hdc, long x1, long y1, long x2, long y2)
{
    MoveToEx(hdc, static_cast<int>(x1), static_cast<int>(y1), nullptr);
    LineTo(hdc, static_cast<int>(x2), static_cast<int>(y2));
}

RectEditor::RectEditor(Shape** shapes, int* shapeCount, int capacity)
    : ShapeEditor(shapes, shapeCount, capacity)
{
}

Shape* RectEditor::CreateShape(long x1, long y1, long x2, long y2)
{
    // Ж mod 2 = 0 -> прямокутник вводиться по двох протилежних кутах.
    return new RectShape(x1, y1, x2, y2);
}

void RectEditor::DrawRubberShape(HDC hdc, long x1, long y1, long x2, long y2)
{
    const RECT r = NormalizeRect(x1, y1, x2, y2);
    Rectangle(hdc, r.left, r.top, r.right, r.bottom);
}

EllipseEditor::EllipseEditor(Shape** shapes, int* shapeCount, int capacity)
    : ShapeEditor(shapes, shapeCount, capacity)
{
}

Shape* EllipseEditor::CreateShape(long x1, long y1, long x2, long y2)
{
    // Ж mod 2 = 0 -> еліпс вводиться від центру до кута охоплюючого прямокутника.
    const RECT r = EllipseBoundsFromCenter(x1, y1, x2, y2);
    return new EllipseShape(r.left, r.top, r.right, r.bottom);
}

void EllipseEditor::DrawRubberShape(HDC hdc, long x1, long y1, long x2, long y2)
{
    const RECT r = EllipseBoundsFromCenter(x1, y1, x2, y2);
    Ellipse(hdc, r.left, r.top, r.right, r.bottom);
}
