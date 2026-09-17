#include <windows.h>
#include <cwchar>

#include "resource.h"
#include "module1.h"
#include "module2.h"


// Результат роботи модуля 1
static wchar_t g_text[256] = L"";

// Результат роботи модуля 2
static int g_number = 1;


// Прототип функції головного вікна
static LRESULT CALLBACK WndProc(
    HWND hWnd,
    UINT message,
    WPARAM wParam,
    LPARAM lParam
);


// Допоміжна функція для виведення Unicode-тексту
static void DrawTextLine(
    HDC hdc,
    int x,
    int y,
    const wchar_t* text
)
{
    if (text == nullptr)
        return;

    TextOutW(
        hdc,
        x,
        y,
        text,
        static_cast<int>(wcslen(text))
    );
}


// Точка входу Windows-програми
int APIENTRY wWinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR lpCmdLine,
    _In_ int nCmdShow
)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    const wchar_t CLASS_NAME[] = L"Lab1WindowClass";


    // Опис класу головного вікна
    WNDCLASSEXW wc{};

    wc.cbSize = sizeof(WNDCLASSEXW);

    wc.style =
        CS_HREDRAW |
        CS_VREDRAW;

    wc.lpfnWndProc = WndProc;

    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;

    wc.hInstance = hInstance;

    wc.hIcon = nullptr;

    wc.hCursor = LoadCursorW(
        nullptr,
        IDC_ARROW
    );

    wc.hbrBackground =
        reinterpret_cast<HBRUSH>(
            COLOR_WINDOW + 1
            );

    // Меню з Lab1.rc
    wc.lpszMenuName =
        MAKEINTRESOURCEW(IDR_MAINMENU);

    wc.lpszClassName = CLASS_NAME;

    wc.hIconSm = nullptr;


    // Реєстрація класу вікна
    if (!RegisterClassExW(&wc))
    {
        MessageBoxW(
            nullptr,
            L"Не вдалося зареєструвати клас головного вікна.",
            L"Помилка",
            MB_OK | MB_ICONERROR
        );

        return 0;
    }


    // Створення головного вікна
    HWND hWnd = CreateWindowExW(
        0,

        CLASS_NAME,

        L"Лабораторна робота №1 - Чакун Олександр Сергійович",

        WS_OVERLAPPEDWINDOW,

        CW_USEDEFAULT,
        CW_USEDEFAULT,

        800,
        500,

        nullptr,
        nullptr,

        hInstance,

        nullptr
    );


    if (hWnd == nullptr)
    {
        MessageBoxW(
            nullptr,
            L"Не вдалося створити головне вікно.",
            L"Помилка",
            MB_OK | MB_ICONERROR
        );

        return 0;
    }


    ShowWindow(
        hWnd,
        nCmdShow
    );

    UpdateWindow(
        hWnd
    );


    // Головний цикл повідомлень
    MSG msg{};

    while (
        GetMessageW(
            &msg,
            nullptr,
            0,
            0
        )
        )
    {
        TranslateMessage(
            &msg
        );

        DispatchMessageW(
            &msg
        );
    }


    return static_cast<int>(
        msg.wParam
        );
}


// Callback-функція головного вікна
static LRESULT CALLBACK WndProc(
    HWND hWnd,
    UINT message,
    WPARAM wParam,
    LPARAM lParam
)
{
    switch (message)
    {

        // ------------------------------------------------
        // Команди меню
        // ------------------------------------------------

    case WM_COMMAND:
    {
        const int commandId =
            LOWORD(wParam);


        switch (commandId)
        {

            // Робота 1
        case IDM_WORK1:
        {
            const int result =
                Func_MOD1(
                    hWnd,
                    g_text,
                    static_cast<int>(
                        _countof(g_text)
                        )
                );


            if (result != 0)
            {
                InvalidateRect(
                    hWnd,
                    nullptr,
                    TRUE
                );
            }

            return 0;
        }


        // Робота 2
        case IDM_WORK2:
        {
            const int result =
                Func_MOD2(
                    hWnd,
                    &g_number
                );


            if (result != 0)
            {
                InvalidateRect(
                    hWnd,
                    nullptr,
                    TRUE
                );
            }

            return 0;
        }


        // Інформація про програму
        case IDM_ABOUT:
        {
            MessageBoxW(
                hWnd,

                L"Лабораторна робота №1\n\n"
                L"Дисципліна:\n"
                L"Об'єктно-орієнтоване програмування\n\n"
                L"Виконав:\n"
                L"Чакун Олександр Сергійович\n\n"
                L"Номер у списку групи: 16\n\n"
                L"Варіант роботи 1:\n"
                L"В1 = 16 mod 4 = 0\n\n"
                L"Варіант роботи 2:\n"
                L"В2 = (16 + 1) mod 4 = 1",

                L"Про програму",

                MB_OK |
                MB_ICONINFORMATION
            );

            return 0;
        }


        // Вихід
        case IDM_EXIT:
        {
            DestroyWindow(
                hWnd
            );

            return 0;
        }

        }

        break;
    }


    // ------------------------------------------------
    // Малювання головного вікна
    // ------------------------------------------------

    case WM_PAINT:
    {
        PAINTSTRUCT ps{};

        HDC hdc = BeginPaint(
            hWnd,
            &ps
        );


        SetBkMode(
            hdc,
            TRANSPARENT
        );


        // Створюємо шрифт із нормальною підтримкою кирилиці
        HFONT hFont = CreateFontW(
            -20,                            // висота
            0,                              // ширина
            0,
            0,
            FW_NORMAL,
            FALSE,
            FALSE,
            FALSE,
            DEFAULT_CHARSET,
            OUT_DEFAULT_PRECIS,
            CLIP_DEFAULT_PRECIS,
            CLEARTYPE_QUALITY,
            DEFAULT_PITCH | FF_DONTCARE,
            L"Segoe UI"
        );


        HFONT hOldFont =
            static_cast<HFONT>(
                SelectObject(
                    hdc,
                    hFont
                )
                );


        // Заголовок
        DrawTextLine(
            hdc,
            30,
            40,
            L"Лабораторна робота №1"
        );


        // Результат Роботи 1
        DrawTextLine(
            hdc,
            30,
            85,
            L"Робота 1 - введений текст:"
        );


        DrawTextLine(
            hdc,
            30,
            115,
            g_text
        );


        // Результат Роботи 2
        wchar_t numberBuffer[100]{};


        swprintf_s(
            numberBuffer,
            _countof(numberBuffer),
            L"Робота 2 - вибране число: %d",
            g_number
        );


        DrawTextLine(
            hdc,
            30,
            165,
            numberBuffer
        );


        // Повертаємо попередній шрифт
        SelectObject(
            hdc,
            hOldFont
        );


        // Видаляємо створений GDI-об'єкт
        DeleteObject(
            hFont
        );


        EndPaint(
            hWnd,
            &ps
        );


        return 0;
    }


    // ------------------------------------------------
    // Закриття програми
    // ------------------------------------------------

    case WM_DESTROY:
    {
        PostQuitMessage(
            0
        );

        return 0;
    }

    }


    return DefWindowProcW(
        hWnd,
        message,
        wParam,
        lParam
    );
}