CC = gcc
CFLAGS = -O2 -Wall `pkg-config --cflags aravis-0.10` -I../libsbl/include
LDFLAGS = -Wall `pkg-config --libs aravis-0.10`
DEBUG = -g

all: sbl_cam_daemon

sbl_cam_daemon: main.o stream_cb.o new_buffer_cb.o periodic_task_cb.o signal_handler.o control_lost_cb.o
	gcc -o $@ $^ ${LDFLAGS}

%.o: src/%.c
	${CC} -c ${CFLAGS} $^

clean:
	rm -f *.o
	rm -f sbl_cam_daemon

.PHONY: all clean
