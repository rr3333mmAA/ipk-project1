CC = gcc

ipkcpc: ipkcpc.c
	$(CC) ipkcpc.c -o ipkcpc 

clean:
	rm -f ipkcpc *.o *~
