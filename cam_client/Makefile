CC = gcc
CFLAGS = -Wall
LDFLAGS = -L.

all: sbl_cam_client

sbl_cam_client: main.o
	gcc -o $@ ${LDFLAGS} $^

%.o: src/%.c
	${CC} -c ${CFLAGS} $^

clean:
	rm -f *.o
	rm -f sbl_cam_client

.PHONY: all clean
