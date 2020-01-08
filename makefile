INCLUDE_DIR = ./include
SRC_DIR = ./src
OBJ_DIR = ./obj
BIN_DIR = ./bin

CC = gcc
CFLAGS = -I$(INCLUDE_DIR) -Wall -g -c -o 

OBJECTS = util.o errormsg.o table.o symbol.o y.tab.o lex.yy.o
OBJS = $(patsubst %, $(OBJ_DIR)/%, $(OBJECTS))

all: $(OBJS)
	@echo "Build complete!"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c 
	$(CC) $(CFLAGS) $@ $<

$(OBJ_DIR)/lex.yy.o: $(SRC_DIR)/lex.yy.c
	$(CC) $(CFLAGS) $@ $< 

$(OBJ_DIR)/y.tab.o: $(SRC_DIR)/y.tab.c 
	$(CC) $(CFLAGS) $@ $<

$(SRC_DIR)/lex.yy.c: $(SRC_DIR)/tiger.l 
	flex -o $@ $<

$(SRC_DIR)/y.tab.c: $(SRC_DIR)/tiger.y 
	yacc --debug -dv $< -o $@

clean:
	rm -f $(OBJ_DIR)/*.o $(SRC_DIR)/lex.yy.c $(SRC_DIR)/y.tab.c $(SRC_DIR)/y.tab.h \
				$(SRC_DIR)/y.output