#pragma once
#include "shape.h"

class LineShape : virtual public Shape
{
public:
    void Show(HDC hdc) override;
    Shape* Clone() const override { return new LineShape(*this); }
    const wchar_t* Name() const override { return L"Лінія"; }
};
