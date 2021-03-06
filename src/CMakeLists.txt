cmake_minimum_required(VERSION 3.10)

# set the project name
project(Corax VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)

set(LIB_OUTPUT_DIR "${CMAKE_HOME_DIRECTORY}/build/dist")

set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wno-attributes")

# if(DEBUG)
# set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -g -O0")
# endif()

add_subdirectory(antlr4/runtime/Cpp)
add_subdirectory(cli11)
# add_subdirectory(compiler)
# add_subdirectory(precompiler)

include_directories(
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/grammars/build/Corax
    ${CMAKE_SOURCE_DIR}/grammars/build/Pre
    ${CMAKE_SOURCE_DIR}/grammars/build/Enca
    ${CMAKE_SOURCE_DIR}/compiler
    ${CMAKE_SOURCE_DIR}/compiler/targets
    ${CMAKE_SOURCE_DIR}/precompiler
    ${CMAKE_SOURCE_DIR}/assembler
    ${CMAKE_SOURCE_DIR}/antlr4/runtime/Cpp/runtime/src
    ${CMAKE_SOURCE_DIR}/cli11/include/CLI/
    )

set (SOURCE_FILES 
    error.cpp
    utils.cpp

    ./precompiler/precompiler.cpp

    ./grammars/build/Pre/PreBaseListener.cpp
    ./grammars/build/Pre/PreBaseVisitor.cpp
    ./grammars/build/Pre/PreLexer.cpp
    ./grammars/build/Pre/PreParser.cpp
    ./grammars/build/Pre/PreListener.cpp
    ./grammars/build/Pre/PreVisitor.cpp

    ./grammars/build/Pre/PreExprBaseListener.cpp
    ./grammars/build/Pre/PreExprBaseVisitor.cpp
    ./grammars/build/Pre/PreExprLexer.cpp
    ./grammars/build/Pre/PreExprParser.cpp
    ./grammars/build/Pre/PreExprListener.cpp
    ./grammars/build/Pre/PreExprVisitor.cpp

    ./compiler/compiler.cpp
    ./compiler/expressions.cpp
    ./compiler/asm.cpp
    ./compiler/identifier.cpp
    ./compiler/stable.cpp
    ./compiler/type.cpp
    ./compiler/target.cpp

    ./compiler/targets/corvassembly.cpp

    ./grammars/build/Corax/CoraxParser.cpp
    ./grammars/build/Corax/CoraxBaseListener.cpp
    ./grammars/build/Corax/CoraxListener.cpp
    ./grammars/build/Corax/CoraxLexer.cpp

    ./assembler/assembler.cpp
    ./assembler/operand.cpp
    ./assembler/instruction.cpp
    ./assembler/symbols.cpp

    ./grammars/build/Enca/EncaVisitor.cpp
    ./grammars/build/Enca/EncaListener.cpp
    ./grammars/build/Enca/EncaLexer.cpp
    ./grammars/build/Enca/EncaParser.cpp
    )

list (APPEND LINK_LIBS
    antlr4_static
    CLI11::CLI11
    )

# add the executable
# add_executable(Compiler main.cpp)
add_executable(corax Corax.cpp
    ${SOURCE_FILES}
    )
target_link_libraries(corax PUBLIC ${LINK_LIBS})
# add_dependencies(Cp 
#   antlr4_static 
#   Compiler
#   )
