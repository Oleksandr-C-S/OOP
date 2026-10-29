#pragma once
#include "shape.h"

class PointShape : public Shape
{
public:
    void Show(HDC hdc) override;
    Shape* Clone() const override { return new PointShape(*this); }
    const wchar_t* Name() const override { return L"Крапка"; }
    bool NeedsDrag() const override { return false; }
};
