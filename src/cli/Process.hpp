#ifndef JECTOR_CLI_PROCESS_HPP
#define JECTOR_CLI_PROCESS_HPP

#include <vector>

#include <windows.h>
#include <tlhelp32.h>
#include <tchar.h>

struct ProcessData {
    DWORD id;
    char *name;
};

struct HandleData {
    DWORD id;
    HWND window;
};

std::vector<ProcessData *> GetProcessList(const char **processFileNames, unsigned int processFileNamesCount);

CHAR *GetProcessName(DWORD pid);

BOOL ProcessExists(DWORD pid);

#endif //JECTOR_CLI_PROCESS_HPP
