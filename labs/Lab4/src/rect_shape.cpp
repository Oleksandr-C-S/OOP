#include "rect_shape.h"

void RectShape::Show(HDC hdc)
{
    HBRUSH brush = preview_
        ? static_cast<HBRUSH>(GetStockObject(HOLLOW_BRUSH))
        : CreateSolidBrush(RGB(192, 192, 192));

    HBRUSH oldBrush = static_cast<HBRUSH>(SelectObject(hdc, brush));

    Rectangle(
        hdc,
        static_cast<int>(xs1_),
        static_cast<int>(ys1_),
        static_cast<int>(xs2_),
        static_cast<int>(ys2_));

    SelectObject(hdc, oldBrush);

    if (!preview_)
        DeleteObject(brush);
}
