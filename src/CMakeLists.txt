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
    ${CMAKE_SOURCE_DIR}/compiler
    ${CMAKE_SOURCE_DIR}/compiler/targets
    ${CMAKE_SOURCE_DIR}/precompiler
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
    )

set (INCLUDE_FILES
    antlr4/runtime/Cpp/runtime/src/antlr4-runtime.h

    colors.h
    error.h
    utils.h

    precompiler/precompiler.h
    compiler/compiler.h

    ./precompiler/sar.h

    ./grammars/build/Pre/PreBaseListener.h
    ./grammars/build/Pre/PreBaseVisitor.h
    ./grammars/build/Pre/PreParser.h
    ./grammars/build/Pre/PreListener.h
    ./grammars/build/Pre/PreVisitor.h
    ./grammars/build/Pre/PreLexer.h

    ./grammars/build/Pre/PreExprBaseListener.h
    ./grammars/build/Pre/PreExprBaseVisitor.h
    ./grammars/build/Pre/PreExprLexer.h
    ./grammars/build/Pre/PreExprParser.h
    ./grammars/build/Pre/PreExprListener.h
    ./grammars/build/Pre/PreExprVisitor.h

    ./compiler/asm.h
    ./compiler/identifier.h
    ./compiler/stable.h
    ./compiler/type.h
    ./compiler/operators.h
    ./compiler/target.h

    ./compiler/targets/corvassembly.h

    ./grammars/build/Corax/CoraxBaseListener.h
    ./grammars/build/Corax/CoraxLexer.h
    ./grammars/build/Corax/CoraxParser.h
    ./grammars/build/Corax/CoraxListener.h

    )

list (APPEND LINK_LIBS
    antlr4_static
    CLI11::CLI11
    )

# add the executable
# add_executable(Compiler main.cpp)
add_executable(corax Corax.cpp
    ${SOURCE_FILES}
    ${INCLUDE_FILES}
    )
target_link_libraries(corax PUBLIC ${LINK_LIBS})
# add_dependencies(Cp 
#   antlr4_static 
#   Compiler
#   )
