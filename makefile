INCLUDE_DIR = ./include
SRC_DIR = ./src
OBJ_DIR = ./obj
BIN_DIR = ./build
TEST_DIR = ./test

CC = gcc
CFLAGS = -I$(INCLUDE_DIR) -Wall -g -c -o

OBJECTS = util.o errormsg.o table.o symbol.o absyn.o env.o translate.o semant.o   \
					types.o frame.o temp.o tree.o canon.o assem.o codegen.o graph.o escape.o \
					flowgraph.o liveness.o color.o regalloc.o y.tab.o lex.yy.o
PARSETEST_OBJECTS = prabsyn.o parse.o parsetest.o
TREETEST_OBJECTS  = printtree.o prabsyn.o parse.o treetest.o
CODEGENTEST_OBJECTS = printtree.o prabsyn.o parse.o parse.o codegentest.o

OBJS = $(patsubst %, $(OBJ_DIR)/%, $(OBJECTS))
PARSETEST_OBJS = $(patsubst %, $(OBJ_DIR)/%, $(PARSETEST_OBJECTS))
TREETEST_OBJS = $(patsubst %, $(OBJ_DIR)/%, $(TREETEST_OBJECTS))
CODEGENTEST_OBJS = $(patsubst %, $(OBJ_DIR)/%, $(CODEGENTEST_OBJECTS))

all: parsetest treetest codegentest
	@echo "All build complete!"

parsetest: $(OBJS) $(PARSETEST_OBJS) 
	$(CC) $^ -g -o $(BIN_DIR)/parsetest
	@echo "Parsetest build complete!"

treetest: $(OBJS) $(TREETEST_OBJS)
	$(CC) $^ -g -o $(BIN_DIR)/treetest
	@echo "Treetest build complete!"

codegentest: $(OBJS) $(CODEGENTEST_OBJS)
	$(CC) $^ -g -o $(BIN_DIR)/codegentest
	@echo "Codegentest build complete!"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c 
	$(CC) $(CFLAGS) $@ $<

$(OBJ_DIR)/frame.o: $(SRC_DIR)/x86frame.c 
	$(CC) $(CFLAGS) $@ $<

$(OBJ_DIR)/codegen.o: $(SRC_DIR)/x86codegen.c 
	$(CC) $(CFLAGS) $@ $<

$(OBJ_DIR)/lex.yy.o: $(SRC_DIR)/lex.yy.c
	$(CC) $(CFLAGS) $@ $< 

$(OBJ_DIR)/y.tab.o: $(SRC_DIR)/y.tab.c 
	$(CC) $(CFLAGS) $@ $<

$(SRC_DIR)/lex.yy.c: $(SRC_DIR)/tiger.l 
	flex -o $@ $<

$(SRC_DIR)/y.tab.c: $(SRC_DIR)/tiger.y 
	yacc --debug -ydvo $@ $<

$(PARSETEST_OBJS):$(OBJ_DIR)/%.o: $(TEST_DIR)/%.c
	$(CC) $(CFLAGS) $@ $<

$(OBJ_DIR)/parsetest.o: $(TEST_DIR)/parsetest.c
	$(CC) $(CFLAGS) $@ $<

$(TREETEST_OBJS):$(OBJ_DIR)/%.o: $(TEST_DIR)/%.c
	$(CC) $(CFLAGS) $@ $<

$(OBJ_DIR)/treetest.o: $(TEST_DIR)/treetest.c
	$(CC) $(CFLAGS) $@ $<

$(CODEGEN_OBJS):$(OBJ_DIR)/%.o: $(TEST_DIR)/%.c
	$(CC) $(CFLAGS) $@ $<

$(OBJ_DIR)/codegentest.o: $(TEST_DIR)/codegentest.c
	$(CC) $(CFLAGS) $@ $<

clean:
	rm -f $(OBJ_DIR)/*.o $(SRC_DIR)/lex.yy.c $(SRC_DIR)/y.tab.c $(SRC_DIR)/y.tab.h \
				$(SRC_DIR)/y.output