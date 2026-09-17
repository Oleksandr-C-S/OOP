#include <windows.h>
#include "module2.h"
#include "resource.h"

static int* g_resultValue = nullptr;
static int g_value = 1;

static void UpdateValue(HWND hDlg)
{
    HWND hScroll = GetDlgItem(hDlg, IDC_SCROLL_MOD2);

    SetScrollPos(
        hScroll,
        SB_CTL,
        g_value,
        TRUE
    );

    SetDlgItemInt(
        hDlg,
        IDC_VALUE_MOD2,
        g_value,
        FALSE
    );
}

static INT_PTR CALLBACK Work2DlgProc(
    HWND hDlg,
    UINT message,
    WPARAM wParam,
    LPARAM lParam)
{
    switch (message)
    {
    case WM_INITDIALOG:
    {
        HWND hScroll = GetDlgItem(hDlg, IDC_SCROLL_MOD2);

        SetScrollRange(
            hScroll,
            SB_CTL,
            1,
            100,
            TRUE
        );

        UpdateValue(hDlg);

        return TRUE;
    }

    case WM_HSCROLL:
    {
        HWND hScroll = GetDlgItem(hDlg, IDC_SCROLL_MOD2);

        if ((HWND)lParam != hScroll)
            break;

        switch (LOWORD(wParam))
        {
        case SB_LINELEFT:
            g_value--;
            break;

        case SB_LINERIGHT:
            g_value++;
            break;

        case SB_PAGELEFT:
            g_value -= 10;
            break;

        case SB_PAGERIGHT:
            g_value += 10;
            break;

        case SB_THUMBPOSITION:
        case SB_THUMBTRACK:
            g_value = HIWORD(wParam);
            break;
        }

        if (g_value < 1)
            g_value = 1;

        if (g_value > 100)
            g_value = 100;

        UpdateValue(hDlg);

        return TRUE;
    }

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDOK:
            if (g_resultValue != nullptr)
                *g_resultValue = g_value;

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

int Func_MOD2(HWND hWnd, int* value)
{
    if (value == nullptr)
        return 0;

    g_resultValue = value;
    g_value = *value;

    if (g_value < 1 || g_value > 100)
        g_value = 1;

    return static_cast<int>(
        DialogBoxW(
            GetModuleHandleW(nullptr),
            MAKEINTRESOURCEW(IDD_DIALOG_MOD2),
            hWnd,
            Work2DlgProc
        )
        );
}