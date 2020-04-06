cmake_minimum_required(VERSION 3.16)
project(JectorDLL)

set(CMAKE_CXX_STANDARD 14)

# Additionally include directories of the Java Native Interface API are needed to compile the DLL
include_directories("C:/Program Files/Java/jdk1.8.0_241/include" "C:/Program Files/Java/jdk1.8.0_241/include/win32")
# Preprocessor definitions are used to tell the injector to invoke the custom DLL entry and to use LoadRemoteLibraryR
add_definitions(-DREFLECTIVEDLLINJECTION_CUSTOM_DLLMAIN -DREFLECTIVEDLLINJECTION_VIA_LOADREMOTELIBRARYR)

add_library(JectorDLL SHARED dllmain.cpp Java.hpp Java.cpp ReflectiveDLLInjection.h ReflectiveLoader.h ReflectiveLoader.c)
