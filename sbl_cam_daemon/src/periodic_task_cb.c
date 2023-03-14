#include <stdio.h>
#include <arv.h>
#include <fcntl.h>

#include "data.h"

extern int done ;
char filename[100] ;

gboolean
periodic_task_cb( void* abstract_data )
{
    data_t *data = abstract_data ;
	int new_file ;
	int old_file ;
	struct tm tm ;

	// Detect if done is true
	
	if (done) {
		g_main_loop_quit( data->main_loop ) ;
		return FALSE ;
	}

	// Execute every <duration>

	if (( g_get_monotonic_time() - data->start_time ) > data->duration * 60e6 ) {
		
		time_t timestamp = time(NULL) ;

		tm = *localtime(&(timestamp));
		sprintf(filename,"%s/M%02dC%02d_%d%02d%02d_%02d%02d%02d.sblv", 
				data->output,
				data->header.module, data->header.cam,
				tm.tm_year + 1900, 
				tm.tm_mon + 1, 
				tm.tm_mday, 
				tm.tm_hour, 
				tm.tm_min, 
				tm.tm_sec);

		new_file=creat( filename, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH );

		old_file = data->handle ;
		
		pthread_mutex_lock( &data->handle_mutex) ;	
		data->handle = new_file ;
		pthread_mutex_unlock( &data->handle_mutex) ;	
		
		data->header.timestamp = timestamp ;
		data->start_time = g_get_monotonic_time();
		
		if ( old_file != -1 ) {
			sleep(1) ;
			write( old_file, &data->header, sizeof(sblv_header) ) ;
			close(old_file) ;
		}

	}


	// Display FPS

	printf ("%3d frame%s - %3.3f GB/s",
		data->buffer_count,	
		(data->buffer_count > 1)?"s/s":"/s ",
		(double) data->transferred *8 / 1e9);

	if (data->error_count > 0)
		printf (" - %d error%s\n", data->error_count, data->error_count > 1 ? "s" : "");
	else
		printf ("\n");

	data->buffer_count = 0;
	data->error_count = 0;
	data->transferred = 0;


    return TRUE ;
}

