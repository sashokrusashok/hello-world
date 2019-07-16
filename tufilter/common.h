 struct netfilter{
	int transport;
	int port;
	int mode;
	int in_or_out_packet;
	struct in_addr ip;
} ;
 struct all_netfilters{
 	int free_cell_of_filter;
	int transport;
	int port;
	int mode;
	int in_or_out_packet;
	struct in_addr ip;
	int number_drop_packet;
} ;

#define NUMBER_FILTER_RULE 10
#define FILTER_ENABLE 1 
#define FILTER_DISABLE 0
#define INPUT_PACKET 0
#define OUTPUT_PACKET 1
#define IOC_MAGIC    247
#define IOCTL_SET_FILTER _IOW( IOC_MAGIC, 1,  struct netfilter )
#define IOCTL_GET_STAT_FILTER _IOR( IOC_MAGIC, 2, struct all_netfilters )
