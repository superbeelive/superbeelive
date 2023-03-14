#ifndef DATA_H
#define DATA_H

#include <pthread.h>
#include "sblv_file.h"

typedef struct {
	GMainLoop *main_loop;
	
	// Variables pour comptage de FPS, Erreurs, etc.
	// Update toutes les secondes
	
	int buffer_count;		
	int error_count;
	size_t transferred;
    
	// Variables acquisition

	gint64 start_time;
	sblv_header header ;
	int duration ;
	size_t payload ;
	char* output ;

	int	handle ;
	pthread_mutex_t handle_mutex ;


} data_t ;








#endif

