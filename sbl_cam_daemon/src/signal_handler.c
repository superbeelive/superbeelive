#include<stdio.h>

extern int done ;

void
signal_handler (int signal) {

    fprintf(stderr,"\n\nReceived SIGINT\n") ;
    done = 1 ;

}
