#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <arv.h>

#include "sblv_file.h"

#define DEFAULT_DATA_PATH	"/home/superbeelive/workspace"
#define RAW_VIDEO_PATH		"/raw_data/cam_videos"

int main( int argc, char** argv, char** envv ) {

	int rows = 1200;	// image height
	int cols = 1920;	// image width
	double fps = 30 ;	// frames per sec
	char *output_prefix = NULL; // output prefix
	char *output = NULL ;      	// output path
	char *camera_name = NULL ;  // camera name
	int	buffer_size = 300 ;	// Buffer size ( in images )

	int i ;
	int opt ;
	ArvCamera *camera ;
	GError *error = NULL ;
	gint sensor_width, sensor_height ;
	gint region_x, region_y, region_width, region_height ;
	ArvStream *stream ;
	size_t payload ;

	// Parsing des options
	
	while((opt = getopt(argc,argv, "c:r:f:o:h")) != -1) {
		switch (opt) {
			case 'c':
				cols=atoi(optarg);
				break;
			case 'r':
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
			case 'h':
			default:
				fprintf(stderr,"Usage: %s [-c cols] [-r rows] [-f framerate] [-o output_directory] camera\n", argv[0] ) ;
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
	
	// Construction de la chaine output contenant le chemin complet

	output = malloc(strlen(output_prefix)+strlen(RAW_VIDEO_PATH));
	sprintf( output,"%s%s", output_prefix, RAW_VIDEO_PATH ) ;

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
		printf("ERROR: Could not delete test file\n") ;
		return EXIT_FAILURE;
	} else {
		printf("\tTest file deleted successfully.\n");
		printf("\tPerformance: %.3f fps\n", test_images / elapsed );
	}

	free(test_filename) ;
	free(test_data) ;

	// Connexion à la camera
	
	if ( camera_name == NULL ) {
		printf("\nNo camera given.\n");
		printf("Available cams: \n");
		ArvInterface* interface ;
		interface = arv_gv_interface_get_instance() ;
		arv_interface_update_device_list( interface ) ;
		int nb = arv_interface_get_n_devices( interface ) ;
		if ( nb == 0 )
			printf("\t NO CAMERA DETECTED\n") ;
		else {
			for ( i=0; i <nb; i++ ) {
				printf("\t - %s\n", arv_get_device_id(i) ) ;
				printf("\t\t Model: %s\n", arv_get_device_model(i) ) ;
				printf("\t\t Physical id: %s\n", arv_get_device_physical_id(i) ) ;
				printf("\t\t IP: %s\n", arv_get_device_address(i) ) ;
				printf("\t\t Serial: %s\n", arv_get_device_serial_nbr(i) ) ;
			}
		}
		exit(EXIT_FAILURE) ;
	}
		
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
	
	printf("\nCamera: %s\n", camera_name );
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
	printf("Framerate: %.3lf\n", fps ) ; 
	
	arv_camera_set_acquisition_mode( camera, ARV_ACQUISITION_MODE_CONTINUOUS, &error ) ;
	if ( error != NULL ) {
		fprintf(stderr,"ERROR: Could not set acquisition mode\n") ;
		fprintf(stderr,"%s\n", error->message );
		exit(EXIT_FAILURE);
	}

	// creation du stream
	
	stream = arv_camera_create_stream( camera, NULL, NULL, &error );
	if ( error != NULL ) {
		fprintf(stderr,"ERROR: Could not create stream\n") ;
		fprintf(stderr,"%s\n", error->message );
		exit(EXIT_FAILURE);
	}
	
	if (!ARV_IS_STREAM(stream)) {
		fprintf(stderr, "ERROR: This is not a valid stream\n");
		exit(EXIT_FAILURE);
	}
	
	payload = arv_camera_get_payload( camera, &error ) ;
	if ( error != NULL ) {
		fprintf(stderr,"ERROR: Could not retrieve payload size\n") ;
		fprintf(stderr,"%s\n", error->message );
		exit(EXIT_FAILURE);
	}
	printf( "\nPayload size: %ld Bytes \n", payload ) ;
	printf( "Buffer: %d images\n", buffer_size ) ;
	
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
	
	arv_camera_start_acquisition(camera, &error);
	if ( error == NULL ) {
		while(1) {
			ArvBuffer *buffer ;
			time_t t = time(NULL) ;
			struct tm tm = *localtime(&t);
			char filename[100] ;
			sprintf(filename, "%d-%02d-%02d %02d:%02d:%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
			FILE* fichier=fopen( filename, "wb" );
			for (i=0; i<1800; i++) {
				buffer = arv_stream_pop_buffer (stream);
				if ( ARV_IS_BUFFER( buffer)){
					fwrite( arv_buffer_get_data(buffer,NULL)
							, payload, 1, fichier) ;
					arv_stream_push_buffer (stream, buffer);
				}
			}
			fclose(fichier) ;
		}
	}
	arv_camera_stop_acquisition( camera, &error ) ;
	
	// Liberation memoire

	g_clear_object( &stream ) ;
	g_clear_object( &camera ) ;

	free(output) ;
	free(output_prefix) ;
	free(camera_name) ;

	exit(EXIT_SUCCESS);

}

