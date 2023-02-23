#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arv.h>

int main( int argc, char** argv, char** envv ) {

	int rows = 1080;	// image height
	int cols = 1920;	// image width
	double fps = 30 ;	// frames per sec
	char output[200] = "/home/superbeelive/records/video" ;	// output path
	char camera_name[200] = "" ;							// camera name
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
				strncpy( output, optarg, sizeof(output));
				break;
			case 'h':
			default:
				fprintf(stderr,"Usage: %s [-c cols] [-r rows] [-f framerate] [-o output_directory] camera\n", argv[0] ) ;
				exit(EXIT_FAILURE) ;
		}
	}

	if ( optind < argc ) {
		strncpy( camera_name, argv[optind], sizeof(camera_name) );
	}


	// Affichages des parametres
	
	printf("Aravis version: %d.%d.%d\n", arv_get_major_version(), arv_get_minor_version(), arv_get_micro_version() ) ;
	printf("Resolution: %dx%d\n", cols, rows ) ;
	printf("Framerate: %.2lf\n", fps ) ;
	printf("Output directory: %s\n", output ) ;

	// Connexion à la camera
	
	if ( camera_name[0] == '\0' ) {
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

	// Setup callback
	


	// Liberation memoire

	g_clear_object( &stream ) ;
	g_clear_object( &camera ) ;

	exit(EXIT_SUCCESS);

}
