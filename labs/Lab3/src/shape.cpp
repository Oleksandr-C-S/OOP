#include "shape.h"
#include <algorithm>

namespace
{
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
}

void Shape::Set(long x1, long y1, long x2, long y2)
{
    xs1_ = x1;
    ys1_ = y1;
    xs2_ = x2;
    ys2_ = y2;
}

PointShape::PointShape(long x, long y)
{
    Set(x, y, x, y);
}

void PointShape::Show(HDC hdc) const
{
    SetPixel(hdc, static_cast<int>(xs1_), static_cast<int>(ys1_), RGB(0, 0, 0));
}

LineShape::LineShape(long x1, long y1, long x2, long y2)
{
    Set(x1, y1, x2, y2);
}

void LineShape::Show(HDC hdc) const
{
    HPEN pen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
    HPEN oldPen = static_cast<HPEN>(SelectObject(hdc, pen));

    MoveToEx(hdc, static_cast<int>(xs1_), static_cast<int>(ys1_), nullptr);
    LineTo(hdc, static_cast<int>(xs2_), static_cast<int>(ys2_));

    SelectObject(hdc, oldPen);
    DeleteObject(pen);
}

RectShape::RectShape(long x1, long y1, long x2, long y2)
{
    Set(x1, y1, x2, y2);
}

void RectShape::Show(HDC hdc) const
{
    const RECT r = NormalizeRect(xs1_, ys1_, xs2_, ys2_);

    HPEN pen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
    HBRUSH brush = CreateSolidBrush(RGB(192, 192, 192)); // Ж=17: 17 mod 6 = 5 -> сірий

    HPEN oldPen = static_cast<HPEN>(SelectObject(hdc, pen));
    HBRUSH oldBrush = static_cast<HBRUSH>(SelectObject(hdc, brush));

    Rectangle(hdc, r.left, r.top, r.right, r.bottom);

    SelectObject(hdc, oldBrush);
    SelectObject(hdc, oldPen);
    DeleteObject(brush);
    DeleteObject(pen);
}

EllipseShape::EllipseShape(long x1, long y1, long x2, long y2)
{
    Set(x1, y1, x2, y2);
}

void EllipseShape::Show(HDC hdc) const
{
    const RECT r = NormalizeRect(xs1_, ys1_, xs2_, ys2_);

    HPEN pen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
    HPEN oldPen = static_cast<HPEN>(SelectObject(hdc, pen));
    HBRUSH oldBrush = static_cast<HBRUSH>(SelectObject(hdc, GetStockObject(HOLLOW_BRUSH)));

    // Ж=17: 17 mod 5 = 2 -> еліпс без заповнення.
    Ellipse(hdc, r.left, r.top, r.right, r.bottom);

    SelectObject(hdc, oldBrush);
    SelectObject(hdc, oldPen);
    DeleteObject(pen);
}
