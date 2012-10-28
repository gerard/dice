dice: dice.c matrix.c matrix.h
	gcc -lpthread -lGLU -lglut $^ -o $@
