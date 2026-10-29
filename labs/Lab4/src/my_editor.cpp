#include <windows.h>
#include <commctrl.h>
#include "my_editor.h"
#include "resource.h"

#pragma comment(lib, "comctl32.lib")

namespace
{
    void CursorToClient(HWND hWnd, long& x, long& y)
    {
        POINT pt{};
        GetCursorPos(&pt);
        ScreenToClient(hWnd, &pt);
        x = pt.x;
        y = pt.y;
    }
}

MyEditor::~MyEditor()
{
    delete prototype_;
    delete working_;

    for (int i = 0; i < count_; ++i)
        delete shapes_[i];

    delete[] shapes_;

    if (imageList_ != nullptr)
        ImageList_Destroy(imageList_);
}

void MyEditor::EnsureCapacity()
{
    if (count_ < capacity_)
        return;

    const int newCapacity = (capacity_ == 0) ? 16 : capacity_ * 2;
    Shape** newArray = new Shape*[newCapacity]{};

    for (int i = 0; i < count_; ++i)
        newArray[i] = shapes_[i];

    delete[] shapes_;
    shapes_ = newArray;
    capacity_ = newCapacity;
}

void MyEditor::AddShape(Shape* shape)
{
    if (shape == nullptr)
        return;

    EnsureCapacity();
    shapes_[count_++] = shape;
}

void MyEditor::SetModeTitle()
{
    if (hostWindow_ == nullptr || prototype_ == nullptr)
        return;

    wchar_t title[180]{};
    wsprintfW(
        title,
        L"Лабораторна робота №4 - Чакун Олександр Сергійович - Режим: %s",
        prototype_->Name());

    SetWindowTextW(hostWindow_, title);
}

void MyEditor::Start(Shape* shape)
{
    if (shape == nullptr)
        return;

    if (rubberVisible_ && hostWindow_ != nullptr)
        DrawRubber(hostWindow_);

    rubberVisible_ = false;
    dragging_ = false;

    delete working_;
    working_ = nullptr;

    delete prototype_;
    prototype_ = shape;

    SetModeTitle();
}

void MyEditor::OnCreate(HWND hWnd)
{
    hostWindow_ = hWnd;

    toolbar_ = CreateWindowExW(
        0,
        TOOLBARCLASSNAMEW,
        nullptr,
        WS_CHILD | WS_VISIBLE | TBSTYLE_FLAT | TBSTYLE_TOOLTIPS | CCS_TOP,
        0, 0, 0, 0,
        hWnd,
        reinterpret_cast<HMENU>(static_cast<INT_PTR>(IDC_MAIN_TOOLBAR)),
        GetModuleHandleW(nullptr),
        nullptr);

    if (toolbar_ == nullptr)
        return;

    SendMessageW(toolbar_, TB_BUTTONSTRUCTSIZE, sizeof(TBBUTTON), 0);
    SendMessageW(toolbar_, TB_SETBITMAPSIZE, 0, MAKELPARAM(24, 24));
    SendMessageW(toolbar_, TB_SETBUTTONSIZE, 0, MAKELPARAM(30, 30));

    HBITMAP bitmap = static_cast<HBITMAP>(
        LoadImageW(
            GetModuleHandleW(nullptr),
            MAKEINTRESOURCEW(IDB_TOOLBAR),
            IMAGE_BITMAP,
            0,
            0,
            LR_CREATEDIBSECTION));

    if (bitmap != nullptr)
    {
        imageList_ = ImageList_Create(24, 24, ILC_COLOR24 | ILC_MASK, 6, 0);

        if (imageList_ != nullptr)
        {
            ImageList_AddMasked(imageList_, bitmap, RGB(255, 0, 255));
            SendMessageW(
                toolbar_,
                TB_SETIMAGELIST,
                0,
                reinterpret_cast<LPARAM>(imageList_));
        }

        DeleteObject(bitmap);
    }

    const UINT commands[6] =
    {
        IDM_POINT,
        IDM_LINE,
        IDM_RECT,
        IDM_ELLIPSE,
        IDM_LINEOO,
        IDM_CUBE
    };

    TBBUTTON buttons[6]{};

    for (int i = 0; i < 6; ++i)
    {
        buttons[i].iBitmap = i;
        buttons[i].idCommand = commands[i];
        buttons[i].fsState = TBSTATE_ENABLED;
        buttons[i].fsStyle = BTNS_BUTTON;
    }

    SendMessageW(
        toolbar_,
        TB_ADDBUTTONS,
        6,
        reinterpret_cast<LPARAM>(buttons));

    SendMessageW(toolbar_, TB_AUTOSIZE, 0, 0);
    ShowWindow(toolbar_, SW_SHOW);
}

void MyEditor::OnSize(HWND)
{
    if (toolbar_ != nullptr)
        SendMessageW(toolbar_, TB_AUTOSIZE, 0, 0);
}

void MyEditor::OnNotify(HWND, WPARAM, LPARAM lParam)
{
    const LPNMHDR header = reinterpret_cast<LPNMHDR>(lParam);

    if (header == nullptr || header->code != TTN_GETDISPINFOW)
        return;

    auto* info = reinterpret_cast<NMTTDISPINFOW*>(lParam);

    switch (static_cast<UINT>(info->hdr.idFrom))
    {
    case IDM_POINT:
        info->lpszText = const_cast<LPWSTR>(L"Крапка");
        break;
    case IDM_LINE:
        info->lpszText = const_cast<LPWSTR>(L"Лінія");
        break;
    case IDM_RECT:
        info->lpszText = const_cast<LPWSTR>(L"Прямокутник");
        break;
    case IDM_ELLIPSE:
        info->lpszText = const_cast<LPWSTR>(L"Еліпс");
        break;
    case IDM_LINEOO:
        info->lpszText = const_cast<LPWSTR>(L"Лінія з кружечками");
        break;
    case IDM_CUBE:
        info->lpszText = const_cast<LPWSTR>(L"Каркас куба");
        break;
    default:
        break;
    }
}

void MyEditor::DrawRubber(HWND hWnd)
{
    if (working_ == nullptr)
        return;

    HDC hdc = GetDC(hWnd);
    if (hdc == nullptr)
        return;

    const int oldRop = SetROP2(hdc, R2_NOTXORPEN);

    HPEN pen = CreatePen(PS_DOT, 1, RGB(0, 0, 0));
    HPEN oldPen = static_cast<HPEN>(SelectObject(hdc, pen));
    HBRUSH oldBrush = static_cast<HBRUSH>(
        SelectObject(hdc, GetStockObject(HOLLOW_BRUSH)));

    working_->SetPreview(true);
    working_->Show(hdc);
    working_->SetPreview(false);

    SelectObject(hdc, oldBrush);
    SelectObject(hdc, oldPen);
    DeleteObject(pen);

    SetROP2(hdc, oldRop);
    ReleaseDC(hWnd, hdc);
}

void MyEditor::OnLBdown(HWND hWnd)
{
    if (prototype_ == nullptr)
        return;

    CursorToClient(hWnd, xStart_, yStart_);
    xEnd_ = xStart_;
    yEnd_ = yStart_;

    if (!prototype_->NeedsDrag())
    {
        Shape* shape = prototype_->Clone();
        shape->Set(xStart_, yStart_, xStart_, yStart_);
        AddShape(shape);
        InvalidateRect(hWnd, nullptr, FALSE);
        return;
    }

    delete working_;
    working_ = prototype_->Clone();
    working_->Set(xStart_, yStart_, xEnd_, yEnd_);

    dragging_ = true;
    rubberVisible_ = false;
}

void MyEditor::OnMouseMove(HWND hWnd)
{
    if (!dragging_ || working_ == nullptr)
        return;

    if (rubberVisible_)
        DrawRubber(hWnd);

    CursorToClient(hWnd, xEnd_, yEnd_);
    working_->Set(xStart_, yStart_, xEnd_, yEnd_);

    DrawRubber(hWnd);
    rubberVisible_ = true;
}

void MyEditor::OnLBup(HWND hWnd)
{
    if (!dragging_ || working_ == nullptr)
        return;

    if (rubberVisible_)
        DrawRubber(hWnd);

    CursorToClient(hWnd, xEnd_, yEnd_);
    working_->Set(xStart_, yStart_, xEnd_, yEnd_);
    working_->SetPreview(false);

    AddShape(working_);
    working_ = nullptr;

    dragging_ = false;
    rubberVisible_ = false;

    InvalidateRect(hWnd, nullptr, FALSE);
}

void MyEditor::OnPaint(HWND, HDC hdc)
{
    for (int i = 0; i < count_; ++i)
    {
        if (shapes_[i] != nullptr)
            shapes_[i]->Show(hdc);
    }
}
