cmake_minimum_required(VERSION 3.16)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../include)

set(LIBRARY_NAME rpiuart)
set(SRC_FILES rpi-uart.c)
set(INCLUDE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/../include/rpi-uart.h)

add_library(${LIBRARY_NAME} SHARED ${SRC_FILES} ${INCLUDE_FILES})
set_target_properties (${LIBRARY_NAME}  PROPERTIES VERSION 1.0 SOVERSION 1)

install(TARGETS ${LIBRARY_NAME} DESTINATION lib)
install(FILES ${INCLUDE_FILES} DESTINATION include)
