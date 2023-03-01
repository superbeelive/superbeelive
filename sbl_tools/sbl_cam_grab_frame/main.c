#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/time.h>
#include <arv.h>

#include "sbl.h"
#include "sblv_file.h"
#include <png.h>

int save_png(const char* filename, unsigned char* image_data, int width, int height) {
    FILE *fp;
    png_structp png_ptr;
    png_infop info_ptr;
    png_bytep row_pointer;

    fp = fopen(filename, "wb");
    if (!fp) {
        return -1;
    }

    png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr) {
        fclose(fp);
        return -1;
    }

    info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        png_destroy_write_struct(&png_ptr, NULL);
        fclose(fp);
        return -1;
    }

    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_write_struct(&png_ptr, &info_ptr);
        fclose(fp);
        return -1;
    }

    png_init_io(png_ptr, fp);

    png_set_IHDR(png_ptr, info_ptr, width, height, 8, 
			PNG_COLOR_TYPE_GRAY, 
			PNG_INTERLACE_NONE, 
			PNG_COMPRESSION_TYPE_DEFAULT, 
			PNG_FILTER_TYPE_DEFAULT);

    png_write_info(png_ptr, info_ptr);

    row_pointer = image_data;

    for (int y = 0; y < height; y++) {
        png_write_row(png_ptr, row_pointer);
        row_pointer += width;
    }

    png_write_end(png_ptr, NULL);

    png_destroy_write_struct(&png_ptr, &info_ptr);
    fclose(fp);

    return 0;
}

int main( int argc, char** argv, char** envv){

	int rows = 1200;	// image height
	int cols = 1920;	// image width
	double fps = 30 ;	// frames per sec
	char *output_prefix = NULL; // output prefix
	char *output = NULL ;      	// output path
	char *camera_name = NULL ;  // camera name
	int	buffer_size = 300 ;	// Buffer size ( in images )
	unsigned int id_module = 0 ;
	int module_flag = 0 ;
	unsigned int id_cam = 0 ;
	int cam_flag = 0 ;
	int duration = 1 ;				// Duration in minutes

	int i ;
	int opt ;
	ArvCamera *camera ;
	GError *error = NULL ;
	gint sensor_width, sensor_height ;
	gint region_x, region_y, region_width, region_height ;
	ArvStream *stream ;
	size_t payload ;

	// Parsing des options
	
	while((opt = getopt(argc,argv, "w:h:o:")) != -1) {
		switch (opt) {
			case 'w':
				cols=atoi(optarg);
				break;
			case 'h':
				rows=atoi(optarg);
				break;
			case 'o':
				if ( output_prefix == NULL ) {
					output_prefix = malloc( strlen(optarg)+1 ) ;
					strcpy( output_prefix, optarg );
				} else {
					fprintf(stderr,"WARNING: only the first -o option is considered\n");
				}
				break;
			default: /* '?' */
				fprintf(stderr,"Usage: %s [-w cols] [-h rows] [-o output_directory] id camera\n", argv[0] ) ;
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

	// Construction de la chaine output contenant le chemin complet

	output = malloc(strlen(output_prefix)+strlen(SBL_VIDEO_PATH));
	sprintf( output,"%s%s", output_prefix, SBL_VIDEO_PATH ) ;

	// Affichages des parametres
	
	printf("Aravis version: %d.%d.%d\n", arv_get_major_version(), arv_get_minor_version(), arv_get_micro_version() ) ;
	printf("Resolution: %dx%d\n", cols, rows ) ;
	printf("Output directory: %s\n", output ) ;

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

	// Buffer acquisition

	ArvBuffer *buffer ;
	buffer = arv_camera_acquisition (camera, 0, &error);
	if (ARV_IS_BUFFER (buffer)) {
			// Display some informations about the retrieved buffer 
			printf ("Acquired %d×%d buffer\n",
				arv_buffer_get_image_width (buffer),
				arv_buffer_get_image_height (buffer));
	
			// Image saving

			save_png("./test.png", arv_buffer_get_data(buffer,NULL), cols, rows ) ;

			g_clear_object (&buffer);
	}	

	
	// Liberation memoire

	g_clear_object( &camera ) ;

	free(output) ;
	free(output_prefix) ;
	free(camera_name) ;

	exit(EXIT_SUCCESS);

// ------------------
    return 0 ;
}
