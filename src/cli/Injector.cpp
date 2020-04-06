#include "Injector.hpp"

void AdjustPrivileges() {
    HANDLE hToken;
    TOKEN_PRIVILEGES privileges;

    if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) {
        privileges.PrivilegeCount = 1;
        privileges.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

        if (LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &privileges.Privileges[0].Luid)) {
            AdjustTokenPrivileges(hToken, FALSE, &privileges, 0, NULL, NULL);
        }

        CloseHandle(hToken);
    }
}

BOOL IsProcessWow64(HANDLE hProcess) {
    USHORT pProcessMachine;
    USHORT pNativeMachine;

    if (!IsWow64Process2(hProcess, &pProcessMachine, &pNativeMachine)) {
        printf("[!] Unable to retrieve process architecture");
    }

    return pProcessMachine != IMAGE_FILE_MACHINE_UNKNOWN;
}

BOOL Inject(DWORD pid, LPVOID lpBuffer, SIZE_T dwLength, LPVOID lpParameter, SIZE_T dwSize) {
    HANDLE hProcess;
    LPVOID lpBaseAddress;
    HANDLE hModule;

    hProcess = OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ | PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION, FALSE, pid);
    if (hProcess == NULL) {
        printf("[!] Could not open process\n");
        return FALSE;
    }

#ifdef _WIN64
    if (IsProcessWow64(hProcess)) {
        printf("[!] The process is a 32-bit application! Download the 32-bit Jector application to inject into the process.\n");
        CloseHandle(hProcess);
        return FALSE;
    }
#else
#ifdef _WIN32
    if (!IsProcessWow64(hProcess)) {
        printf("[!] The process is a 64-bit application! Download the 64-bit Jector application to inject into the process.\n");
        CloseHandle(hProcess);
        return FALSE;
    }
#else
#error Unknown system
#endif // _WIN32
#endif // _WIN64

    lpBaseAddress = VirtualAllocEx(hProcess, NULL, dwSize, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    if (lpBaseAddress == NULL) {
        printf("[!] Could not allocate memory\n");
        CloseHandle(hProcess);
        return FALSE;
    }

    if (!WriteProcessMemory(hProcess, lpBaseAddress, lpParameter, dwSize, NULL)) {
        printf("[!] Could not write to memory\n");
        VirtualFreeEx(hProcess, lpBaseAddress, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return FALSE;
    }


    hModule = LoadRemoteLibraryR(hProcess, lpBuffer, dwLength, lpBaseAddress);
    if (hModule == NULL) {
        printf("[!] Could not load remote library\n");
        VirtualFreeEx(hProcess, lpBaseAddress, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return FALSE;
    }

    WaitForSingleObject(hModule, INFINITE);
    CloseHandle(hModule);
    CloseHandle(hProcess);

    return TRUE;
}

BOOL Inject(DWORD pid, const char *parameter) {
    LPVOID resourceBuffer;
    DWORD sizeOfResource;

    resourceBuffer = LoadResource(&sizeOfResource);
    if (resourceBuffer == NULL) {
        return FALSE;
    }

    AdjustPrivileges();
    return Inject(pid, resourceBuffer, sizeOfResource, (LPVOID) parameter, strlen(parameter) * sizeof(char));
}
