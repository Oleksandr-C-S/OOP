#pragma once
#include "shape.h"

class EllipseShape : virtual public Shape
{
public:
    void Show(HDC hdc) override;
    Shape* Clone() const override { return new EllipseShape(*this); }
    const wchar_t* Name() const override { return L"Еліпс"; }
};
