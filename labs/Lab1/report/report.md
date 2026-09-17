<div style="text-align: center; font-size: 24px; margin-top: 60px;">

Міністерство освіти і науки України

Національний технічний університет України

«Київський політехнічний інститут імені Ігоря Сікорського»

Факультет інформатики та обчислювальної техніки

Кафедра обчислювальної техніки

</div>

<div style="text-align: center; margin-top: 120px;">

<h1 style="font-size: 22px;">Лабораторна робота №1</h1>

<h2 style="font-size: 22px;">з дисципліни «Об'єктно-орієнтоване програмування»</h2>

<h3 style="font-size: 22px; margin-top: 20px;">на тему</h3>

<h2 style="font-size: 22px;">«Знайомство із середовищем розробки програм Microsoft Visual Studio та складання модульних проєктів програм на C++»</h2>

</div>

<div style="text-align: right; margin-top: 120px; font-size: 18px;">

<strong>Виконав:</strong><br>
Чакун Олександр Сергійович<br>
студент групи ІМ-о51<br>
номер у списку групи: 16<br><br>

<strong>Перевірив:</strong><br>
Рекечинський Дмитро Олександрович

</div>

<div style="text-align: center; margin-top: 120px; font-size: 20px;">

Київ 2026

</div>

---

## Завдання

1. Створити у середовищі MS Visual Studio C++ проєкт Windows Desktop Application з ім'ям `Lab1`.
2. Написати вихідний текст програми згідно варіанту завдання.
3. Скомпілювати вихідний текст та отримати виконуваний файл програми.
4. Перевірити роботу програми та виконати налагодження.
5. Проаналізувати та прокоментувати результати роботи й вихідний текст програми.
6. Реалізувати функції «Робота 1» і «Робота 2» в окремих незалежних модулях.
7. Оформити звіт із вихідним кодом, схемою залежностей, скріншотами та висновками.

---

## Завдання згідно варіанту

Номер студента у списку групи:

```text
Ж = 16
```

Варіант для «Робота 1»:

```text
В1 = Ж mod 4
В1 = 16 mod 4 = 0
```

Для варіанта `0` потрібно створити діалогове вікно з елементом **Edit Control** та кнопками «Так» і «Відміна». Користувач вводить рядок тексту, після чого натискає «Так». Введений рядок повинен відображатися у клієнтській області головного вікна.

Варіант для «Робота 2»:

```text
В2 = (Ж + 1) mod 4
В2 = 17 mod 4 = 1
```

Для варіанта `1` потрібно створити діалогове вікно з горизонтальним **Scroll Bar** та кнопками «Так» і «Відміна». Повзунок використовується для введення числа у діапазоні від `1` до `100`. Після натискання «Так» вибране число повинно відображатися у головному вікні програми.

Callback-функції діалогових вікон оголошені як `static`, тому вони приховані всередині модулів. Інтерфейс кожного модуля містить одну зовнішню функцію.

---

## Вихідний текст програми

### Головний файл `Lab1.cpp`

```cpp
#include <windows.h>
#include <cwchar>

#include "resource.h"
#include "module1.h"
#include "module2.h"

static wchar_t g_text[256] = L"";
static int g_number = 1;

static LRESULT CALLBACK WndProc(
    HWND hWnd,
    UINT message,
    WPARAM wParam,
    LPARAM lParam
);

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

    WNDCLASSEXW wc{};
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
    wc.lpszMenuName = MAKEINTRESOURCEW(IDR_MAINMENU);
    wc.lpszClassName = CLASS_NAME;

    if (!RegisterClassExW(&wc))
    {
        MessageBoxW(nullptr,
            L"Не вдалося зареєструвати клас головного вікна.",
            L"Помилка", MB_OK | MB_ICONERROR);
        return 0;
    }

    HWND hWnd = CreateWindowExW(
        0,
        CLASS_NAME,
        L"Лабораторна робота №1 - Чакун Олександр Сергійович",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        800, 500,
        nullptr, nullptr,
        hInstance, nullptr
    );

    if (hWnd == nullptr)
    {
        MessageBoxW(nullptr,
            L"Не вдалося створити головне вікно.",
            L"Помилка", MB_OK | MB_ICONERROR);
        return 0;
    }

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

static LRESULT CALLBACK WndProc(
    HWND hWnd,
    UINT message,
    WPARAM wParam,
    LPARAM lParam
)
{
    switch (message)
    {
    case WM_COMMAND:
    {
        const int commandId = LOWORD(wParam);

        switch (commandId)
        {
        case IDM_WORK1:
        {
            const int result = Func_MOD1(
                hWnd,
                g_text,
                static_cast<int>(_countof(g_text))
            );

            if (result != 0)
                InvalidateRect(hWnd, nullptr, TRUE);

            return 0;
        }

        case IDM_WORK2:
        {
            const int result = Func_MOD2(hWnd, &g_number);

            if (result != 0)
                InvalidateRect(hWnd, nullptr, TRUE);

            return 0;
        }

        case IDM_ABOUT:
            MessageBoxW(
                hWnd,
                L"Лабораторна робота №1\n\n"
                L"Дисципліна:\n"
                L"Об'єктно-орієнтоване програмування\n\n"
                L"Виконав:\n"
                L"Чакун Олександр Сергійович\n\n"
                L"Номер у списку групи: 16\n\n"
                L"В1 = 16 mod 4 = 0\n"
                L"В2 = (16 + 1) mod 4 = 1",
                L"Про програму",
                MB_OK | MB_ICONINFORMATION
            );
            return 0;

        case IDM_EXIT:
            DestroyWindow(hWnd);
            return 0;
        }
        break;
    }

    case WM_PAINT:
    {
        PAINTSTRUCT ps{};
        HDC hdc = BeginPaint(hWnd, &ps);

        SetBkMode(hdc, TRANSPARENT);

        HFONT hFont = CreateFontW(
            -20, 0, 0, 0,
            FW_NORMAL,
            FALSE, FALSE, FALSE,
            DEFAULT_CHARSET,
            OUT_DEFAULT_PRECIS,
            CLIP_DEFAULT_PRECIS,
            CLEARTYPE_QUALITY,
            DEFAULT_PITCH | FF_DONTCARE,
            L"Segoe UI"
        );

        HFONT hOldFont = static_cast<HFONT>(SelectObject(hdc, hFont));

        DrawTextLine(hdc, 30, 40, L"Лабораторна робота №1");
        DrawTextLine(hdc, 30, 85, L"Робота 1 - введений текст:");
        DrawTextLine(hdc, 30, 115, g_text);

        wchar_t numberBuffer[100]{};
        swprintf_s(
            numberBuffer,
            _countof(numberBuffer),
            L"Робота 2 - вибране число: %d",
            g_number
        );

        DrawTextLine(hdc, 30, 165, numberBuffer);

        SelectObject(hdc, hOldFont);
        DeleteObject(hFont);

        EndPaint(hWnd, &ps);
        return 0;
    }

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProcW(hWnd, message, wParam, lParam);
}
```

### Модуль 1 — `module1.h`

```cpp
#pragma once

#include <windows.h>

extern int Func_MOD1(HWND hWnd, wchar_t* buffer, int bufferSize);
```

### Модуль 1 — `module1.cpp`

```cpp
#include <windows.h>
#include "module1.h"
#include "resource.h"

static wchar_t* g_buffer = nullptr;
static int g_bufferSize = 0;

static INT_PTR CALLBACK Work1DlgProc(
    HWND hDlg,
    UINT message,
    WPARAM wParam,
    LPARAM lParam
)
{
    UNREFERENCED_PARAMETER(lParam);

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
```

### Модуль 2 — `module2.h`

```cpp
#pragma once

#include <windows.h>

extern int Func_MOD2(HWND hWnd, int* value);
```

### Модуль 2 — `module2.cpp`

```cpp
#include <windows.h>
#include "module2.h"
#include "resource.h"

static int* g_resultValue = nullptr;
static int g_value = 1;

static void UpdateValue(HWND hDlg)
{
    HWND hScroll = GetDlgItem(hDlg, IDC_SCROLL_MOD2);

    SetScrollPos(hScroll, SB_CTL, g_value, TRUE);
    SetDlgItemInt(hDlg, IDC_VALUE_MOD2, g_value, FALSE);
}

static INT_PTR CALLBACK Work2DlgProc(
    HWND hDlg,
    UINT message,
    WPARAM wParam,
    LPARAM lParam
)
{
    switch (message)
    {
    case WM_INITDIALOG:
    {
        HWND hScroll = GetDlgItem(hDlg, IDC_SCROLL_MOD2);
        SetScrollRange(hScroll, SB_CTL, 1, 100, TRUE);
        UpdateValue(hDlg);
        return TRUE;
    }

    case WM_HSCROLL:
    {
        HWND hScroll = GetDlgItem(hDlg, IDC_SCROLL_MOD2);

        if (reinterpret_cast<HWND>(lParam) != hScroll)
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

        if (g_value < 1) g_value = 1;
        if (g_value > 100) g_value = 100;

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
```

### Файл ідентифікаторів `resource.h`

```cpp
#define IDR_MAINMENU 101

#define IDM_WORK1 40001
#define IDM_WORK2 40002
#define IDM_EXIT 40003
#define IDM_ABOUT 40004

#define IDD_DIALOG_MOD1 201
#define IDC_EDIT_MOD1 1001

#define IDD_DIALOG_MOD2 202
#define IDC_SCROLL_MOD2 1002
#define IDC_VALUE_MOD2 1003
```

### Головний ресурс `Lab1.rc`

```rc
#pragma code_page(65001)

#include <windows.h>
#include "../src/resource.h"

IDR_MAINMENU MENU
BEGIN
    POPUP "&Файл"
    BEGIN
        MENUITEM "Вихід", IDM_EXIT
    END

    POPUP "&Робота"
    BEGIN
        MENUITEM "Робота 1", IDM_WORK1
        MENUITEM "Робота 2", IDM_WORK2
    END

    POPUP "&Довідка"
    BEGIN
        MENUITEM "Про програму", IDM_ABOUT
    END
END
```

### Ресурс модуля 1 `module1.rc`

```rc
#pragma code_page(65001)

#include <windows.h>
#include "../src/resource.h"

IDD_DIALOG_MOD1 DIALOGEX 0, 0, 230, 90
STYLE DS_SETFONT | DS_MODALFRAME | WS_POPUP | WS_CAPTION | WS_SYSMENU
CAPTION "Робота 1 - введення тексту"
FONT 9, "Segoe UI"
BEGIN
    LTEXT         "Введіть текст:", -1, 10, 12, 80, 12
    EDITTEXT      IDC_EDIT_MOD1, 10, 28, 210, 14, ES_AUTOHSCROLL
    DEFPUSHBUTTON "Так", IDOK, 65, 60, 45, 15
    PUSHBUTTON    "Відміна", IDCANCEL, 120, 60, 50, 15
END
```

### Ресурс модуля 2 `module2.rc`

```rc
#pragma code_page(65001)

#include <windows.h>
#include "../src/resource.h"

IDD_DIALOG_MOD2 DIALOGEX 0, 0, 240, 100
STYLE DS_SETFONT | DS_MODALFRAME | WS_POPUP | WS_CAPTION | WS_SYSMENU
CAPTION "Робота 2 - вибір числа"
FONT 9, "Segoe UI"
BEGIN
    LTEXT         "Вибране число:", -1, 10, 12, 75, 12
    LTEXT         "1", IDC_VALUE_MOD2, 90, 12, 40, 12
    SCROLLBAR     IDC_SCROLL_MOD2, 10, 32, 220, 14, SBS_HORZ
    DEFPUSHBUTTON "Так", IDOK, 68, 68, 45, 15
    PUSHBUTTON    "Відміна", IDCANCEL, 125, 68, 50, 15
END
```

---

## Діаграма залежностей файлів та модулів

```text
                         Lab1.cpp
                    ┌────────┼────────┐
                    │        │        │
                    ▼        ▼        ▼
              resource.h  module1.h module2.h
                             │         │
                      ┌──────┘         └──────┐
                      ▼                       ▼
                 module1.cpp             module2.cpp
                  │       │               │       │
                  ▼       ▼               ▼       ▼
             module1.h resource.h    module2.h resource.h

Lab1.rc ────────────────► ../src/resource.h
module1.rc ─────────────► ../src/resource.h
module2.rc ─────────────► ../src/resource.h
```

Головний файл залежить від інтерфейсів `module1.h` та `module2.h`. Модулі не включають один одного і не мають перехресних `#include`-зв'язків. Callback-функції діалогів є внутрішніми для відповідних модулів.

---

## Скріншоти роботи програми

### Головне вікно

<img src="../screenshots/main_window.png" style="width: 100%; max-width: 800px;">

_Рис. 1. Головне вікно програми з меню «Файл», «Робота» та «Довідка»._

---

### Діалог «Робота 1»

<img src="../screenshots/dialog_work1.png" style="width: 100%; max-width: 800px;">

_Рис. 2. Діалогове вікно модуля 1 для введення тексту._

---

### Результат виконання «Робота 1»

<img src="../screenshots/result_work1.png" style="width: 100%; max-width: 800px;">

_Рис. 3. Відображення введеного тексту у головному вікні._

---

### Діалог «Робота 2»

<img src="../screenshots/dialog_work2.png" style="width: 100%; max-width: 800px;">

_Рис. 4. Діалогове вікно модуля 2 з горизонтальним Scroll Bar._

---

### Результат виконання «Робота 2»

<img src="../screenshots/result_work2.png" style="width: 100%; max-width: 800px;">

_Рис. 5. Відображення вибраного числового значення у головному вікні._

---

## Аналіз реалізації

Програма є застосунком, керованим повідомленнями Windows. Функція `wWinMain` реєструє клас головного вікна, створює вікно та запускає цикл отримання повідомлень `GetMessage`. Повідомлення передаються Callback-функції `WndProc`.

Повідомлення `WM_COMMAND` використовується для обробки пунктів меню. Команди `IDM_WORK1` і `IDM_WORK2` викликають зовнішні інтерфейсні функції `Func_MOD1` та `Func_MOD2`.

У модулі 1 функція `DialogBoxW` створює модальне діалогове вікно. Після натискання кнопки «Так» функція `GetDlgItemTextW` читає текст із `Edit Control`. Якщо користувач натискає «Відміна» або закриває діалог, модуль повертає `0`.

У модулі 2 діапазон горизонтального Scroll Bar задається від `1` до `100`. Повідомлення `WM_HSCROLL` обробляє натискання стрілок, сторінкові переходи та переміщення повзунка. Поточне значення відображається у статичному елементі діалогу.

Результати обох модулів виводяться у головному вікні під час обробки `WM_PAINT`. Для коректного відображення українського тексту застосовано Unicode-функції WinAPI, UTF-8 для вихідних файлів та шрифт `Segoe UI`.

---

## Висновки

У лабораторній роботі було створено Windows-застосунок на C++ з використанням Windows API та модульного підходу. Для номера у списку `16` реалізовано варіанти `В1 = 0` та `В2 = 1`.

У першому незалежному модулі реалізовано введення текстового рядка через `Edit Control`. У другому модулі створено діалог із горизонтальним Scroll Bar для вибору числа від 1 до 100. Результати роботи обох модулів відображаються у головному вікні.

Під час виконання роботи було практично опрацьовано реєстрацію та створення Win32-вікна, цикл обробки повідомлень, Callback-функції, повідомлення `WM_COMMAND`, `WM_PAINT`, `WM_HSCROLL`, модальні діалоги, ресурси `.rc`, роздільну компіляцію та формування мінімального інтерфейсу модулів через заголовочні файли.

Модульна структура дозволяє відокремити реалізацію двох незалежних функцій від головного файлу, приховати внутрішні деталі модулів та спростити подальше розширення програми.
