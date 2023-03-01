#include <stdio.h>
#include <arv.h>

int main( int argc, char** argv, char** envv ) {

	int i ;

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

	exit(EXIT_SUCCESS);

}

