#ifndef JECTOR_CLI_INJECTOR_HPP
#define JECTOR_CLI_INJECTOR_HPP

#include <windows.h>

#include "Files.hpp"

extern "C" {
#include "LoadLibraryR.h"
}

BOOL Inject(DWORD pid, const char *parameter);

#endif //JECTOR_CLI_INJECTOR_HPP
