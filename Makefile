TEST_SRCS = $(wildcard *_test.c)
TEST_EXECS = $(TEST_SRCS:.c=)

all: $(TEST_EXECS)

clear:
	rm -rf *.o *.a *_test

check_fmt:
	clang-format -style=LLVM -i `find -regex ".+\.[ch]"` --dry-run --Werror

fmt:
	clang-format -style=LLVM -i `find -regex ".+\.[ch]"`

# --- %

%.o: %.c %.h
	gcc -g -c $< -o $@

%.a: %.o
	ar rc $@ $<

%_test.o: %_test.c
	gcc -g -c $< -o$@

%_test: %_test.o %.a
	gcc -g -static -o $@ $^ -lm

# ---

test: $(TEST_EXECS)
	@for t in $(TEST_EXECS); do \
		./$$t; \
	done
