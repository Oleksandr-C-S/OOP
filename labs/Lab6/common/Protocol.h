#pragma once
#include <windows.h>

namespace Lab6Protocol
{
    constexpr wchar_t MANAGER_CLASS[] = L"LAB6_MANAGER_CLASS";
    constexpr wchar_t OBJECT2_CLASS[] = L"LAB6_OBJECT2_CLASS";
    constexpr wchar_t OBJECT3_CLASS[] = L"LAB6_OBJECT3_CLASS";

    constexpr ULONG_PTR COPYDATA_PARAMS = 0x6001;

    constexpr UINT WM_OBJECT2_DONE = WM_APP + 101;
    constexpr UINT WM_OBJECT3_PROCESS = WM_APP + 102;
    constexpr UINT WM_OBJECT3_DONE = WM_APP + 103;

    struct Params
    {
        int nPoint;
        int xMin;
        int xMax;
        int yMin;
        int yMax;
    };
}
