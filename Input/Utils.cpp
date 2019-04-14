#include "Utils.h"

BOOL CALLBACK EnumWindowsCallback(HWND handle, LPARAM lParam)
{
    handle_data& data = *(handle_data*)lParam;
    unsigned long process_id = 0;
    GetWindowThreadProcessId(handle, &process_id);
    if (data.process_id != process_id || !IsMainWindow(handle))
        return TRUE;
    data.window_handle = handle;
    return FALSE;
}

BOOL IsMainWindow(HWND handle)
{
    return GetWindow(handle, GW_OWNER) == (HWND)0 && IsWindowVisible(handle);
}

HWND FindMainWindow()
{
    handle_data data;
    data.process_id = GetCurrentProcessId();
    data.window_handle = 0;
    EnumWindows(EnumWindowsCallback, (LPARAM)&data);
    return data.window_handle;
}

void MemCpy(void* destination, void* source, size_t size)
{
    DWORD dwProtect[2];
    VirtualProtect(destination, 4, PAGE_EXECUTE_READWRITE, &dwProtect[0]);
    memcpy(destination, source, size);
    VirtualProtect(destination, 5, dwProtect[0], &dwProtect[1]);
}