#include "Files.hpp"

BOOL FileExists(const char *path) {
    HANDLE hFile = CreateFile(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        return FALSE;
    }

    CloseHandle(hFile);
    return TRUE;
}

LPVOID LoadResource(DWORD *sizeOfResource) {
    HGLOBAL hResLoad;
    HRSRC hRes;
    LPVOID lpResLock;
    DWORD sRes;

    LPSTR lpName;
#ifdef _WIN64
    lpName = MAKEINTRESOURCE(IDR_JECTOR_DLL_WIN64);
#else
#ifdef _WIN32
    lpName = MAKEINTRESOURCE(IDR_JECTOR_DLL_WIN32);
#else
#error Unknown system
#endif // _WIN32
#endif // _WIN64

    hRes = FindResource(NULL, lpName, TEXT("BINARY"));
    if (hRes == NULL) {
        printf("[!] Could not locate resource");
        return NULL;
    }

    hResLoad = LoadResource(NULL, hRes);
    if (hResLoad == NULL) {
        printf("[!] Could not load resource");
        return NULL;
    }

    lpResLock = LockResource(hResLoad);
    if (lpResLock == NULL) {
        printf("[!] Could not lock resource");
        return NULL;
    }

    sRes = SizeofResource(NULL, hRes);
    if (sizeOfResource) {
        *sizeOfResource = sRes;
    }

    return lpResLock;
}
