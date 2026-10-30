#include <windows.h>
#include <commctrl.h>

#include "resource.h"
#include "my_editor.h"
#include "point_shape.h"
#include "line_shape.h"
#include "rect_shape.h"
#include "ellipse_shape.h"
#include "line_oo_shape.h"
#include "cube_shape.h"

static MyEditor* g_editor = nullptr;

static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

int APIENTRY wWinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR lpCmdLine,
    _In_ int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    INITCOMMONCONTROLSEX icc{};
    icc.dwSize = sizeof(icc);
    icc.dwICC = ICC_BAR_CLASSES;
    InitCommonControlsEx(&icc);

    const wchar_t CLASS_NAME[] = L"Lab4WindowClass";

    WNDCLASSEXW wc{};
    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
    wc.lpszMenuName = MAKEINTRESOURCEW(IDR_MAINMENU);
    wc.lpszClassName = CLASS_NAME;

    if (!RegisterClassExW(&wc))
        return 0;

    HWND hWnd = CreateWindowExW(
        0,
        CLASS_NAME,
        L"Лабораторна робота №4 - Чакун Олександр Сергійович",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        1100, 760,
        nullptr, nullptr,
        hInstance, nullptr);

    if (hWnd == nullptr)
        return 0;

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    MSG msg{};
    while (GetMessageW(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    return static_cast<int>(msg.wParam);
}

static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
        g_editor = new MyEditor();
        g_editor->OnCreate(hWnd);
        g_editor->Start(new PointShape());
        return 0;

    case WM_SIZE:
        if (g_editor)
            g_editor->OnSize(hWnd);
        return 0;

    case WM_NOTIFY:
        if (g_editor)
            g_editor->OnNotify(hWnd, wParam, lParam);
        return 0;

    case WM_LBUTTONDOWN:
        if (g_editor)
            g_editor->OnLBdown(hWnd);
        return 0;

    case WM_LBUTTONUP:
        if (g_editor)
            g_editor->OnLBup(hWnd);
        return 0;

    case WM_MOUSEMOVE:
        if (g_editor)
            g_editor->OnMouseMove(hWnd);
        return 0;

    case WM_PAINT:
    {
        PAINTSTRUCT ps{};
        HDC hdc = BeginPaint(hWnd, &ps);

        if (g_editor)
            g_editor->OnPaint(hWnd, hdc);

        EndPaint(hWnd, &ps);
        return 0;
    }

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDM_POINT:
            if (g_editor) g_editor->Start(new PointShape());
            return 0;
        case IDM_LINE:
            if (g_editor) g_editor->Start(new LineShape());
            return 0;
        case IDM_RECT:
            if (g_editor) g_editor->Start(new RectShape());
            return 0;
        case IDM_ELLIPSE:
            if (g_editor) g_editor->Start(new EllipseShape());
            return 0;
        case IDM_LINEOO:
            if (g_editor) g_editor->Start(new LineOOShape());
            return 0;
        case IDM_CUBE:
            if (g_editor) g_editor->Start(new CubeShape());
            return 0;
        case IDM_ABOUT:
            MessageBoxW(
                hWnd,
                L"Лабораторна робота №4\n"
                L"Чакун Олександр Сергійович\n"
                L"Номер у списку: 16\n\n"
                L"Динамічний об'єкт MyEditor\n"
                L"6 типів графічних об'єктів\n"
                L"LineOOShape і CubeShape реалізовані множинним успадкуванням.",
                L"Про програму",
                MB_OK | MB_ICONINFORMATION);
            return 0;
        case IDM_EXIT:
            DestroyWindow(hWnd);
            return 0;
        }
        break;

    case WM_DESTROY:
        delete g_editor;
        g_editor = nullptr;
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProcW(hWnd, message, wParam, lParam);
}
