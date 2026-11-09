#pragma once
#include <windows.h>
#include <string>
#include <vector>

class MyTable
{
private:
    struct Row
    {
        std::wstring name;
        long x1{};
        long y1{};
        long x2{};
        long y2{};
    };

    HWND hDlg_{ nullptr };
    HWND hList_{ nullptr };
    std::vector<Row> rows_;

    static INT_PTR CALLBACK DlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
    std::wstring FormatRow(const Row& row) const;
    void Populate();

public:
    void Toggle(HWND parent, HINSTANCE hInstance);
    void Close();
    void Add(const wchar_t* name, long x1, long y1, long x2, long y2);
    void Clear();

    HWND GetHandle() const { return hDlg_; }
    bool IsOpen() const { return hDlg_ != nullptr; }
};
