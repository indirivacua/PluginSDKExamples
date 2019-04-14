#pragma once
#include <Windows.h>

struct handle_data 
{
    unsigned long process_id;
    HWND window_handle;
};

BOOL CALLBACK EnumWindowsCallback(HWND handle, LPARAM lParam);
BOOL IsMainWindow(HWND handle);
HWND FindMainWindow();
void MemCpy(void* destination, void* source, size_t size);