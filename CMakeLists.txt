cmake_minimum_required(VERSION 3.0.0)
project(ARCHIVER_AND_UNARCHIVER VERSION 0.1.0 LANGUAGES C)

include(CTest)
enable_testing()

add_executable(ARCHIVER_AND_UNARCHIVER main.c)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
