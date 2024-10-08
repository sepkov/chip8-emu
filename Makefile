INCLUDES= -I ./include
FLAGS= -g 

OBJECTS=./build/chip8memory.o ./build/chip8stack.o ./build/chip8keyboard.o ./build/chip8.o ./build/chip8screen.o ./build/utils.o
all: $(OBJECTS)
	gcc $(FLAGS) $(INCLUDES) ./src/main.c ${OBJECTS} -L lib -lSDL2-2.0.0 -o ./bin/main

build/utils.o:src/utils.c
	gcc ${FLAGS} ${INCLUDES} ./src/utils.c -c -o ./build/utils.o

build/chip8memory.o:src/chip8memory.c
	gcc ${FLAGS} ${INCLUDES} ./src/chip8memory.c -c -o ./build/chip8memory.o

build/chip8stack.o:src/chip8stack.c
	gcc ${FLAGS} ${INCLUDES} ./src/chip8stack.c -c -o ./build/chip8stack.o

build/chip8keyboard.o:src/chip8keyboard.c
	gcc ${FLAGS} ${INCLUDES} ./src/chip8keyboard.c -c -o ./build/chip8keyboard.o

build/chip8screen.o:src/chip8screen.c
	gcc ${FLAGS} ${INCLUDES} ./src/chip8screen.c -c -o ./build/chip8screen.o

build/chip8.o:src/chip8.c
	gcc ${FLAGS} ${INCLUDES} ./src/chip8.c -c -o ./build/chip8.o

clean:
	rm -rf build/*