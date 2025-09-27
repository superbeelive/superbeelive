#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

#include <sblv_file.h>

#define RAW_DIR     "/beegfs/superbeelive_data/raw_data/cam_videos"
#define OUTPUT_DIR  "/beegfs/superbeelive_data/web_server/videos"

int main( int argc, char** argv, char** envv ) {
	
	int module ;
	int camera ;
	int start_year ;
	int start_month ;
	int start_day ;
	int start_hour ;
	int start_minute ;
	int stop_year ;
	int stop_month ;
	int stop_day ;
	int stop_hour ;
	int stop_minute ;

	if (argc != 7) {
		fprintf(stderr, "Missing argument.\n\n") ;
		fprintf(stderr, "Syntax: %s module camera start_YYYY-MM-DD start_HH:MM stop-YYY-MM-DD stop_HH:MM\n", argv[0] );
		return EXIT_FAILURE ;
	}

	module = atoi(argv[1]);
	if (!module) {
		fprintf(stderr, "Error reading arg : module.\n");
		return EXIT_FAILURE ;
	}

	camera = atoi(argv[2]);
	if (!camera) {
		fprintf(stderr, "Error reading arg : camera.\n");
		return EXIT_FAILURE ;
	}

	if (sscanf(argv[3],"%d-%d-%d", &start_year,&start_month,&start_day) != 3 ) {
		fprintf(stderr,"Error reading arg : start_day\n");
		return EXIT_FAILURE;
	}

	if (sscanf(argv[4],"%d:%d", &start_hour,&start_minute) != 2 ) {
		fprintf(stderr,"Error reading arg : start_time\n");
		return EXIT_FAILURE;
	}

	if (sscanf(argv[5],"%d-%d-%d", &stop_year,&stop_month,&stop_day) != 3 ) {
		fprintf(stderr,"Error reading arg : stop_day\n");
		return EXIT_FAILURE;
	}

	if (sscanf(argv[6],"%d:%d", &stop_hour,&stop_minute) != 2 ) {
		fprintf(stderr,"Error reading arg : stop_time\n");
		return EXIT_FAILURE;
	}

	printf("Module: %02d\n", module);
	printf("Camera: %02d\n", camera);
	printf("Start time : %4d-%02d-%02d %02d:%02d\n",
			start_year, start_month, start_day,
			start_hour, start_minute );
	printf("Stop time :  %4d-%02d-%02d %02d:%02d\n",
			stop_year, stop_month, stop_day,
			stop_hour, stop_minute );

	time_t rawtime ;
	time(&rawtime) ;
	struct tm start = {0} ;
	localtime_r(&rawtime,  &start) ;
	struct tm stop = {0} ;
	localtime_r(&rawtime,  &stop) ;

	start.tm_min = start_minute ;
	start.tm_hour = start_hour ;
	start.tm_mon = start_month-1 ;
	start.tm_year = start_year-1900 ;
	start.tm_mday = start_day ;
	start.tm_sec = 0 ;
	
	stop.tm_min = stop_minute ;
	stop.tm_hour = stop_hour ;
	stop.tm_mon = stop_month-1 ;
	stop.tm_year = stop_year-1900 ;
	stop.tm_mday = stop_day ;
	stop.tm_sec = 0 ;

	time_t start_ts = mktime(&start) ;
	time_t stop_ts = mktime(&stop) ;
	time_t current_ts = start_ts ;

	while ( current_ts <= stop_ts ) {
		struct tm *current_tm = localtime(&current_ts) ;
		
		char buffer[1024] = {0} ;
		strftime(buffer, sizeof(buffer), "%Y%m%d_%H%M", current_tm) ;

		char buffer2[1024] ;
		snprintf(buffer2,1024,"M%02dC%02d_%s", module, camera, buffer) ;
		
		printf("%s\n", buffer2) ;

		current_ts += 60 ;
	}

	printf("<a href=\"http://www.perdu.com\"> test </a>\n");

	return EXIT_SUCCESS ;
}

