all:main.o robot.o
	gcc main.o robot.o -o Monitoring

main.o: main.c robot.h
	gcc -c main.c -o main.o

robot.o: robot.c
	 gcc -c robot.c -o robot.o
