#include "ellipse_shape.h"

void EllipseShape::Show(HDC hdc)
{
    HBRUSH oldBrush = static_cast<HBRUSH>(
        SelectObject(hdc, GetStockObject(HOLLOW_BRUSH)));

    Ellipse(
        hdc,
        static_cast<int>(xs1_),
        static_cast<int>(ys1_),
        static_cast<int>(xs2_),
        static_cast<int>(ys2_));

    SelectObject(hdc, oldBrush);
}
