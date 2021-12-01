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

// fungsi detour
BOOL WINAPI Hooked_WF(HANDLE hFile, LPCVOID lpBuffer, DWORD nOBTW, LPDWORD lpNOBW, LPOVERLAPPED lpO) {
    
    //manipulasi atau monitor disini
    MessageBoxA(HWND_DESKTOP, (LPCSTR)lpBuffer, "isi lpBuffer", MB_OK);
    return True_WF(hFile, lpBuffer, nOBTW, lpNOBW, lpO);
}


// Task Solution

/*
HANDLE CreateFileA(
  [in]           LPCSTR                lpFileName,
  [in]           DWORD                 dwDesiredAccess,
  [in]           DWORD                 dwShareMode,
  [in, optional] LPSECURITY_ATTRIBUTES lpSecurityAttributes,
  [in]           DWORD                 dwCreationDisposition,
  [in]           DWORD                 dwFlagsAndAttributes,
  [in, optional] HANDLE                hTemplateFile
);
*/

// alamat CreateFile
HANDLE (WINAPI* True_CF)(LPCWSTR, DWORD, DWORD, LPSECURITY_ATTRIBUTES, DWORD, DWORD, HANDLE) = CreateFile;

//fungsi detour
HANDLE WINAPI Hooked_CF(LPCWSTR Fn, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile) {

    // cek parameter dwCreateDisposition
    if (dwCreationDisposition == CREATE_NEW) {

        MessageBox(HWND_DESKTOP, Fn, L"New File Created", MB_OK);
    }


    return True_CF(Fn, dwDesiredAccess,  dwShareMode,  lpSecurityAttributes,  dwCreationDisposition,  dwFlagsAndAttributes, hTemplateFile);
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
        DetourAttach(&(PVOID&)True_CF, Hooked_CF);

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
        DetourDetach(&(PVOID&)True_CF, Hooked_CF);

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

