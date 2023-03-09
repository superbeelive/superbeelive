#include <stdio.h>
#include <arv.h>

#define OPTION_REALTIME 		FALSE
#define OPTION_HIGH_PRIORITY	FALSE

void
stream_cb (void *user_data, ArvStreamCallbackType type, ArvBuffer *buffer)
{
	if (type == ARV_STREAM_CALLBACK_TYPE_INIT) {
	#if OPTION_REALTIME	
		printf("Thread priority: Realtime\n");
		if (!arv_make_thread_realtime (10)) {
			fprintf (stderr,"Failed to make stream thread realtime\n");
			exit( EXIT_FAILURE);
		}
	#elif OPTION_HIGH_PRIORITY
		printf("Thread priority: High\n");
		if (!arv_make_thread_high_priority (-10)) {
			fprintf (stderr,"Failed to make stream thread high priority\n");
			exit(EXIT_FAILURE);
		}
	#endif
	}
}
