#include "my_table.h"
#include "my_table_resources.h"

std::wstring MyTable::FormatRow(const Row& row) const
{
    wchar_t buffer[512]{};
    swprintf_s(
        buffer,
        L"%s\t%ld\t%ld\t%ld\t%ld",
        row.name.c_str(),
        row.x1,
        row.y1,
        row.x2,
        row.y2);

    return buffer;
}

void MyTable::Populate()
{
    if (hList_ == nullptr)
        return;

    SendMessageW(hList_, LB_RESETCONTENT, 0, 0);

    for (const Row& row : rows_)
    {
        const std::wstring text = FormatRow(row);
        SendMessageW(hList_, LB_ADDSTRING, 0, reinterpret_cast<LPARAM>(text.c_str()));
    }
}

void MyTable::Toggle(HWND parent, HINSTANCE hInstance)
{
    if (hDlg_ != nullptr)
    {
        Close();
        return;
    }

    CreateDialogParamW(
        hInstance,
        MAKEINTRESOURCEW(IDD_MYTABLE),
        parent,
        DlgProc,
        reinterpret_cast<LPARAM>(this));
}

void MyTable::Close()
{
    if (hDlg_ != nullptr)
        DestroyWindow(hDlg_);
}

void MyTable::Add(const wchar_t* name, long x1, long y1, long x2, long y2)
{
    Row row{};
    row.name = (name != nullptr) ? name : L"Об'єкт";
    row.x1 = x1;
    row.y1 = y1;
    row.x2 = x2;
    row.y2 = y2;

    rows_.push_back(row);

    if (hList_ != nullptr)
    {
        const std::wstring text = FormatRow(row);
        const LRESULT index = SendMessageW(
            hList_,
            LB_ADDSTRING,
            0,
            reinterpret_cast<LPARAM>(text.c_str()));

        if (index != LB_ERR)
            SendMessageW(hList_, LB_SETTOPINDEX, static_cast<WPARAM>(index), 0);
    }
}

void MyTable::Clear()
{
    rows_.clear();

    if (hList_ != nullptr)
        SendMessageW(hList_, LB_RESETCONTENT, 0, 0);
}

INT_PTR CALLBACK MyTable::DlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    MyTable* self = reinterpret_cast<MyTable*>(
        GetWindowLongPtrW(hDlg, GWLP_USERDATA));

    if (message == WM_INITDIALOG)
    {
        self = reinterpret_cast<MyTable*>(lParam);
        if (self == nullptr)
            return FALSE;

        SetWindowLongPtrW(
            hDlg,
            GWLP_USERDATA,
            reinterpret_cast<LONG_PTR>(self));

        self->hDlg_ = hDlg;
        self->hList_ = GetDlgItem(hDlg, IDC_TABLE_LIST);

        if (self->hList_ != nullptr)
        {
            const int tabStops[4] = { 130, 180, 230, 280 };
            SendMessageW(
                self->hList_,
                LB_SETTABSTOPS,
                4,
                reinterpret_cast<LPARAM>(tabStops));
        }

        self->Populate();
        return TRUE;
    }

    if (self == nullptr)
        return FALSE;

    switch (message)
    {
    case WM_COMMAND:
        if (LOWORD(wParam) == IDCANCEL)
        {
            DestroyWindow(hDlg);
            return TRUE;
        }
        break;

    case WM_CLOSE:
        DestroyWindow(hDlg);
        return TRUE;

    case WM_DESTROY:
        self->hDlg_ = nullptr;
        self->hList_ = nullptr;
        return TRUE;
    }

    return FALSE;
}
