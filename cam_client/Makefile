CC = gcc
CFLAGS = -Wall `pkg-config --cflags aravis-0.8`
LDFLAGS = -Wall `pkg-config --libs aravis-0.8`

all: sbl_cam_client

sbl_cam_client: main.o
	gcc -o $@ ${LDFLAGS} $^

%.o: src/%.c
	${CC} -c ${CFLAGS} $^

clean:
	rm -f *.o
	rm -f sbl_cam_client

.PHONY: all clean
