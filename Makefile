CFLAGS :=-Wpedantic -Wall -Wextra -Wdouble-promotion
INCLUDES := -I src/include/
LIBS := -lncursesw -lm

SRCS := $(wildcard src/*.c)
OBJS := $(SRCS:src/%.c=build/%.o)
BIN := build/orb

$(BIN): $(SRCS)
	mkdir -p build
	gcc $(SRCS) $(INCLUDES) $(CFLAGS) $(LIBS) -o $(BIN)

#We can't pass args into the make invocation, so this defaults to 1 for dev
run: $(BIN)
	./$(BIN) "00:00:01"

clean:
	rm -rf build/*

.PHONY: all clean run test
