all: data task

data: data.c
	gcc -o data data.c -lpthread -g

task: task.c
	gcc -o task task.c -lpthread -g
