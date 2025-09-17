#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/time.h>
#include <arv.h>

#include "sbl.h"
#include "data.h"

void stream_cb (void*, ArvStreamCallbackType, ArvBuffer*) ;
void new_buffer_cb(ArvStream*, data_t *) ;
gboolean periodic_task_cb (void *) ;
void control_lost_cb (ArvGvDevice *) ;
void signal_handler (int) ;

int done = 0 ;

int main( int argc, char** argv, char** envv ) {

	int rows = 1200;			// image height
	int cols = 1920;			// image width
	double fps = 30 ;			// frames per sec
	char *output_prefix = NULL; // output prefix
	char *output = NULL ;      	// output path
	char *camera_name = NULL ;  // camera name
	int	buffer_size = 30 ;	    // Buffer size ( in images )
	int duration = 1 ;			// Duration in minutes
	unsigned int id_module = 0 ; // Module id
	unsigned int id_cam = 0 ;	 // Cam id

	data_t data ;

	int module_flag = 0 ;		
	int cam_flag = 0 ;

	int i ;
	int opt ;
	ArvCamera *camera ;
	GError *error = NULL ;
	gint sensor_width, sensor_height ;
	gint region_x, region_y, region_width, region_height ;
	ArvStream *stream ;
	size_t payload ;

	void (*old_sigint_handler)(int);

	// Parsing des options
	
	while((opt = getopt(argc,argv, "d:w:h:f:o:m:c:")) != -1) {
		switch (opt) {
			case 'd':
				duration=atoi(optarg);
				break;
			case 'w':
				cols=atoi(optarg);
				break;
			case 'h':
				rows=atoi(optarg);
				break;
			case 'f':
				fps=atof(optarg);
				break;
			case 'o':
				if ( output_prefix == NULL ) {
					output_prefix = malloc( strlen(optarg)+1 ) ;
					strcpy( output_prefix, optarg );
				} else {
					fprintf(stderr,"WARNING: only the first -o option is considered\n");
				}
				break;
			case 'm':
				id_module=atoi(optarg);
				module_flag = 1 ;
				break;
			case 'c':
				id_cam=atoi(optarg);
				cam_flag = 1 ;
				break;
			default: /* '?' */
				fprintf(stderr,"Usage: %s [-d duration] [-w cols] [-h rows] [-f framerate] [-o output_directory] -m module_id -c cam_id camera\n", argv[0] ) ;
				exit(EXIT_FAILURE) ;
		}
	}

	if ( optind < argc ) {
		camera_name = malloc(strlen(argv[optind])+1);
		strcpy( camera_name, argv[optind] );
	}

	if ( output_prefix == NULL ) {
		output_prefix = malloc( strlen( DEFAULT_DATA_PATH )+1 ) ;
		sprintf( output_prefix, DEFAULT_DATA_PATH ) ;
	}
	
	if ( camera_name == NULL ) {
		fprintf(stderr, "\nNo camera given.\n");
		exit(EXIT_FAILURE) ;
	}

	if (!(module_flag && cam_flag)) {
		fprintf(stderr,"\nOptions -m and -c are mandatory\n");
		exit(EXIT_FAILURE) ;
	}

	// Construction de la chaine output contenant le chemin complet

	output = malloc(strlen(output_prefix)+strlen(SBL_VIDEO_PATH));
	sprintf( output,"%s%s", output_prefix, SBL_VIDEO_PATH ) ;

	// Affichages des parametres
	
	printf("Aravis version: %d.%d.%d\n", arv_get_major_version(), arv_get_minor_version(), arv_get_micro_version() ) ;
	printf("Resolution: %dx%d\n", cols, rows ) ;
	printf("Framerate: %.2lf\n", fps ) ;
	printf("Output directory: %s\n", output ) ;

	// Verification de l'output path et benchmarking
	
	printf("Testing write access ...\n") ;

	char* test_filename ;
	unsigned char* test_data ;
	FILE* test_file ;
	size_t test_images = 300 ;
	size_t test_payload = cols*rows ;
	size_t written = 0 ;
	struct timeval begin, end ;

	test_filename = malloc( strlen(output) + 50 ) ;
	if ( test_filename == NULL ) {
		fprintf(stderr, "ERROR: Could not allocate test filename\n");
		return EXIT_FAILURE ;
	}
	sprintf( test_filename, "%s/%s", output, "file.test" ) ;
	
	test_data = malloc( test_payload ) ;
	if ( test_data == NULL ) {
		fprintf(stderr,"ERROR: Could not allocate test data\n") ;
		return EXIT_FAILURE ;
	}
	for (i=0; i<test_payload; i++ )
		test_data[i] = (unsigned char) ( rand()%255 ) ;

	gettimeofday(&begin,0) ;
	test_file = fopen( test_filename, "wb" ) ;
	if (test_file == NULL ) {
		fprintf(stderr, "ERROR: Could not open file for writing\n" ) ;
		fprintf(stderr, "       filename: %s\n", test_filename ) ;
		return EXIT_FAILURE ;
	}

	for ( i=0; i<test_images; i++ )
		written += fwrite( test_data, test_payload, 1, test_file) ;

	fflush(test_file) ;
	fclose(test_file) ;

	gettimeofday(&end,0) ;

	long seconds = end.tv_sec - begin.tv_sec;
    long microseconds = end.tv_usec - begin.tv_usec;
    double elapsed = seconds + microseconds*1e-6;

	if ( written != test_images ) {
		fprintf(stderr, "ERROR: Mismatch\n") ;
		fprintf(stderr, "\ttest:    %ld Frames\n", test_images ) ;
		fprintf(stderr, "\tWritten: %ld Frames\n", written) ;
		fprintf(stderr, "\tPayload per frame: %ld Bytes\n", test_payload ) ;
		return EXIT_FAILURE ;
	}

	printf("\tSuccess.\n");
	printf("\tElapsed: %.3f seconds.\n", elapsed);

	if ( elapsed > ((float) test_images) / (float) fps ) {
		fprintf(stderr,"ERROR: Performance is too low \n");
		return EXIT_FAILURE;
	}

	if ( remove(test_filename) ) {
		fprintf(stderr,"ERROR: Could not delete test file\n") ;
		return EXIT_FAILURE;
	} else {
		printf("\tTest file deleted successfully.\n");
		printf("\tPerformance: %.3f fps\n\n", test_images / elapsed );
	}

	free(test_filename) ;
	free(test_data) ;

	// Connexion à la camera
	
		
	camera = arv_camera_new( camera_name, &error );
	if ( error != NULL ) {
		fprintf(stderr,"ERROR: Could not connect to camera %s\n", camera_name ) ;
		fprintf(stderr,"%s\n", error->message );
		exit(EXIT_FAILURE);
	}

	if (!ARV_IS_CAMERA(camera)) {
		fprintf(stderr,"ERROR: %s is not a valid camera\n", camera_name );
		exit(EXIT_FAILURE);
	}

	// Application des parametres à la camera
	
	printf("Camera: %s\n", camera_name );
	printf("Vendor: %s\n", arv_camera_get_vendor_name(camera, NULL) ) ;
	printf("Model: %s\n",  arv_camera_get_model_name(camera, NULL) ) ;
	printf("Serial: %s\n",  arv_camera_get_device_serial_number(camera, NULL) ) ;
	printf("Device ID: %s\n",  arv_camera_get_device_id(camera, NULL) ) ;
	arv_camera_get_sensor_size( camera, &sensor_width, &sensor_height, NULL ) ;
	printf("Sensor size: %d x %d\n", sensor_width, sensor_height ) ;

	region_width = cols ;
	region_height = rows ;
	region_x = ( sensor_width - region_width ) / 2 ;
	region_y = ( sensor_height - region_height ) / 2 ;

	arv_camera_set_region( camera, 
			region_x, region_y, region_width, region_height, &error );
	if ( error != NULL ) {
		fprintf(stderr,"ERROR: Could not set region\n") ;
		fprintf(stderr,"%s\n", error->message );
		exit(EXIT_FAILURE);
	}

	arv_camera_set_pixel_format( camera, ARV_PIXEL_FORMAT_MONO_8, &error ) ;
	if ( error != NULL ) {
		fprintf(stderr,"ERROR: Could not set pixel format\n") ;
		fprintf(stderr,"%s\n", error->message );
		exit(EXIT_FAILURE);
	}

	arv_camera_set_frame_rate( camera, (double) fps, &error ) ;
	if ( error != NULL ) {
		fprintf(stderr,"ERROR: Could not set framerate\n") ;
		fprintf(stderr,"%s\n", error->message );
		exit(EXIT_FAILURE);
	}

	arv_camera_get_region( camera, 
			&region_x, &region_y,
			&region_width, &region_height, 
			&error ) ;
	if ( error != NULL ) {
		fprintf(stderr,"ERROR: Could not get region\n") ;
		fprintf(stderr,"%s\n", error->message );
		exit(EXIT_FAILURE);
	}
	printf("Region: %d x %d (%d, %d)\n", region_width, region_height, region_x, region_y ) ;
	cols = region_width ;
	rows = region_height ;

	fps = arv_camera_get_frame_rate( camera, &error ) ;
	if ( error != NULL ) {
		fprintf(stderr,"ERROR: Could not get framerate\n") ;
		fprintf(stderr,"%s\n", error->message );
		exit(EXIT_FAILURE);
	}
	printf("Framerate: %.3lf\n\n", fps ) ; 
	
	arv_camera_set_acquisition_mode( camera, ARV_ACQUISITION_MODE_CONTINUOUS, &error ) ;
	if ( error != NULL ) {
		fprintf(stderr,"ERROR: Could not set acquisition mode\n") ;
		fprintf(stderr,"%s\n", error->message );
		exit(EXIT_FAILURE);
	}

	// creation du stream
	
	stream = arv_camera_create_stream( camera, stream_cb, NULL, NULL, &error );
	if ( error != NULL ) {
		fprintf(stderr,"ERROR: Could not create stream\n") ;
		fprintf(stderr,"%s\n", error->message );
		exit(EXIT_FAILURE);
	}
	
	if (!ARV_IS_STREAM(stream)) {
		fprintf(stderr, "ERROR: This is not a valid stream\n");
		exit(EXIT_FAILURE);
	}
	
	// Buffers allocation

	payload = arv_camera_get_payload( camera, &error ) ;
	if ( error != NULL ) {
		fprintf(stderr,"ERROR: Could not retrieve payload size\n") ;
		fprintf(stderr,"%s\n", error->message );
		exit(EXIT_FAILURE);
	}
	printf( "Payload size: %ld Bytes \n", payload ) ;
	printf( "Buffer: %d images\n\n", buffer_size ) ;
	
	for( i=0; i<buffer_size; i++) {
		ArvBuffer *buff ;
		buff = arv_buffer_new_allocate( payload ) ;
		if ( buff == NULL ) {
			fprintf(stderr,"ERROR: Could not allocate buffer\n") ;
			exit(EXIT_FAILURE);
		}
		arv_stream_push_buffer( stream, buff );
	}

	// Record loop


	data.payload = payload ;
	data.duration = duration ;

	data.buffer_count = 0;
	data.error_count = 0;
	data.transferred = 0;
	data.start_time = g_get_monotonic_time();
	data.handle = -1 ;
	if (pthread_mutex_init(&data.handle_mutex, NULL) != 0) {
        printf("\n mutex init has failed\n");
        return 1;
    }
	sprintf( data.header.cam_serial, arv_camera_get_device_serial_number(camera, NULL) ) ;
	data.header.rows = rows ;
	data.header.cols = cols ;
	data.header.fps = fps ;
	data.header.encoding = MONO8 ;
	data.header.hive = ID_HIVE ;
	data.header.module = id_module ;
	data.header.cam = id_cam ;
	data.output = output ;

	printf("ID Hive: %d\n", data.header.hive ) ;
	printf("ID Module: %d\n", data.header.module ) ;
	printf("ID Cam: %d\n", data.header.cam ) ;
	printf("File duration: %d minutes\n\n", duration ) ;
	
	printf("Recording...\n") ;

	arv_camera_start_acquisition(camera, &error);

    g_signal_connect (stream, "new-buffer", G_CALLBACK (new_buffer_cb), &data);
	arv_stream_set_emit_signals (stream, TRUE);

	g_signal_connect (arv_camera_get_device (camera), "control-lost",
					      G_CALLBACK (control_lost_cb), NULL);

	data.start_time = g_get_monotonic_time();
	
	g_timeout_add (1000, periodic_task_cb, &data);
	data.header.timestamp = time(NULL) ;
	
	data.main_loop = g_main_loop_new (NULL, FALSE);
	old_sigint_handler = signal (SIGINT, signal_handler) ;
	g_main_loop_run (data.main_loop);
	
	signal (SIGINT, old_sigint_handler);
	g_main_loop_unref (data.main_loop);

	arv_camera_stop_acquisition( camera, &error ) ;
	arv_stream_set_emit_signals (stream, FALSE);

	
	// Liberation memoire

	g_clear_object( &stream ) ;
	g_clear_object( &camera ) ;

	free(output) ;
	free(output_prefix) ;
	free(camera_name) ;

	exit(EXIT_SUCCESS);

}

