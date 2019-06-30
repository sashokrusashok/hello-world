char buf[ 160 ];

#define IOC_MAGIC    'h'
//создавать уникальные коды команд ioctl
#define IOCTL_GET_STRING _IOR( IOC_MAGIC, 1, char * )

#define DEVPATH "/dev/ioctl"