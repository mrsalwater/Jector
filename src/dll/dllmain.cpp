#include <string>
#include <Windows.h>

#include "Java.hpp"

const char *methodName = "entry";
const char *methodSignature = "()V";

enum class PrintType {
    INFO, ERR, DEBUG
};

static const char *StringCopy(const std::string &string) {
    char *copy = new char[string.size() + 1];
    std::copy(string.begin(), string.end(), copy);
    copy[string.size()] = '\0';
    return copy;
}

static void Print(PrintType type, const char *string) {
    std::string message = std::string("[INJ-JVM] ");

    const char *prefix;
    switch (type) {
        case PrintType::INFO:
            prefix = "[Info] ";
            break;
        case PrintType::ERR:
            prefix = "[Error] ";
            break;
        case PrintType::DEBUG:
            prefix = "[Debug] ";
            break;
    }

    OutputDebugString(message.append(prefix).append(string).c_str());
}

static void Inject(const char *path, const char *className) {
    JContext *jContext;
    int returnCode;

    Print(PrintType::INFO, "Injected DLL into java process.");

    jContext = new JContext;
    if (!AttachJVM(jContext)) {
        Print(PrintType::ERR, "Failed to attach to the jvm.");
        goto out;
    }

    Print(PrintType::INFO, "Attached to the jvm.");

    returnCode = InjectJar(jContext, path, className, methodName, methodSignature);
    if (returnCode == INJ_OK) {
        Print(PrintType::INFO, "Injected jar into jvm and invoked method.");
    } else {
        Print(PrintType::ERR, "Failed to inject jar into jvm and invoke method.");
    }

    out:
    if (!DetachJVM(jContext)) Print(PrintType::ERR, "Failed to detach from the jvm.");
    delete jContext;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
        std::string parameter = std::string((const char *) lpReserved);
        size_t iSpace = parameter.find_last_of(' ');

        const char *path = StringCopy(parameter.substr(0, iSpace));
        const char *className = StringCopy(parameter.substr(iSpace + 1, parameter.length()));
        Inject(path, className);

        VirtualFreeEx((HANDLE) -1, lpReserved, 0, MEM_RELEASE);
        FreeLibrary(hModule);
    }

    return TRUE;
}
