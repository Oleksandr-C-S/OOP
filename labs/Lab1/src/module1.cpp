#include <windows.h>
#include "module1.h"
#include "resource.h"

static wchar_t* g_buffer = nullptr;
static int g_bufferSize = 0;

static INT_PTR CALLBACK Work1DlgProc(
    HWND hDlg,
    UINT message,
    WPARAM wParam,
    LPARAM lParam)
{
    switch (message)
    {
    case WM_INITDIALOG:
        return TRUE;

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDOK:
            if (g_buffer != nullptr && g_bufferSize > 0)
            {
                GetDlgItemTextW(
                    hDlg,
                    IDC_EDIT_MOD1,
                    g_buffer,
                    g_bufferSize
                );
            }

            EndDialog(hDlg, 1);
            return TRUE;

        case IDCANCEL:
            EndDialog(hDlg, 0);
            return TRUE;
        }
        break;

    case WM_CLOSE:
        EndDialog(hDlg, 0);
        return TRUE;
    }

    return FALSE;
}

int Func_MOD1(HWND hWnd, wchar_t* buffer, int bufferSize)
{
    g_buffer = buffer;
    g_bufferSize = bufferSize;

    return static_cast<int>(
        DialogBoxW(
            GetModuleHandleW(nullptr),
            MAKEINTRESOURCEW(IDD_DIALOG_MOD1),
            hWnd,
            Work1DlgProc
        )
        );
}