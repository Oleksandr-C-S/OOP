#include <windows.h>
#include "resource.h"
#include "shape_editor.h"

static ShapeObjectsEditor g_editor;
static const wchar_t kClassName[] = L"Lab2ChakunWindowClass";

static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);

    switch (message)
    {
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

    case WM_INITMENUPOPUP:
        g_editor.OnInitMenuPopup(hWnd, wParam);
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
                L"Лабораторна робота №2\n"
                L"Розробка графічного редактора об'єктів на C++\n\n"
                L"Виконав: Чакун Олександр Сергійович\n"
                L"Група: ІМ-о51\n"
                L"Номер у списку: 16\n\n"
                L"Варіант Ж = 16:\n"
                L"• масив Shape*: статичний, N = 116;\n"
                L"• гумовий слід: суцільний чорний;\n"
                L"• прямокутник: по двох протилежних кутах,\n"
                L"  чорний контур, помаранчеве заповнення;\n"
                L"• еліпс: від центру до кута,\n"
                L"  чорний контур, біле заповнення;\n"
                L"• поточний режим позначається в меню.",
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
        L"Лабораторна робота №2 - Чакун Олександр Сергійович",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        1000,
        700,
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
