#include <windows.h>
#include <vector>
#include <string>
#include <sstream>
#include <random>
#include <cstring>
#include "../common/Protocol.h"

using namespace Lab6Protocol;

namespace
{
    std::vector<POINT> g_points;
    HWND g_hEdit = nullptr;
    HWND g_hManager = nullptr;

    std::wstring BuildPointsText()
    {
        std::wstringstream ss;
        ss << L"x\ty\r\n";
        for (const POINT& p : g_points)
            ss << p.x << L'\t' << p.y << L"\r\n";
        return ss.str();
    }

    bool PutUnicodeTextToClipboard(HWND hWnd, const std::wstring& text)
    {
        if (text.empty())
            return false;

        const SIZE_T bytes = (text.size() + 1) * sizeof(wchar_t);
        HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, bytes);
        if (!hMem)
            return false;

        void* dst = GlobalLock(hMem);
        if (!dst)
        {
            GlobalFree(hMem);
            return false;
        }

        memcpy(dst, text.c_str(), bytes);
        GlobalUnlock(hMem);

        if (!OpenClipboard(hWnd))
        {
            GlobalFree(hMem);
            return false;
        }

        EmptyClipboard();
        if (!SetClipboardData(CF_UNICODETEXT, hMem))
        {
            CloseClipboard();
            GlobalFree(hMem);
            return false;
        }

        CloseClipboard();
        return true; // ownership of hMem is now transferred to Windows
    }

    void GeneratePoints(const Params& p)
    {
        g_points.clear();
        g_points.reserve(static_cast<size_t>(p.nPoint));

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<int> dx(p.xMin, p.xMax);
        std::uniform_int_distribution<int> dy(p.yMin, p.yMax);

        for (int i = 0; i < p.nPoint; ++i)
        {
            POINT pt{};
            pt.x = dx(gen);
            pt.y = dy(gen);
            g_points.push_back(pt);
        }
    }

    LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        switch (msg)
        {
        case WM_CREATE:
            g_hEdit = CreateWindowExW(
                WS_EX_CLIENTEDGE, L"EDIT", L"Очікування параметрів від Lab6...",
                WS_CHILD | WS_VISIBLE | ES_MULTILINE | ES_READONLY | ES_AUTOVSCROLL |
                WS_VSCROLL | WS_HSCROLL,
                0, 0, 0, 0, hWnd, nullptr,
                reinterpret_cast<LPCREATESTRUCTW>(lParam)->hInstance, nullptr);
            return 0;

        case WM_SIZE:
            if (g_hEdit)
                MoveWindow(g_hEdit, 8, 8, LOWORD(lParam) - 16, HIWORD(lParam) - 16, TRUE);
            return 0;

        case WM_COPYDATA:
        {
            const COPYDATASTRUCT* cds = reinterpret_cast<const COPYDATASTRUCT*>(lParam);
            if (!cds || cds->dwData != COPYDATA_PARAMS || cds->cbData != sizeof(Params) || !cds->lpData)
                return FALSE;

            const Params p = *reinterpret_cast<const Params*>(cds->lpData);
            if (p.nPoint <= 0 || p.xMin > p.xMax || p.yMin > p.yMax)
                return FALSE;

            g_hManager = reinterpret_cast<HWND>(wParam);
            GeneratePoints(p);
            const std::wstring text = BuildPointsText();
            SetWindowTextW(g_hEdit, text.c_str());
            SetWindowTextW(hWnd, L"Object2 — згенеровані точки та Clipboard");

            const bool clipboardOk = PutUnicodeTextToClipboard(hWnd, text);
            if (!clipboardOk)
            {
                MessageBoxW(hWnd, L"Не вдалося записати дані у Clipboard.", L"Object2", MB_OK | MB_ICONERROR);
                return FALSE;
            }

            if (!g_hManager)
                g_hManager = FindWindowW(MANAGER_CLASS, nullptr);
            if (g_hManager)
                PostMessageW(g_hManager, WM_OBJECT2_DONE, reinterpret_cast<WPARAM>(hWnd), 0);

            return TRUE;
        }

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
    wc.lpszClassName = OBJECT2_CLASS;
    RegisterClassExW(&wc);

    HWND hWnd = CreateWindowExW(0, OBJECT2_CLASS, L"Object2 — очікування",
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, 520, 500,
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
