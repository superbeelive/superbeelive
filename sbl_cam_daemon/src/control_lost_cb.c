#include <stdio.h>
#include <arv.h>

extern int done ;

void
control_lost_cb (ArvGvDevice *gv_device)
{
	printf ("\n\nControl lost\n");
	done = 1 ;
}

