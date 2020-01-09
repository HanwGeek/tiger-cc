INCLUDE_DIR = ./include
SRC_DIR = ./src
OBJ_DIR = ./obj
BIN_DIR = ./bin
TEST_DIR = ./test

CC = gcc
CFLAGS = -I$(INCLUDE_DIR) -Wall -g -c -o 

OBJECTS = util.o errormsg.o table.o symbol.o absyn.o env.o translate.o semant.o   \
					types.o frame.o temp.o y.tab.o lex.yy.o
OBJS = $(patsubst %, $(OBJ_DIR)/%, $(OBJECTS))

PARSE_TEST = $(BIN_DIR)/parsetest

all: $(OBJS)
	@echo "Build complete!"

parsetest: $(OBJS) $(OBJ_DIR)/prabsyn.o $(OBJ_DIR)/parsetest.o
	$(CC) $^ -o $(BIN_DIR)/parsetest

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

$(OBJ_DIR)/parsetest.o: $(TEST_DIR)/parse.c
	$(CC) $(CFLAGS) $@ $<

$(OBJ_DIR)/prabsyn.o: $(TEST_DIR)/prabsyn.c 
	$(CC) $(CFLAGS) $@ $<

clean:
	rm -f $(OBJ_DIR)/*.o $(SRC_DIR)/lex.yy.c $(SRC_DIR)/y.tab.c $(SRC_DIR)/y.tab.h \
				$(SRC_DIR)/y.output