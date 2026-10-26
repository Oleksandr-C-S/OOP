#pragma once
#include <windows.h>

class Shape
{
protected:
    long xs1_{};
    long ys1_{};
    long xs2_{};
    long ys2_{};

public:
    virtual ~Shape() = default;

    void Set(long x1, long y1, long x2, long y2);
    virtual void Show(HDC hdc) const = 0;
};

class PointShape final : public Shape
{
public:
    PointShape(long x, long y);
    void Show(HDC hdc) const override;
};

class LineShape final : public Shape
{
public:
    LineShape(long x1, long y1, long x2, long y2);
    void Show(HDC hdc) const override;
};

class RectShape final : public Shape
{
public:
    RectShape(long x1, long y1, long x2, long y2);
    void Show(HDC hdc) const override;
};

class EllipseShape final : public Shape
{
public:
    EllipseShape(long x1, long y1, long x2, long y2);
    void Show(HDC hdc) const override;
};
