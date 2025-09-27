#define _DEFAULT_SOURCE
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>

int filter ( const struct dirent* dir ) {
	int r = strncmp(dir->d_name, "M02C02_20250926_2146", 20 ) ;
	return !r ;
}


int
main(void)
{
   struct dirent **namelist;
   int n;

   n = scandir(".", &namelist, filter, alphasort);
   if (n == -1) {
       perror("scandir");
       exit(EXIT_FAILURE);
   }

   while (n--) {
       printf("%s\n", namelist[n]->d_name);
       free(namelist[n]);
   }
   free(namelist);

   exit(EXIT_SUCCESS);
}

