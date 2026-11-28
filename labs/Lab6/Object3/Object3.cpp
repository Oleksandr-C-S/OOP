#include <windows.h>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <limits>
#include <cmath>
#include <climits>
#include "../common/Protocol.h"

using namespace Lab6Protocol;

namespace
{
    std::vector<POINT> g_points;
    HWND g_hManager = nullptr;
    std::wstring g_error;

    bool ReadUnicodeTextFromClipboard(HWND hWnd, std::wstring& out)
    {
        out.clear();
        if (!IsClipboardFormatAvailable(CF_UNICODETEXT))
            return false;
        if (!OpenClipboard(hWnd))
            return false;

        HGLOBAL hMem = GetClipboardData(CF_UNICODETEXT);
        if (!hMem)
        {
            CloseClipboard();
            return false;
        }

        const wchar_t* src = static_cast<const wchar_t*>(GlobalLock(hMem));
        if (!src)
        {
            CloseClipboard();
            return false;
        }

        out = src;
        GlobalUnlock(hMem);
        CloseClipboard();
        return true;
    }

    bool ParsePoints(const std::wstring& text)
    {
        g_points.clear();
        std::wstringstream all(text);
        std::wstring line;

        while (std::getline(all, line))
        {
            if (!line.empty() && line.back() == L'\r')
                line.pop_back();
            if (line.empty())
                continue;

            std::wistringstream row(line);
            long long x = 0, y = 0;
            if (!(row >> x >> y))
                continue; // skips header "x y"

            if (x < LONG_MIN || x > LONG_MAX || y < LONG_MIN || y > LONG_MAX)
                continue;

            POINT p{};
            p.x = static_cast<LONG>(x);
            p.y = static_cast<LONG>(y);
            g_points.push_back(p);
        }

        std::stable_sort(g_points.begin(), g_points.end(), [](const POINT& a, const POINT& b)
        {
            if (a.x != b.x) return a.x < b.x;
            return a.y < b.y;
        });

        return !g_points.empty();
    }

    int MapX(long x, long minX, long maxX, const RECT& plot)
    {
        if (maxX == minX)
            return (plot.left + plot.right) / 2;
        const double t = static_cast<double>(x - minX) / static_cast<double>(maxX - minX);
        return plot.left + static_cast<int>(t * (plot.right - plot.left));
    }

    int MapY(long y, long minY, long maxY, const RECT& plot)
    {
        if (maxY == minY)
            return (plot.top + plot.bottom) / 2;
        const double t = static_cast<double>(y - minY) / static_cast<double>(maxY - minY);
        return plot.bottom - static_cast<int>(t * (plot.bottom - plot.top));
    }

    void DrawGraph(HWND hWnd, HDC hdc)
    {
        RECT client{};
        GetClientRect(hWnd, &client);
        SetBkMode(hdc, TRANSPARENT);

        if (!g_error.empty())
        {
            DrawTextW(hdc, g_error.c_str(), -1, &client, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
            return;
        }
        if (g_points.empty())
        {
            const wchar_t* text = L"Очікування даних із Clipboard...";
            DrawTextW(hdc, text, -1, &client, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
            return;
        }

        RECT plot{ client.left + 80, client.top + 55, client.right - 45, client.bottom - 70 };
        if (plot.right <= plot.left + 20 || plot.bottom <= plot.top + 20)
            return;

        long minX = g_points.front().x;
        long maxX = g_points.front().x;
        long minY = g_points.front().y;
        long maxY = g_points.front().y;
        for (const POINT& p : g_points)
        {
            minX = min(minX, p.x);
            maxX = max(maxX, p.x);
            minY = min(minY, p.y);
            maxY = max(maxY, p.y);
        }
        if (minX == maxX) { --minX; ++maxX; }
        if (minY == maxY) { --minY; ++maxY; }

        // Title
        HFONT hTitle = CreateFontW(22, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
            CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI");
        HFONT oldFont = static_cast<HFONT>(SelectObject(hdc, hTitle));
        const wchar_t* title = L"Object3 — графік y = f(x)";
        TextOutW(hdc, plot.left, 15, title, lstrlenW(title));
        SelectObject(hdc, oldFont);
        DeleteObject(hTitle);

        // Plot border
        Rectangle(hdc, plot.left, plot.top, plot.right, plot.bottom);

        const int xAxisY = (minY <= 0 && maxY >= 0) ? MapY(0, minY, maxY, plot) : plot.bottom;
        const int yAxisX = (minX <= 0 && maxX >= 0) ? MapX(0, minX, maxX, plot) : plot.left;

        HPEN axisPen = CreatePen(PS_SOLID, 2, RGB(60, 60, 60));
        HPEN oldPen = static_cast<HPEN>(SelectObject(hdc, axisPen));
        MoveToEx(hdc, plot.left, xAxisY, nullptr);
        LineTo(hdc, plot.right, xAxisY);
        MoveToEx(hdc, yAxisX, plot.top, nullptr);
        LineTo(hdc, yAxisX, plot.bottom);
        SelectObject(hdc, oldPen);
        DeleteObject(axisPen);

        // Labels
        std::wstringstream labels;
        std::wstring s;
        s = L"xMin=" + std::to_wstring(minX);
        TextOutW(hdc, plot.left, plot.bottom + 10, s.c_str(), static_cast<int>(s.size()));
        s = L"xMax=" + std::to_wstring(maxX);
        SIZE sz{};
        GetTextExtentPoint32W(hdc, s.c_str(), static_cast<int>(s.size()), &sz);
        TextOutW(hdc, plot.right - sz.cx, plot.bottom + 10, s.c_str(), static_cast<int>(s.size()));
        s = L"yMax=" + std::to_wstring(maxY);
        TextOutW(hdc, 8, plot.top - 5, s.c_str(), static_cast<int>(s.size()));
        s = L"yMin=" + std::to_wstring(minY);
        TextOutW(hdc, 8, plot.bottom - 15, s.c_str(), static_cast<int>(s.size()));
        s = L"Кількість точок: " + std::to_wstring(g_points.size());
        TextOutW(hdc, plot.left, client.bottom - 32, s.c_str(), static_cast<int>(s.size()));

        // Graph polyline
        HPEN graphPen = CreatePen(PS_SOLID, 2, RGB(0, 90, 190));
        oldPen = static_cast<HPEN>(SelectObject(hdc, graphPen));
        for (size_t i = 0; i < g_points.size(); ++i)
        {
            const int px = MapX(g_points[i].x, minX, maxX, plot);
            const int py = MapY(g_points[i].y, minY, maxY, plot);
            if (i == 0)
                MoveToEx(hdc, px, py, nullptr);
            else
                LineTo(hdc, px, py);
        }
        SelectObject(hdc, oldPen);
        DeleteObject(graphPen);

        HBRUSH pointBrush = CreateSolidBrush(RGB(190, 30, 30));
        HBRUSH oldBrush = static_cast<HBRUSH>(SelectObject(hdc, pointBrush));
        for (const POINT& p : g_points)
        {
            const int px = MapX(p.x, minX, maxX, plot);
            const int py = MapY(p.y, minY, maxY, plot);
            Ellipse(hdc, px - 3, py - 3, px + 4, py + 4);
        }
        SelectObject(hdc, oldBrush);
        DeleteObject(pointBrush);
    }

    LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        switch (msg)
        {
        case WM_OBJECT3_PROCESS:
        {
            g_hManager = reinterpret_cast<HWND>(wParam);
            std::wstring text;
            if (!ReadUnicodeTextFromClipboard(hWnd, text))
            {
                g_error = L"Не вдалося прочитати Unicode-текст із Clipboard.";
                g_points.clear();
            }
            else if (!ParsePoints(text))
            {
                g_error = L"У Clipboard немає коректних пар (x, y).";
            }
            else
            {
                g_error.clear();
                SetWindowTextW(hWnd, L"Object3 — графік y=f(x)");
            }

            InvalidateRect(hWnd, nullptr, TRUE);
            UpdateWindow(hWnd);

            if (!g_hManager)
                g_hManager = FindWindowW(MANAGER_CLASS, nullptr);
            if (g_hManager)
                PostMessageW(g_hManager, WM_OBJECT3_DONE, reinterpret_cast<WPARAM>(hWnd), 0);
            return 0;
        }

        case WM_PAINT:
        {
            PAINTSTRUCT ps{};
            HDC hdc = BeginPaint(hWnd, &ps);
            DrawGraph(hWnd, hdc);
            EndPaint(hWnd, &ps);
            return 0;
        }

        case WM_SIZE:
            InvalidateRect(hWnd, nullptr, TRUE);
            return 0;

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        }
        return DefWindowProcW(hWnd, msg, wParam, lParam);
    }
}

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE, LPWSTR, int nCmdShow)
{
    WNDCLASSEXW wc{};
    wc.cbSize = sizeof(wc);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
    wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
    wc.lpszClassName = OBJECT3_CLASS;
    RegisterClassExW(&wc);

    HWND hWnd = CreateWindowExW(0, OBJECT3_CLASS, L"Object3 — очікування",
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, 850, 650,
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
