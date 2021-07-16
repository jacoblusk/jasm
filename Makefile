DEBUG_SETTINGS_COMPILER = /MDd -D_DEBUG /Z7
DEBUG_SETTINGS_LINKER = /DEBUG
ifndef DEBUG
DEBUG_FLAG_COMPILER = 
DEBUG_FLAG_LINKER =
else
DEBUG_FLAG_COMPILER = $(DEBUG_SETTINGS_COMPILER)
DEBUG_FLAG_LINKER = $(DEBUG_SETTINGS_LINKER)
endif

C_FLAGS := /W4 $(DEBUG_FLAG_COMPILER) /Iincludes
LINK_FLAGS := $(DEBUG_FLAG_LINKER) /subsystem:console
CC := clang-cl
LINK := lld-link

all: bin obj bin/main.exe

debug: clean set_debug all

set_debug:
	echo "Setting Debug"
	$(eval DEBUG_FLAG_COMPILER = $(DEBUG_SETTINGS_COMPILER))
	$(eval DEBUG_FLAG_LINKER = $(DEBUG_SETTINGS_LINKER))

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

bin/main.exe: obj/main.o obj/lexer.o obj/hashmap.o obj/parser.o
	$(LINK) $(LINK_FLAGS) /out:$@ $^

obj/main.o: src/main.c includes/lexer.h
	$(CC) $< /c /o $@ $(C_FLAGS)

obj/lexer.o: src/lexer.c includes/lexer.h
	$(CC) $< /c /o $@ $(C_FLAGS)

obj/parser.o: src/parser.c includes/lexer.h
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

.PHONY: all clean test set_debug
