CC = gcc
CFLAGS = -Wall `pkg-config --cflags aravis-0.10` -I../../libsbl/include
LDFLAGS = -Wall `pkg-config --libs aravis-0.10` -lpng
DEBUG = -g

all: sbl_cam_grab_frame

sbl_cam_grab_frame: main.o
	gcc -o $@ $^ ${LDFLAGS}

%.o: src/%.c
	${CC} -c ${CFLAGS} ${DEBUG} $^

clean:
	rm -f *.o
	rm -f sbl_cam_list

.PHONY: all clean
