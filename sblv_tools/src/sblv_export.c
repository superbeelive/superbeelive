#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

#include <sblv_file.h>

int main( int argc, char** argv, char** envv ) {
	
	int i ;
	FILE* fhandle ;
	sblv_header header ;
	int pos ;

	if (argc < 2) {
		fprintf(stderr, "Missing argument.\n\n") ;
		fprintf(stderr, "Syntax: %s module camera start_YYYY_MM_DD start_HH:MM stop_YYY_MMM_DD stop_HH:MM\n", argv[0] );
		return EXIT_FAILURE ;
	}

	printf("Module: %s\n", argv[1]);

	return EXIT_SUCCESS ;
}

