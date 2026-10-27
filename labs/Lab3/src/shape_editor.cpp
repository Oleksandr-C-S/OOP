#include <windows.h>
#include <commctrl.h>
#include "shape_editor.h"
#include "editor.h"
#include "shape.h"
#include "resource.h"

#pragma comment(lib, "comctl32.lib")

namespace
{
    HIMAGELIST g_toolbarImages = nullptr;
}

ShapeObjectsEditor::ShapeObjectsEditor()
{
    currentEditor_ = new PointEditor(pcshape_, &shapeCount_, kMaxShapes);
    currentCommand_ = IDM_POINT;
}

ShapeObjectsEditor::~ShapeObjectsEditor()
{
    delete currentEditor_;
    currentEditor_ = nullptr;

    for (int i = 0; i < shapeCount_; ++i)
    {
        delete pcshape_[i];
        pcshape_[i] = nullptr;
    }

    if (g_toolbarImages != nullptr)
    {
        ImageList_Destroy(g_toolbarImages);
        g_toolbarImages = nullptr;
    }
}

void ShapeObjectsEditor::SetModeTitle(HWND hWnd, const wchar_t* modeName)
{
    wchar_t title[160]{};
    wsprintfW(
        title,
        L"Лабораторна робота №3 - Чакун Олександр Сергійович - Режим: %s",
        modeName);

    SetWindowTextW(hWnd, title);
}

void ShapeObjectsEditor::ReplaceEditor(
    ShapeEditor* editor,
    UINT commandId,
    HWND hWnd,
    const wchar_t* modeName)
{
    delete currentEditor_;
    currentEditor_ = editor;
    currentCommand_ = commandId;

    SetModeTitle(hWnd, modeName);
    InvalidateRect(hWnd, nullptr, FALSE);
}

void ShapeObjectsEditor::OnCreate(HWND hWnd)
{
    toolbar_ = CreateWindowExW(
        0,
        TOOLBARCLASSNAMEW,
        nullptr,
        WS_CHILD | WS_VISIBLE | TBSTYLE_FLAT | TBSTYLE_TOOLTIPS | CCS_TOP,
        0, 0, 0, 0,
        hWnd,
        reinterpret_cast<HMENU>(
            static_cast<INT_PTR>(IDC_MAIN_TOOLBAR)),
        GetModuleHandleW(nullptr),
        nullptr);

    if (toolbar_ == nullptr)
        return;

    SendMessageW(
        toolbar_,
        TB_BUTTONSTRUCTSIZE,
        static_cast<WPARAM>(sizeof(TBBUTTON)),
        0);

    SendMessageW(toolbar_, TB_SETBITMAPSIZE, 0, MAKELPARAM(24, 24));
    SendMessageW(toolbar_, TB_SETBUTTONSIZE, 0, MAKELPARAM(30, 30));

    // Завантажуємо власний BMP як звичайний Win32 bitmap-ресурс.
    HBITMAP hBitmap = static_cast<HBITMAP>(
        LoadImageW(
            GetModuleHandleW(nullptr),
            MAKEINTRESOURCEW(IDB_TOOLBAR),
            IMAGE_BITMAP,
            0,
            0,
            LR_CREATEDIBSECTION));

    if (hBitmap != nullptr)
    {
        g_toolbarImages = ImageList_Create(
            24,
            24,
            ILC_COLOR24 | ILC_MASK,
            4,
            0);

        if (g_toolbarImages != nullptr)
        {
            // Фон toolbar.bmp — яскраво-рожевий; він стає прозорим.
            ImageList_AddMasked(
                g_toolbarImages,
                hBitmap,
                RGB(255, 0, 255));

            SendMessageW(
                toolbar_,
                TB_SETIMAGELIST,
                0,
                reinterpret_cast<LPARAM>(g_toolbarImages));
        }

        DeleteObject(hBitmap);
    }
    else
    {
        MessageBoxW(
            hWnd,
            L"Не вдалося завантажити ресурс toolbar.bmp.",
            L"Lab3",
            MB_OK | MB_ICONWARNING);
    }

    TBBUTTON buttons[4]{};
    const UINT commands[4] =
    {
        IDM_POINT,
        IDM_LINE,
        IDM_RECT,
        IDM_ELLIPSE
    };

    for (int i = 0; i < 4; ++i)
    {
        buttons[i].iBitmap = i;
        buttons[i].idCommand = commands[i];
        buttons[i].fsState = TBSTATE_ENABLED;
        buttons[i].fsStyle = BTNS_BUTTON;
        buttons[i].dwData = 0;
        buttons[i].iString = 0;
    }

    SendMessageW(
        toolbar_,
        TB_ADDBUTTONS,
        4,
        reinterpret_cast<LPARAM>(buttons));

    SendMessageW(toolbar_, TB_AUTOSIZE, 0, 0);
    ShowWindow(toolbar_, SW_SHOW);

    SetModeTitle(hWnd, L"Крапка");
}

void ShapeObjectsEditor::OnSize(HWND)
{
    if (toolbar_ != nullptr)
        SendMessageW(toolbar_, TB_AUTOSIZE, 0, 0);
}

void ShapeObjectsEditor::OnNotify(HWND, WPARAM, LPARAM lParam)
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

    default:
        break;
    }
}

void ShapeObjectsEditor::StartPointEditor(HWND hWnd)
{
    ReplaceEditor(
        new PointEditor(pcshape_, &shapeCount_, kMaxShapes),
        IDM_POINT,
        hWnd,
        L"Крапка");
}

void ShapeObjectsEditor::StartLineEditor(HWND hWnd)
{
    ReplaceEditor(
        new LineEditor(pcshape_, &shapeCount_, kMaxShapes),
        IDM_LINE,
        hWnd,
        L"Лінія");
}

void ShapeObjectsEditor::StartRectEditor(HWND hWnd)
{
    ReplaceEditor(
        new RectEditor(pcshape_, &shapeCount_, kMaxShapes),
        IDM_RECT,
        hWnd,
        L"Прямокутник");
}

void ShapeObjectsEditor::StartEllipseEditor(HWND hWnd)
{
    ReplaceEditor(
        new EllipseEditor(pcshape_, &shapeCount_, kMaxShapes),
        IDM_ELLIPSE,
        hWnd,
        L"Еліпс");
}

void ShapeObjectsEditor::OnLBdown(HWND hWnd)
{
    if (currentEditor_ != nullptr)
        currentEditor_->OnLBdown(hWnd);
}

void ShapeObjectsEditor::OnLBup(HWND hWnd)
{
    if (currentEditor_ != nullptr)
        currentEditor_->OnLBup(hWnd);
}

void ShapeObjectsEditor::OnMouseMove(HWND hWnd)
{
    if (currentEditor_ != nullptr)
        currentEditor_->OnMouseMove(hWnd);
}

void ShapeObjectsEditor::OnPaint(HWND hWnd)
{
    if (currentEditor_ != nullptr)
    {
        currentEditor_->OnPaint(hWnd);
        return;
    }

    PAINTSTRUCT ps{};
    BeginPaint(hWnd, &ps);
    EndPaint(hWnd, &ps);
}
