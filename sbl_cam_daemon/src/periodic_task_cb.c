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

    return TRUE ;
}

