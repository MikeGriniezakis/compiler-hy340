cmake_minimum_required(VERSION 3.27)
project(hy340)

find_package(FLEX REQUIRED)
#find_package(BISON REQUIRED)

set(CMAKE_CXX_STANDARD 17)

FLEX_TARGET(scanner src/scanner/scanner.l ${CMAKE_CURRENT_BINARY_DIR}/scanner.cpp)

add_executable(hy340 ${FLEX_scanner_OUTPUTS})
target_link_libraries(hy340 ${FLEX_LIBRARIES})
