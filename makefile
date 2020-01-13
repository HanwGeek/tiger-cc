INCLUDE_DIR = ./include
SRC_DIR = ./src
OBJ_DIR = ./obj
BIN_DIR = ./bin
TEST_DIR = ./test

CC = gcc
CFLAGS = -I$(INCLUDE_DIR) -Wall -g -c -o 

OBJECTS = util.o errormsg.o table.o symbol.o absyn.o env.o translate.o semant.o   \
					types.o frame.o temp.o tree.o canon.o y.tab.o lex.yy.o
PARSETEST_OBJECTS = parsetest.o prabsyn.o
TREETEST_OBJECTS  = printtree.o treetest.o 

OBJS = $(patsubst %, $(OBJ_DIR)/%, $(OBJECTS))
PARSETEST_OBJS = $(patsubst %, $(OBJ_DIR)/%, $(PARSETEST_OBJECTS))
TREETEST_OBJS = $(patsubst %, $(OBJ_DIR)/%, $(TREETEST_OBJECTS))

PARSE_TEST = $(BIN_DIR)/parsetest

all: $(OBJS) parsetest treetest
	@echo "Build complete!"

parsetest: $(OBJS) $(PARSETEST_OBJS)
	$(CC) $^ -o $(BIN_DIR)/parsetest
	@echo "Parsetest build complete!"

treetest: $(OBJS) $(TREETEST_OBJS) 
	$(CC) $^ -o (BIN_DIR)/treetest
	@echo "Treetest build complete!"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c 
	$(CC) $(CFLAGS) $@ $<

$(OBJ_DIR)/frame.o: $(SRC_DIR)/x86frame.c 
	$(CC) $(CFLAGS) $@ $<

$(OBJ_DIR)/lex.yy.o: $(SRC_DIR)/lex.yy.c
	$(CC) $(CFLAGS) $@ $< 

$(OBJ_DIR)/y.tab.o: $(SRC_DIR)/y.tab.c 
	$(CC) $(CFLAGS) $@ $<

$(SRC_DIR)/lex.yy.c: $(SRC_DIR)/tiger.l 
	flex -o $@ $<

$(SRC_DIR)/y.tab.c: $(SRC_DIR)/tiger.y 
	yacc --debug -ydvo $@ $<

$(OBJ_DIR)/prabsyn.o: $(TEST_DIR)/prabsyn.c 
	$(CC) $(CFLAGS) $@ $<

$(OBJ_DIR)/parsetest.o: $(TEST_DIR)/parse.c
	$(CC) $(CFLAGS) $@ $<

$(OBJ_DIR)/printtree.o: $(TEST_DIR)/printtree.c
	$(CC) $(CFLAGS) $@ @<

clean:
	rm -f $(OBJ_DIR)/*.o $(SRC_DIR)/lex.yy.c $(SRC_DIR)/y.tab.c $(SRC_DIR)/y.tab.h \
				$(SRC_DIR)/y.output