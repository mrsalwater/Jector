#include "Process.hpp"

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) {
    HandleData &data = *(HandleData *) lParam;
    unsigned long pid = 0;

    GetWindowThreadProcessId(hwnd, &pid);
    if (data.id == pid && GetWindow(hwnd, GW_OWNER) == NULL) {
        data.window = hwnd;
        return FALSE;
    }

    return TRUE;
}

std::vector<ProcessData *> GetProcessList(const char **processFileNames, unsigned int processFileNamesCount) {
    std::vector<ProcessData *> processes;

    HANDLE hProcessSnap;
    PROCESSENTRY32 pe32;

    hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hProcessSnap == INVALID_HANDLE_VALUE) {
        return processes;
    }

    pe32.dwSize = sizeof(PROCESSENTRY32);

    if (!Process32First(hProcessSnap, &pe32)) {
        CloseHandle(hProcessSnap);
        return processes;
    }

    do {
        for (int i = 0; i < processFileNamesCount; ++i) {
            const char *processFileName = processFileNames[i];

            if (strcmp(pe32.szExeFile, processFileName) == 0) {
                DWORD pid = pe32.th32ProcessID;
                CHAR *name = GetProcessName(pid);

                auto *data = new ProcessData;
                data->id = pid;
                data->name = name;

                processes.push_back(data);
            }
        }
    } while (Process32Next(hProcessSnap, &pe32));

    CloseHandle(hProcessSnap);
    return processes;
}

CHAR *GetProcessName(DWORD pid) {
    HandleData data{};
    data.id = pid;
    EnumWindows(EnumWindowsProc, (LPARAM) &data);

    int length = GetWindowTextLength(data.window) + 1;
    CHAR *lpString = new CHAR[length];
    GetWindowText(data.window, lpString, length);

    return lpString;
}

BOOL ProcessExists(DWORD pid) {
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pid);
    if (hProcess == nullptr) {
        return FALSE;
    }

    CloseHandle(hProcess);
    return TRUE;
}
