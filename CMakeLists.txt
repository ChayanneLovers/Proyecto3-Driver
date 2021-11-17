cmake_minimum_required(VERSION 3.16)
project(rpi_uart)

add_subdirectory(lib)

set(SOURCE src/main.c)

include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/lib)

add_executable(main_uart ${SOURCE})

if (CMAKE_CROSSCOMPILING)

target_link_libraries(main_uart rpiuart)

install (TARGETS main_uart DESTINATION bin)
install (TARGETS rpiuart DESTINATION lib)
install (FILES rpi-uart.h DESTINATION include)

endif (CMAKE_CROSSCOMPILING)