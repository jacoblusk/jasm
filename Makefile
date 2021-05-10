ifndef DEBUG
DEBUG_FLAG :=
else
DEBUG_FLAG = /MDd -D_DEBUG
endif

C_FLAGS := /W4 /Z7 $(DEBUG_FLAG)
LINK_FLAGS := /DEBUG /subsystem:console
CC := clang-cl
LINK := lld-link

all: main.exe

test: hashmap_test.exe
	hashmap_test.exe

main.exe: main.o lexer.o
	$(LINK) $(LINK_FLAGS) /out:$@ $^

main.o: main.c lexer.h
	$(CC) $< /c /o $@ $(C_FLAGS)

lexer.o: lexer.c lexer.h
	$(CC) $< /c /o $@ $(C_FLAGS)

hashmap.o: hashmap.c hashmap.h
	$(CC) $< /c /o $@ $(C_FLAGS)

hashmap_test.o: hashmap_test.c hashmap.c hashmap.h
	$(CC) $< /c /o $@ $(C_FLAGS)

hashmap_test.exe: hashmap_test.o hashmap.o test.o
	$(LINK) $(LINK_FLAGS) /out:$@ $^

test.o: test.c test.h
	$(CC) $< /c /o $@ $(C_FLAGS)

clean:
	rm -rf *.o *.exe *.exp *.lib *.pdb

.PHONY: all clean
