#include "cube_shape.h"
#include <cstdlib>

void CubeShape::Show(HDC hdc)
{
    const long x1 = xs1_;
    const long y1 = ys1_;
    const long x2 = xs2_;
    const long y2 = ys2_;

    long dx = (x2 - x1) / 4;
    long dy = -std::labs(y2 - y1) / 4;

    if (dx == 0) dx = 20;
    if (dy == 0) dy = -20;

    const bool oldPreview = preview_;
    preview_ = true; // каркас: прямокутники тільки контуром

    Set(x1, y1, x2, y2);
    RectShape::Show(hdc);

    Set(x1 + dx, y1 + dy, x2 + dx, y2 + dy);
    RectShape::Show(hdc);

    Set(x1, y1, x1 + dx, y1 + dy);
    LineShape::Show(hdc);

    Set(x2, y1, x2 + dx, y1 + dy);
    LineShape::Show(hdc);

    Set(x2, y2, x2 + dx, y2 + dy);
    LineShape::Show(hdc);

    Set(x1, y2, x1 + dx, y2 + dy);
    LineShape::Show(hdc);

    preview_ = oldPreview;
    Set(x1, y1, x2, y2);
}
