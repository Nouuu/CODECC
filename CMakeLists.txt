cmake_minimum_required(VERSION 3.1)
project(CODECC LANGUAGES C)
enable_language(C)

set(CMAKE_C_STANDARD 11)

FIND_PACKAGE(PkgConfig REQUIRED)
PKG_CHECK_MODULES(GTK3 REQUIRED gtk+-3.0)
INCLUDE_DIRECTORIES(${GTK3_INCLUDE_DIRS})
LINK_DIRECTORIES(${GTK3_LIBRARY_DIRS})
ADD_DEFINITIONS(${GTK3_CFLAGS_OTHER})

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pthread")


add_executable(CODECC main.c gtkFunctions.c codecFunctions.c functions.c)

TARGET_LINK_LIBRARIES(CODECC -lm ${GTK3_LIBRARIES})

#target_compile_options(CODECC PRIVATE -Werror)
#set_target_properties(CODECC PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS true)
#set_target_properties(CODECC PROPERTIES LINK_FLAGS "-static")
