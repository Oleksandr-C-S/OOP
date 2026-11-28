#include <windows.h>
#include <string>
#include <sstream>
#include <cwchar>
#include "resource.h"
#include "../common/Protocol.h"

using namespace Lab6Protocol;

namespace
{
    HINSTANCE g_hInst = nullptr;
    Params g_params{ 12, -10, 10, -10, 10 };
    std::wstring g_status = L"Готово. Оберіть «Робота → Виконати».";

    enum class Stage
    {
        Idle,
        WaitingObject2,
        WaitingObject2Result,
        WaitingObject3,
        WaitingObject3Result,
        Finished
    };

    Stage g_stage = Stage::Idle;
    constexpr UINT_PTR TIMER_FIND = 1;

    void SetStatus(HWND hWnd, const std::wstring& text)
    {
        g_status = text;
        InvalidateRect(hWnd, nullptr, TRUE);
        UpdateWindow(hWnd);
    }

    std::string SiblingExePathA(const char* exeName)
    {
        char path[MAX_PATH]{};
        GetModuleFileNameA(nullptr, path, MAX_PATH);
        std::string p(path);
        const size_t pos = p.find_last_of("\\/");
        if (pos != std::string::npos)
            p.resize(pos + 1);
        p += exeName;
        return p;
    }

    bool LaunchSibling(const char* exeName)
    {
        std::string exe = SiblingExePathA(exeName);
        std::string cmd = "\"" + exe + "\"";
        return WinExec(cmd.c_str(), SW_SHOW) > 31;
    }

    HWND FindObject2()
    {
        return FindWindowW(OBJECT2_CLASS, nullptr);
    }

    HWND FindObject3()
    {
        return FindWindowW(OBJECT3_CLASS, nullptr);
    }

    bool SendParamsToObject2(HWND hManager, HWND hObject2)
    {
        COPYDATASTRUCT cds{};
        cds.dwData = COPYDATA_PARAMS;
        cds.cbData = sizeof(g_params);
        cds.lpData = &g_params;

        return SendMessageW(
            hObject2,
            WM_COPYDATA,
            reinterpret_cast<WPARAM>(hManager),
            reinterpret_cast<LPARAM>(&cds)) != 0;
    }

    void ArrangeWindows(HWND hManager)
    {
        HWND h2 = FindObject2();
        HWND h3 = FindObject3();
        if (!h2 || !h3)
            return;

        RECT work{};
        SystemParametersInfoW(SPI_GETWORKAREA, 0, &work, 0);
        const int sw = work.right - work.left;
        const int sh = work.bottom - work.top;
        const int leftW = (sw * 36) / 100;
        const int topH = sh / 2;

        SetWindowPos(hManager, nullptr, work.left, work.top, leftW, topH, SWP_NOZORDER | SWP_SHOWWINDOW);
        SetWindowPos(h2, nullptr, work.left, work.top + topH, leftW, sh - topH, SWP_NOZORDER | SWP_SHOWWINDOW);
        SetWindowPos(h3, nullptr, work.left + leftW, work.top, sw - leftW, sh, SWP_NOZORDER | SWP_SHOWWINDOW);
        SetForegroundWindow(hManager);
    }

    bool ReadSignedInt(HWND hDlg, int id, int& value)
    {
        wchar_t buffer[64]{};
        GetDlgItemTextW(hDlg, id, buffer, 64);
        wchar_t* end = nullptr;
        long v = wcstol(buffer, &end, 10);
        if (end == buffer || *end != L'\0')
            return false;
        value = static_cast<int>(v);
        return true;
    }

    INT_PTR CALLBACK ParamsDlgProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM)
    {
        switch (msg)
        {
        case WM_INITDIALOG:
            SetDlgItemInt(hDlg, IDC_NPOINT, g_params.nPoint, FALSE);
            SetDlgItemInt(hDlg, IDC_XMIN, static_cast<UINT>(g_params.xMin), TRUE);
            SetDlgItemInt(hDlg, IDC_XMAX, static_cast<UINT>(g_params.xMax), TRUE);
            SetDlgItemInt(hDlg, IDC_YMIN, static_cast<UINT>(g_params.yMin), TRUE);
            SetDlgItemInt(hDlg, IDC_YMAX, static_cast<UINT>(g_params.yMax), TRUE);
            return TRUE;

        case WM_COMMAND:
            if (LOWORD(wParam) == IDOK)
            {
                Params p{};
                if (!ReadSignedInt(hDlg, IDC_NPOINT, p.nPoint) ||
                    !ReadSignedInt(hDlg, IDC_XMIN, p.xMin) ||
                    !ReadSignedInt(hDlg, IDC_XMAX, p.xMax) ||
                    !ReadSignedInt(hDlg, IDC_YMIN, p.yMin) ||
                    !ReadSignedInt(hDlg, IDC_YMAX, p.yMax))
                {
                    MessageBoxW(hDlg, L"Усі параметри повинні бути цілими числами.", L"Помилка", MB_OK | MB_ICONERROR);
                    return TRUE;
                }

                if (p.nPoint <= 0 || p.nPoint > 1000 || p.xMin > p.xMax || p.yMin > p.yMax)
                {
                    MessageBoxW(hDlg,
                        L"Перевірте параметри:\n"
                        L"nPoint має бути від 1 до 1000, xMin <= xMax, yMin <= yMax.",
                        L"Некоректні дані", MB_OK | MB_ICONWARNING);
                    return TRUE;
                }

                g_params = p;
                EndDialog(hDlg, IDOK);
                return TRUE;
            }
            if (LOWORD(wParam) == IDCANCEL)
            {
                EndDialog(hDlg, IDCANCEL);
                return TRUE;
            }
            break;
        }
        return FALSE;
    }

    void BeginWork(HWND hWnd)
    {
        if (DialogBoxParamW(g_hInst, MAKEINTRESOURCEW(IDD_PARAMS), hWnd, ParamsDlgProc, 0) != IDOK)
            return;

        g_stage = Stage::WaitingObject2;
        SetStatus(hWnd, L"Крок 1: пошук або запуск Object2...");

        HWND h2 = FindObject2();
        if (h2)
        {
            SendParamsToObject2(hWnd, h2);
            g_stage = Stage::WaitingObject2Result;
            SetStatus(hWnd, L"Крок 2: параметри передано Object2. Очікую результат...");
        }
        else
        {
            if (!LaunchSibling("Object2.exe"))
            {
                g_stage = Stage::Idle;
                SetStatus(hWnd, L"Помилка: не вдалося запустити Object2.exe.");
                MessageBoxW(hWnd, L"Object2.exe має знаходитися в тій самій папці, що й Lab6.exe.", L"Помилка запуску", MB_OK | MB_ICONERROR);
                return;
            }
            SetTimer(hWnd, TIMER_FIND, 100, nullptr);
        }
    }

    void PollPrograms(HWND hWnd)
    {
        if (g_stage == Stage::WaitingObject2)
        {
            HWND h2 = FindObject2();
            if (h2)
            {
                KillTimer(hWnd, TIMER_FIND);
                SendParamsToObject2(hWnd, h2);
                g_stage = Stage::WaitingObject2Result;
                SetStatus(hWnd, L"Крок 2: Object2 запущено, параметри передано. Очікую результат...");
            }
        }
        else if (g_stage == Stage::WaitingObject3)
        {
            HWND h3 = FindObject3();
            if (h3)
            {
                KillTimer(hWnd, TIMER_FIND);
                PostMessageW(h3, WM_OBJECT3_PROCESS, reinterpret_cast<WPARAM>(hWnd), 0);
                g_stage = Stage::WaitingObject3Result;
                SetStatus(hWnd, L"Крок 4: Object3 запущено. Очікую побудову графіка...");
            }
        }
    }

    LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        switch (msg)
        {
        case WM_CREATE:
        {
            HMENU hMenu = CreateMenu();
            HMENU hWork = CreatePopupMenu();
            AppendMenuW(hWork, MF_STRING, IDM_RUN, L"Виконати...");
            AppendMenuW(hWork, MF_SEPARATOR, 0, nullptr);
            AppendMenuW(hWork, MF_STRING, IDM_EXIT, L"Вихід");
            AppendMenuW(hMenu, MF_POPUP, reinterpret_cast<UINT_PTR>(hWork), L"Робота");

            HMENU hHelp = CreatePopupMenu();
            AppendMenuW(hHelp, MF_STRING, IDM_ABOUT, L"Про програму");
            AppendMenuW(hMenu, MF_POPUP, reinterpret_cast<UINT_PTR>(hHelp), L"Довідка");
            SetMenu(hWnd, hMenu);
            return 0;
        }

        case WM_COMMAND:
            switch (LOWORD(wParam))
            {
            case IDM_RUN:
                BeginWork(hWnd);
                return 0;
            case IDM_EXIT:
                SendMessageW(hWnd, WM_CLOSE, 0, 0);
                return 0;
            case IDM_ABOUT:
                MessageBoxW(hWnd,
                    L"Lab6 — менеджер системи об'єктів\n"
                    L"Варіант 0 (16 mod 4 = 0)\n"
                    L"Чакун Олександр, ІМ-о51",
                    L"Про програму", MB_OK | MB_ICONINFORMATION);
                return 0;
            }
            break;

        case WM_TIMER:
            if (wParam == TIMER_FIND)
                PollPrograms(hWnd);
            return 0;

        case WM_OBJECT2_DONE:
        {
            SetStatus(hWnd, L"Крок 3: Object2 створив точки та записав їх у Clipboard. Запускаю Object3...");
            HWND h3 = FindObject3();
            if (h3)
            {
                PostMessageW(h3, WM_OBJECT3_PROCESS, reinterpret_cast<WPARAM>(hWnd), 0);
                g_stage = Stage::WaitingObject3Result;
            }
            else
            {
                g_stage = Stage::WaitingObject3;
                if (!LaunchSibling("Object3.exe"))
                {
                    g_stage = Stage::Idle;
                    SetStatus(hWnd, L"Помилка: не вдалося запустити Object3.exe.");
                    MessageBoxW(hWnd, L"Object3.exe має знаходитися в тій самій папці, що й Lab6.exe.", L"Помилка запуску", MB_OK | MB_ICONERROR);
                    return 0;
                }
                SetTimer(hWnd, TIMER_FIND, 100, nullptr);
            }
            return 0;
        }

        case WM_OBJECT3_DONE:
            g_stage = Stage::Finished;
            SetStatus(hWnd, L"Готово: Object2 створив точки, Object3 прочитав Clipboard і побудував графік.");
            ArrangeWindows(hWnd);
            return 0;

        case WM_PAINT:
        {
            PAINTSTRUCT ps{};
            HDC hdc = BeginPaint(hWnd, &ps);
            RECT rc{};
            GetClientRect(hWnd, &rc);
            SetBkMode(hdc, TRANSPARENT);

            HFONT hTitle = CreateFontW(24, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
                DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI");
            HFONT old = static_cast<HFONT>(SelectObject(hdc, hTitle));
            TextOutW(hdc, 20, 20, L"Лабораторна робота №6", 22);
            SelectObject(hdc, old);
            DeleteObject(hTitle);

            std::wstringstream ss;
            ss << L"Варіант 0: 16 mod 4 = 0\n\n"
               << L"Lab6 — менеджер\n"
               << L"Object2 — генератор точок та запис у Clipboard\n"
               << L"Object3 — читання Clipboard та побудова y=f(x)\n\n"
               << L"Поточний стан:\n" << g_status;

            RECT textRc{ 20, 65, rc.right - 20, rc.bottom - 20 };
            DrawTextW(hdc, ss.str().c_str(), -1, &textRc, DT_LEFT | DT_TOP | DT_WORDBREAK);
            EndPaint(hWnd, &ps);
            return 0;
        }

        case WM_DESTROY:
        {
            KillTimer(hWnd, TIMER_FIND);
            if (HWND h2 = FindObject2())
                PostMessageW(h2, WM_CLOSE, 0, 0);
            if (HWND h3 = FindObject3())
                PostMessageW(h3, WM_CLOSE, 0, 0);
            PostQuitMessage(0);
            return 0;
        }
        }
        return DefWindowProcW(hWnd, msg, wParam, lParam);
    }
}

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE, LPWSTR, int nCmdShow)
{
    g_hInst = hInstance;

    WNDCLASSEXW wc{};
    wc.cbSize = sizeof(wc);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
    wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
    wc.lpszClassName = MANAGER_CLASS;
    RegisterClassExW(&wc);

    HWND hWnd = CreateWindowExW(0, MANAGER_CLASS, L"Lab6 — менеджер (варіант 0)",
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, 560, 400,
        nullptr, nullptr, hInstance, nullptr);

    if (!hWnd)
        return 0;

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    MSG msg{};
    while (GetMessageW(&msg, nullptr, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    return static_cast<int>(msg.wParam);
}
