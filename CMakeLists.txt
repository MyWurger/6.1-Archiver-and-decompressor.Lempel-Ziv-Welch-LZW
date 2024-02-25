cmake_minimum_required(VERSION 3.0.0)
project(ARCHIVER_AND_UNARCHIVER VERSION 0.1.0 LANGUAGES CXX)

include(CTest)
enable_testing()

# Компилятор и его опции
set(CMAKE_CXX_COMPILER g++)
set(CMAKE_CXX_STANDARD 11)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")

# Внешний синтаксический анализатор cppcheck
find_program(CPPCHECK cppcheck)
if(CPPCHECK)
    set(CPPCHECK_OPTIONS --enable=all)
    set(CPPCHECK_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)
    add_custom_target(cppcheck COMMAND ${CPPCHECK} ${CPPCHECK_OPTIONS} ${CPPCHECK_SOURCE_DIR})
endif()

add_executable(ARCHIVER_AND_UNARCHIVER main.cpp)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)