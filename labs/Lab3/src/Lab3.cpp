#include <windows.h>
#include <commctrl.h>
#include "resource.h"
#include "shape_editor.h"

#pragma comment(lib, "comctl32.lib")

static ShapeObjectsEditor g_editor;
static const wchar_t kClassName[] = L"Lab3ChakunWindowClass";

static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
        g_editor.OnCreate(hWnd);
        return 0;

    case WM_SIZE:
        g_editor.OnSize(hWnd);
        return 0;

    case WM_NOTIFY:
        g_editor.OnNotify(hWnd, wParam, lParam);
        return 0;

    case WM_LBUTTONDOWN:
        g_editor.OnLBdown(hWnd);
        return 0;

    case WM_LBUTTONUP:
        g_editor.OnLBup(hWnd);
        return 0;

    case WM_MOUSEMOVE:
        g_editor.OnMouseMove(hWnd);
        return 0;

    case WM_PAINT:
        g_editor.OnPaint(hWnd);
        return 0;

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDM_POINT:
            g_editor.StartPointEditor(hWnd);
            return 0;

        case IDM_LINE:
            g_editor.StartLineEditor(hWnd);
            return 0;

        case IDM_RECT:
            g_editor.StartRectEditor(hWnd);
            return 0;

        case IDM_ELLIPSE:
            g_editor.StartEllipseEditor(hWnd);
            return 0;

        case IDM_ABOUT:
            MessageBoxW(
                hWnd,
                L"Лабораторна робота №3\n"
                L"Розробка інтерфейсу користувача на C++\n\n"
                L"Виконав: Чакун Олександр Сергійович\n"
                L"Група: ІМ-о51\n"
                L"Номер у списку Lab2: 16\n"
                L"Варіант Lab3: Ж = 16 + 1 = 17\n\n"
                L"• масив Shape*: статичний, N = 117;\n"
                L"• гумовий слід: суцільний червоний;\n"
                L"• прямокутник: від центра до кута,\n"
                L"  чорний контур, сіре заповнення;\n"
                L"• еліпс: по двох протилежних кутах,\n"
                L"  чорний контур без заповнення;\n"
                L"• поточний режим відображається у заголовку;\n"
                L"• Toolbar дублює меню та має підказки.",
                L"Про програму",
                MB_OK | MB_ICONINFORMATION);
            return 0;

        case IDM_EXIT:
            DestroyWindow(hWnd);
            return 0;
        }
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProcW(hWnd, message, wParam, lParam);
}

int APIENTRY wWinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR lpCmdLine,
    _In_ int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    INITCOMMONCONTROLSEX controls{};
    controls.dwSize = sizeof(controls);
    controls.dwICC = ICC_BAR_CLASSES;
    InitCommonControlsEx(&controls);

    WNDCLASSEXW wc{};
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
    wc.lpszMenuName = MAKEINTRESOURCEW(IDR_MAINMENU);
    wc.lpszClassName = kClassName;

    if (!RegisterClassExW(&wc))
    {
        MessageBoxW(nullptr, L"Не вдалося зареєструвати клас вікна.", L"Помилка", MB_OK | MB_ICONERROR);
        return 1;
    }

    HWND hWnd = CreateWindowExW(
        0,
        kClassName,
        L"Лабораторна робота №3 - Чакун Олександр Сергійович",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        1050,
        720,
        nullptr,
        nullptr,
        hInstance,
        nullptr);

    if (hWnd == nullptr)
    {
        MessageBoxW(nullptr, L"Не вдалося створити головне вікно.", L"Помилка", MB_OK | MB_ICONERROR);
        return 1;
    }

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
