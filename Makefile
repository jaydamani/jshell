BUILD := release
LDFLAGS='-lreadline'
CFLAGS := -Wall
RELEASES := release debug
CFLAGS_debug := -g -Werror -fsanitize=address  -g
CFLAGS_release :=
SUB_BUILDS = parser/parser.o
CC=gcc

.PHONY: clean $(RELEASES)
.PRECIOUS: build/debug/%.o build/release/%.o

$(RELEASES): %: build/%/shell

build/%/shell: src/main.c build/%/$(SUB_BUILDS)
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(LDFLAGS) $(CFLAGS_$*) $^ -o $@

build/debug/%.o: src/%.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(CFLAGS_debug) -c $^ -o $@

build/release/%.o: src/%.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(CFLAGS_release) -c $^ -o $@

clean:
	rm -r build
