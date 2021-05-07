C_FLAGS := -O1 -g -fsanitize=address -fno-omit-frame-pointer -pedantic -Wextra -Wall
CC := clang

all: main.exe

main.exe: main.o lexer.o
	$(CC) $? -o $@ $(C_FLAGS)

main.o: main.c lexer.h
	$(CC) $^ -c $(C_FLAGS)

lexer.o: lexer.c lexer.h
	$(CC) $^ -c $(C_FLAGS)

clean:
	rm -rf *.o *.exe *.exp *.lib *.pdb

.PHONY: all clean
