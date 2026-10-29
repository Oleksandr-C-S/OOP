#include "line_shape.h"

void LineShape::Show(HDC hdc)
{
    MoveToEx(hdc, static_cast<int>(xs1_), static_cast<int>(ys1_), nullptr);
    LineTo(hdc, static_cast<int>(xs2_), static_cast<int>(ys2_));
}
