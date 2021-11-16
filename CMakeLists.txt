cmake_minimum_required(VERSION 3.16)
project(rpi_uart)

add_subdirectory(lib)

set(SOURCE src/main.c)

include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/lib)

add_executable(main_uart ${SOURCE})

target_link_libraries(main_uart rpiuart)

install (TARGETS main_uart DESTINATION bin)

set(MAJOR "0")
set(MINOR "1")
set(PATCH "1")
set(CPACK_SOURCE_GENERATOR "TGZ")
set(CPACK_SOURCE_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}-${MAJOR}.${MINOR}.${PATCH}")
set(CPACK_SOURCE_IGNORE_FILES "/build/;${CPACK_SOURCE_IGNORE_FILES}")
include(CPack)