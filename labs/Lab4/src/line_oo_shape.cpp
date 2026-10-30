#include "line_oo_shape.h"

void LineOOShape::Show(HDC hdc)
{
    const long x1 = xs1_;
    const long y1 = ys1_;
    const long x2 = xs2_;
    const long y2 = ys2_;

    LineShape::Show(hdc);

    constexpr long r = 7;

    Set(x1 - r, y1 - r, x1 + r, y1 + r);
    EllipseShape::Show(hdc);

    Set(x2 - r, y2 - r, x2 + r, y2 + r);
    EllipseShape::Show(hdc);

    Set(x1, y1, x2, y2);
}
