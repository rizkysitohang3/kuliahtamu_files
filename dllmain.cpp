// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "detours.h"
#include "windows.h"
#pragma comment(lib,"detours.lib")

/* Prototipe WriteFile
BOOL WriteFile(
  [in]                HANDLE       hFile,
  [in]                LPCVOID      lpBuffer,
  [in]                DWORD        nNumberOfBytesToWrite,
  [out, optional]     LPDWORD      lpNumberOfBytesWritten,
  [in, out, optional] LPOVERLAPPED lpOverlapped
);

*/

// Alamat WriteFile
BOOL (WINAPI *True_WF)(HANDLE, LPCVOID, DWORD, LPDWORD, LPOVERLAPPED) = WriteFile;

// fungsi yang detour
BOOL WINAPI Hooked_WF(HANDLE hFile, LPCVOID lpBuffer, DWORD nOBTW, LPDWORD lpNOBW, LPOVERLAPPED lpO) {
    
    //manipulasi atau monitor disini
    MessageBoxA(HWND_DESKTOP, (LPCSTR)lpBuffer, "isi lpBuffer", MB_OK);
    return True_WF(hFile, lpBuffer, nOBTW, lpNOBW, lpO);
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  dwReason,
                       LPVOID lpReserved
                     )
{
    switch (dwReason)
    {
    case DLL_PROCESS_ATTACH:
    {
        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());
        DetourAttach(&(PVOID&)True_WF, Hooked_WF);

        LONG lError = DetourTransactionCommit();
        if (lError != NO_ERROR) {
            MessageBox(HWND_DESKTOP, L"Failed to attach", L"Error", MB_OK);
            return FALSE;
        }

    }
    break;
    //case DLL_THREAD_ATTACH:
    //case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
    {
        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());
        DetourDetach(&(PVOID&)True_WF, Hooked_WF);

        LONG lError = DetourTransactionCommit();
        if (lError != NO_ERROR) {
            MessageBox(HWND_DESKTOP, L"Failed to detach", L"Error", MB_OK);
            return FALSE;
        }
    }
    break;
    }
    return TRUE;
}

