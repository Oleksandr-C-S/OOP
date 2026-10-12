#include "shape_editor.h"
#include "editor.h"
#include "shape.h"
#include "resource.h"

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
}

void ShapeObjectsEditor::ReplaceEditor(ShapeEditor* editor, UINT commandId, HWND hWnd)
{
    delete currentEditor_;
    currentEditor_ = editor;
    currentCommand_ = commandId;
    InvalidateRect(hWnd, nullptr, FALSE);
}

void ShapeObjectsEditor::StartPointEditor(HWND hWnd)
{
    ReplaceEditor(new PointEditor(pcshape_, &shapeCount_, kMaxShapes), IDM_POINT, hWnd);
}

void ShapeObjectsEditor::StartLineEditor(HWND hWnd)
{
    ReplaceEditor(new LineEditor(pcshape_, &shapeCount_, kMaxShapes), IDM_LINE, hWnd);
}

void ShapeObjectsEditor::StartRectEditor(HWND hWnd)
{
    ReplaceEditor(new RectEditor(pcshape_, &shapeCount_, kMaxShapes), IDM_RECT, hWnd);
}

void ShapeObjectsEditor::StartEllipseEditor(HWND hWnd)
{
    ReplaceEditor(new EllipseEditor(pcshape_, &shapeCount_, kMaxShapes), IDM_ELLIPSE, hWnd);
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

void ShapeObjectsEditor::OnInitMenuPopup(HWND hWnd, WPARAM wParam)
{
    HMENU hMenu = GetMenu(hWnd);
    if (hMenu == nullptr)
        return;

    // "Об'єкти" є другим POPUP-меню: індекс 1.
    HMENU hObjectsMenu = GetSubMenu(hMenu, 1);
    if (hObjectsMenu == nullptr || reinterpret_cast<HMENU>(wParam) != hObjectsMenu)
        return;

    const UINT ids[] = { IDM_POINT, IDM_LINE, IDM_RECT, IDM_ELLIPSE };
    for (UINT id : ids)
        CheckMenuItem(hObjectsMenu, id, MF_BYCOMMAND | MF_UNCHECKED);

    CheckMenuItem(hObjectsMenu, currentCommand_, MF_BYCOMMAND | MF_CHECKED);
}
