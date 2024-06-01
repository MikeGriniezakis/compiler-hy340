cmake_minimum_required(VERSION 3.18)
project(hy340)

find_package(FLEX REQUIRED)
find_package(BISON REQUIRED)

set(CMAKE_CXX_STANDARD 17)

FLEX_TARGET(scanner src/scanner/scanner.l ${CMAKE_CURRENT_BINARY_DIR}/scanner.cpp)
BISON_TARGET(parser src/parser/parser.y ${CMAKE_CURRENT_BINARY_DIR}/parser.cpp COMPILE_FLAGS "--verbose")

include_directories(src/symbol src/symbolTable)

add_executable(
        hy340
        ${FLEX_scanner_OUTPUTS}
        ${BISON_parser_OUTPUTS}
        src/symbolTable/SymbolTable.cpp
        src/symbolTable/SymbolTable.h
        src/symbol/Symbol.h
        src/symbol/Symbol.cpp
        src/quads/Quads.cpp
        src/quads/Quads.h
        src/targetCode/targetCode.cpp
        src/targetCode/targetCode.h
        src/generators/generators.h
        src/generators/generators.cpp
)

target_include_directories(hy340 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(
        hy340
        ${FLEX_LIBRARIES}
        ${BISON_LIBRARIES}
)

add_executable(vm
        src/vm/vm.cpp
        src/vm/vm.h
        src/dispatcher/dispatcher.cpp
        src/dispatcher/dispatcher.h
        src/vm/memory/memory.cpp
        src/vm/memory/memory.h
        src/dispatcher/dispatch_assign.cpp
        src/dispatcher/dispatcher_functions.cpp
        src/vm/toString/toString.cpp
        src/vm/toString/toString.h
        src/dispatcher/dispatch_numeric.cpp
        src/dispatcher/dispatch_comparison.cpp
        src/vm/toBool/toBool.cpp
        src/vm/toBool/toBool.h
        src/dispatcher/dispatch_table.cpp
)

target_include_directories(vm PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(vm)
