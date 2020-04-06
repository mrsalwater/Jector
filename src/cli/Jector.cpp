#include <cstdio>
#include <string>
#include <iostream>

#include "Files.hpp"
#include "Process.hpp"
#include "Injector.hpp"

const char *name = "Jector";
const char *version = "1.0.0";

#ifdef _WIN64
const char *architecture = "64-bit";
#else
#ifdef _WIN32
const char *architecture = "32-bit";
#else
#error Unknown system
#endif // _WIN32
#endif // _WIN64

const char *javaProcessFileNames[2] = {"java.exe", "javaw.exe"};
const unsigned int javaProcessFileNamesCount = 2;

const char *input_file;
const char *class_name;

static const char *StringCopy(const std::string &string) {
    char *copy = new char[string.size() + 1];
    std::copy(string.begin(), string.end(), copy);
    copy[string.size()] = '\0';
    return copy;
}

static inline bool StringIsNumber(const std::string &s) {
    std::string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it)) ++it;
    return !s.empty() && it == s.end();
}

static inline std::string ParseString() {
    std::string line;
    std::getline(std::cin, line);
    return line;
}

static void PrintInfo() {
    printf("%s %s [Version %s]\n", name, architecture, version);
    printf("Type \"help\" for more information\n");
    printf("(c) 2020 mrsalwater [https://www.github.com/mrsalwater/Jector]. All rights reserved.\n");
}

static void PrintHelp() {
    printf("Available commands are:\n");
    printf(" help                               prints all available commands\n");
    printf(" select <input_file> <class_name>   selects input file to be injected and class name to be executed\n");
    printf(" inject <pid>                       injects the selected file into the process with specified id\n");
    printf(" inject list                        injects the selected file into a process of the list\n");
    printf(" exit                               exits the program\n");
}

static void PrintUsage() {
    printf("[!] Wrong usage, type \"help\" for more information\n");
}

static void InjectFile(DWORD pid) {
    const char *parameter = StringCopy(std::string(input_file).append(" ").append(class_name));
    if (Inject(pid, parameter)) {
        printf("Injected into process with id %lu\n", pid);
    }

    delete parameter;
}

static void ParseInputInjectList() {
    std::vector<ProcessData *> processes = GetProcessList(javaProcessFileNames, javaProcessFileNamesCount);

    if (processes.empty()) {
        printf("[!] No java processes found\n");
    } else {
        printf("Select process to inject by entering the index of the java process:\n");
        for (std::vector<ProcessData>::size_type i = 0; i != processes.size(); i++) {
            ProcessData *data = processes[i];
            printf("	[%d] %s (pid: %lu)\n", (int) i + 1, data->name, data->id);
        }

        printf("\nEnter index\n> ");
        std::string input = ParseString();
        if (!StringIsNumber(input)) {
            PrintUsage();
            return;
        }

        int index = strtol(input.c_str(), nullptr, 0);
        if (index < 1 || index > processes.size()) {
            printf("[!] Invalid index entered\n");
        } else {
            ProcessData *data = processes.at((size_t) index - 1);
            InjectFile(data->id);
        }

        for (auto *process : processes) {
            delete process->name;
            delete process;
        }
    }
}

static void ParseInput() {
    printf("\n> ");
    std::string line = ParseString();

    if (line == "exit") {
        return;
    } else if (line == "help") {
        PrintHelp();
    } else if (line.substr(0, 6) == "select") {
        if (line.size() < 7) {
            PrintUsage();
        } else {
            std::string argument = line.substr(7, line.size());
            size_t nSpace = std::count(argument.begin(), argument.end(), ' ');
            size_t iSpace = argument.find_last_of(' ');

            if (nSpace != 1 || iSpace == std::string::npos) {
                PrintUsage();
            } else {
                std::string argumentInputFile = argument.substr(0, iSpace);
                std::string argumentClassName = argument.substr(iSpace + 1, argument.size());

                argumentInputFile.erase(std::remove(argumentInputFile.begin(), argumentInputFile.end(), '\"'), argumentInputFile.end());
                const char *file = argumentInputFile.c_str();

                if (!FileExists(file)) {
                    printf("[!] Input file does not exist\n");
                } else if (argumentClassName.empty()) {
                    printf("[!] Invalid class name\n");
                } else {
                    input_file = StringCopy(argumentInputFile);
                    class_name = StringCopy(argumentClassName);
                    printf("Selected input file \"%s\" and classname \"%s\"\n", input_file, class_name);
                }
            }
        }
    } else if (line.substr(0, 6) == "inject") {
        if (line.size() < 7) {
            PrintUsage();
        } else {
            if (input_file == nullptr) {
                printf("[!] Input file is not selected\n");
            } else {
                std::string argument = line.substr(7, line.size());

                if (argument == "list") {
                    ParseInputInjectList();
                } else {
                    if (StringIsNumber(argument)) {
                        int pid = strtol(argument.c_str(), nullptr, 0);

                        if (!ProcessExists(pid)) {
                            printf("[!] Process does not exist\n");
                        } else {
                            InjectFile(pid);
                        }
                    } else {
                        PrintUsage();
                    }
                }
            }
        }
    } else {
        printf("[!] Unknown command, type help for more information\n");
    }

    ParseInput();
}

int main() {
    SetConsoleTitleA(name);
    PrintInfo();
    ParseInput();
    return 0;
}
