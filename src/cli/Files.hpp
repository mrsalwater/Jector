#ifndef JECTOR_CLI_FILES_HPP
#define JECTOR_CLI_FILES_HPP

#include <cstdio>
#include <vector>

#include <windows.h>
#include <strsafe.h>

#include "resource.h"

BOOL FileExists(const char *path);

LPVOID LoadResource(DWORD *sizeOfResource);

#endif //JECTOR_CLI_FILES_HPP
