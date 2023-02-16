INCLUDES= -I ./include
FLAGS= -g

OBJECTS=./build/Chip7_memory.o ./build/Chip7_stack.o ./build/Chip7_keyboard.o ./build/Chip7.o ./build/Chip7_screen.o

all: ${OBJECTS}
	gcc ${FLAGS} ${INCLUDES} ./src/main.c ${OBJECTS} -L ./lib -lmingw32 -lSDL2main -lSDL2 -o ./bin/main

./build/Chip7_memory.o:src/Chip7_memory.c
	gcc ${FLAGS} ${INCLUDES} ./src/Chip7_memory.c -c -o ./build/Chip7_memory.o

./build/Chip7_stack.o:src/Chip7_stack.c
	gcc ${FLAGS} ${INCLUDES} ./src/Chip7_stack.c -c -o ./build/Chip7_stack.o

./build/Chip7_keyboard.o:src/Chip7_keyboard.c
	gcc ${FLAGS} ${INCLUDES} ./src/Chip7_keyboard.c -c -o ./build/Chip7_keyboard.o

./build/Chip7.o:src/Chip7.c
	gcc ${FLAGS} ${INCLUDES} ./src/Chip7.c -c -o ./build/Chip7.o

./build/Chip7_screen.o:src/Chip7_screen.c
	gcc ${FLAGS} ${INCLUDES} ./src/Chip7_screen.c -c -o ./build/Chip7_screen.o


clean:
	del build\*

run:
	bin/main