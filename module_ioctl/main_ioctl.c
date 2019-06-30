#include <fcntl.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include "ioctl.h"

#define ERR(...) fprintf( stderr, "\7" __VA_ARGS__ ), exit( EXIT_FAILURE )

int main() 
{
   	int dfd;                  // дескриптор устройства 
   	if( ( dfd = open( DEVPATH, O_RDWR ) ) < 0 ) 
	   ERR( "Open device error: %m\n" );
   	if( ioctl( dfd, IOCTL_GET_STRING, &buf ) ) 
   		ERR( "IOCTL_GET_STRING error: %m\n" );
   	fprintf( stdout, (char*)&buf );
   	close( dfd );
   	return EXIT_SUCCESS;
};