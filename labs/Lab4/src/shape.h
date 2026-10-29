#pragma once
#include <windows.h>

class Shape
{
protected:
    long xs1_{};
    long ys1_{};
    long xs2_{};
    long ys2_{};
    bool preview_{ false };

public:
    virtual ~Shape() = default;

    void Set(long x1, long y1, long x2, long y2);
    void SetPreview(bool value) { preview_ = value; }

    virtual void Show(HDC hdc) = 0;
    virtual Shape* Clone() const = 0;
    virtual const wchar_t* Name() const = 0;
    virtual bool NeedsDrag() const { return true; }
};
