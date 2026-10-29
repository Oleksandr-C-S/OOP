#include "point_shape.h"

void PointShape::Show(HDC hdc)
{
    SetPixel(hdc, static_cast<int>(xs1_), static_cast<int>(ys1_), RGB(0, 0, 0));
}
