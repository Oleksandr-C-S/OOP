#pragma once
#include "line_shape.h"
#include "rect_shape.h"

class CubeShape : public LineShape, public RectShape
{
public:
    void Show(HDC hdc) override;
    Shape* Clone() const override { return new CubeShape(*this); }
    const wchar_t* Name() const override { return L"Каркас куба"; }
};
