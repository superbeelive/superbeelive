CC = gcc
CFLAGS = -Wall `pkg-config --cflags aravis-0.8` -I../libsbl/include
LDFLAGS = -Wall `pkg-config --libs aravis-0.8`
DEBUG = -g

all: sbl_cam_list

sbl_cam_list: main.o
	gcc -o $@ $^ ${LDFLAGS}

%.o: src/%.c
	${CC} -c ${CFLAGS} ${DEBUG} $^

clean:
	rm -f *.o
	rm -f sbl_cam_list

.PHONY: all clean
