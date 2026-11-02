#pragma once
#include "shape.h"

class RectShape : virtual public Shape
{
public:
    void Show(HDC hdc) override;
    Shape* Clone() const override { return new RectShape(*this); }
    const wchar_t* Name() const override { return L"Прямокутник"; }
};
