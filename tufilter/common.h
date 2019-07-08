 struct netfilter{
	int transport;
	int port;
	int mode;
	struct in_addr ip;
} ;
 struct all_netfilters{
 	int free_cell_of_filter;
	int transport;
	int port;
	int mode;
	struct in_addr ip;
	int number_drop_packet;
} ;

#define NUMBER_FILTER_RULE 10
#define FILTER_ENABLE 1 
#define FILTER_DISABLE 0
#define IOC_MAGIC    'h'
#define IOCTL_SET_FILTER _IOW( IOC_MAGIC, 1,  struct netfilter )
#define IOCTL_GET_STAT_FILTER _IOR( IOC_MAGIC, 2, struct all_netfilters )
