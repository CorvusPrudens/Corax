ANTLR_HOME = src/antlr4-jar
ANTLR_JAR = $(wildcard $(ANTLR_HOME)/*.jar)

ANTLR = java -jar ${ANTLR_JAR}
ANTLR_REL = java -jar ../../${ANTLR_JAR}
GRUN = java org.antlr.v4.gui.TestRig

SRC  = ./src
ANTLR4 = antlr4
GRAMMARS = grammars
POST = Corax
PRE = Pre
JAVA_PATH = build
BUILD_PATH = ${SRC}/build

ASSEMBLER = ./assembler/assembler.py
EXAMPLE  = ./examples/test.cx

all: build_compiler

update: update_compiler build_compiler

test: post_java_build post_java_test

test_pre: pre_java_build pre_java_test

test_expr_pre: pre_expr_java_build pre_expr_java_test

test_expr_post: post_expr_java_build post_expr_java_test

.PHONY: all test clean build_cpp build_java parse_python test_java build_compiler update grammar

grammar: ${SRC}/${GRAMMARS}/${POST}.g4 ${SRC}/${GRAMMARS}/${PRE}.g4 clean
	$(info Building main compiler...)
	@ cd ${SRC}/${GRAMMARS}; ${ANTLR_REL} -visitor -Dlanguage=Cpp -o build/${POST} ${POST}.g4;
	$(info Building pre-compiler...)
	@ cd ${SRC}/${GRAMMARS}; ${ANTLR_REL} -visitor -Dlanguage=Cpp -o build/${PRE} ${PRE}.g4;
	@ cd ${SRC}/${GRAMMARS}; ${ANTLR_REL} -visitor -Dlanguage=Cpp -o build/${PRE} ${PRE}Expr.g4;
	$(info Done.)

update_and_build: update_compiler build_compiler
	
build_compiler: ${SRC}/${GRAMMARS}/build/${POST} ${SRC}/${GRAMMARS}/build/${PRE}
	@ if [ ! -d "${SRC}/build" ]; then $(MAKE) update_compiler; fi
	cd ${SRC}/build; make;

update_compiler: 
	@ if [ ! -d "${SRC}/build" ]; then mkdir ${SRC}/build; fi
	cd ${SRC}; cmake . -B build;

update_compiler_debug: 
	@ if [ ! -d "${SRC}/build" ]; then mkdir ${SRC}/build; fi
	cd ${SRC}; cmake . -B build -DCMAKE_BUILD_TYPE=Debug;

parse_python: ${ASSEMBLER} ${EXAMPLE}
	python3 test.py

test_python:
	cd tests; py.test;
	
post_java_build: ${SRC}/${GRAMMARS}/${POST}.g4
	$(info Building main compiler parse tree...)
	@ cd ${SRC}/${GRAMMARS}; ${ANTLR_REL} -o ${JAVA_PATH}/${POST}_java ${POST}.g4; \
	javac -d ${JAVA_PATH}/${POST}_java ${JAVA_PATH}/${POST}_java/${POST}*.java

post_java_test: ${SRC}/${EXAMPLE}
	$(info Executing main compiler parse tree...)
	@ cd ${SRC}/${GRAMMARS}/${JAVA_PATH}/${POST}_java; \
	${GRUN} ${POST} parse ../../../examples/preprocessed.cx -gui

pre_java_build: ${SRC}/${GRAMMARS}/${PRE}.g4
	$(info Building pre-compiler parser...)
	@ cd ${SRC}/${GRAMMARS}; ${ANTLR} -o ${JAVA_PATH}/${PRE}_java ${PRE}.g4; \
	cd ${JAVA_PATH}/${PRE}_java; javac ${PRE}*.java

pre_java_test: ${SRC}/${EXAMPLE}
	$(info Executing pre-compiler parse tree...)
	@ cd ${SRC}/${GRAMMARS}/${JAVA_PATH}/${PRE}_java; \
	${GRUN} ${PRE} parse ../../.${EXAMPLE} -gui

pre_expr_java_build: ${SRC}/${GRAMMARS}/${PRE}Expr.g4
	$(info Building pre-compiler expression parser...)
	@ cd ${SRC}/${GRAMMARS}; ${ANTLR} -o ${JAVA_PATH}/${PRE}Expr_java ${PRE}Expr.g4; \
	cd ${JAVA_PATH}/${PRE}Expr_java; javac ${PRE}Expr*.java

pre_expr_java_test: ${SRC}/${EXAMPLE}
	$(info Executing pre-compiler expression parse tree...)
	@ cd ${SRC}/${GRAMMARS}/${JAVA_PATH}/${PRE}Expr_java; \
	${GRUN} ${PRE}Expr statement ../../../examples/expr.txt -gui

post_expr_java_build: ${SRC}/${GRAMMARS}/CoraxExpr.g4
	$(info Building pre-compiler expression parser...)
	@ cd ${SRC}/${GRAMMARS}; ${ANTLR} -o ${JAVA_PATH}/CoraxExpr_java CoraxExpr.g4; \
	cd ${JAVA_PATH}/CoraxExpr_java; javac CoraxExpr*.java

post_expr_java_test: ${SRC}/${EXAMPLE}
	$(info Executing pre-compiler expression parse tree...)
	@ cd ${SRC}/${GRAMMARS}/${JAVA_PATH}/CoraxExpr_java; \
	${GRUN} CoraxExpr expression ../../../examples/pexpr.txt -gui

clean:
	@ if [ -d "${SRC}/${GRAMMARS}/build" ]; then rm -r ${SRC}/${GRAMMARS}/build; fi

reset: clean
	@ if [ -d "${SRC}/build" ]; then rm -rf ${SRC}/build; fi
	@ if [ -d "${SRC}/dist" ]; then rm -rf ${SRC}/dist; fi

run: ${SRC}/build/corax
	${SRC}/build/corax ${SRC}/examples/test.cx

graph: ${SRC}/build/corax
	${SRC}/build/corax ${SRC}/examples/test.cx --graph | dot -Tpng -Gdpi=300 -o graph.png