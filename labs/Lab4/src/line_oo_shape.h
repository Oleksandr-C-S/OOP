#pragma once
#include "line_shape.h"
#include "ellipse_shape.h"

class LineOOShape : public LineShape, public EllipseShape
{
public:
    void Show(HDC hdc) override;
    Shape* Clone() const override { return new LineOOShape(*this); }
    const wchar_t* Name() const override { return L"Лінія з кружечками"; }
};
