cmake_minimum_required(VERSION 3.16)
project(JectorCLI)

set(CMAKE_CXX_STANDARD 14)

add_executable(JectorCLI Jector.cpp
        Files.hpp Files.cpp Process.hpp Process.cpp Injector.hpp Injector.cpp
        ReflectiveDLLInjection.h LoadLibraryR.h LoadLibraryR.c GetProcAddressR.h GetProcAddressR.c
        resource.rc)
