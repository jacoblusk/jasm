all: clean format jasm.exe

jasm.exe: lexer.o main.o x86.o parser.o
	clang $^ -o $@

lexer.o: lexer.c lexer.h
	clang -c $< -o $@ $(CFLAGS)

x86.o: x86.c x86.h
	clang -c $< -o $@ $(CFLAGS)

parser.o: parser.c parser.h
	clang -c $< -o $@ $(CFLAGS)

main.o: main.c lexer.c lexer.h x86.c x86.h
	clang -c $< -o $@ $(CFLAGS)

format: main.c lexer.c lexer.h x86.c x86.h common.h parser.c parser.h
	clang-format -i --style="{BasedOnStyle: LLVM, IndentWidth: 4, AlignConsecutiveAssignments: true}" $^

clean:
	rm -rf *.o *.exe

.PHONY: all clean format