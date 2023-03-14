#include <stdio.h>
#include <arv.h>

#include "data.h"

extern int done ;

gboolean
periodic_task_cb( void* abstract_data )
{
    data_t *data = abstract_data ;

	// Detect if done is true
	
	if (done) {
		g_main_loop_quit( data->main_loop ) ;
		return FALSE ;
	}

	// Display FPS

	printf ("%3d frame%s - %7.3g MiB/s",
		data->buffer_count,
		data->buffer_count > 1 ? "s/s" : "/s ",
		(double) data->transferred / 1e6);

	if (data->error_count > 0)
		printf (" - %d error%s\n", data->error_count, data->error_count > 1 ? "s" : "");
	else
		printf ("\n");

	data->buffer_count = 0;
	data->error_count = 0;
	data->transferred = 0;


    return TRUE ;
}

