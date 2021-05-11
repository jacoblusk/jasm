DEBUG_SETTINGS = /MDd -D_DEBUG

ifndef DEBUG
DEBUG_FLAG = 
else
DEBUG_FLAG = $(DEBUG_SETTINGS)
endif

C_FLAGS := /W4 /Z7 $(DEBUG_FLAG) /Iincludes
LINK_FLAGS := /DEBUG /subsystem:console
CC := clang-cl
LINK := lld-link

all: bin obj bin/main.exe

debug: clean set_debug all

set_debug:
	$(eval DEBUG_FLAG = $(DEBUG_SETTINGS))

bin:
	mkdir bin

obj:
	mkdir obj

tests/bin:
	mkdir tests/bin

tests/obj:
	mkdir tests/obj

test: clean set_debug obj tests/bin tests/obj tests/bin/hashmap_test.exe
	tests/bin/hashmap_test.exe

bin/main.exe: obj/main.o obj/lexer.o
	$(LINK) $(LINK_FLAGS) /out:$@ $^

obj/main.o: src/main.c includes/lexer.h
	$(CC) $< /c /o $@ $(C_FLAGS)

obj/lexer.o: src/lexer.c includes/lexer.h
	$(CC) $< /c /o $@ $(C_FLAGS)

obj/hashmap.o: src/hashmap.c includes/hashmap.h
	$(CC) $< /c /o $@ $(C_FLAGS)

tests/obj/hashmap_test.o: tests/hashmap_test.c src/hashmap.c includes/hashmap.h
	$(CC) $< /c /o $@ $(C_FLAGS)

tests/bin/hashmap_test.exe: tests/obj/hashmap_test.o obj/hashmap.o tests/obj/test.o
	$(LINK) $(LINK_FLAGS) /out:$@ $^

tests/obj/test.o: src/test.c includes/test.h
	$(CC) $< /c /o $@ $(C_FLAGS)

clean:
	rm -rf obj bin tests/obj tests/bin tests/bin

.PHONY: all clean tests set_debug
