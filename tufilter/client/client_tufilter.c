#include "client_tufilter.h"
/*Поиск транспортного поротокола*/

int transport_protocol(char *value_of_transport_parametr,struct netfilter *filter)
{
   int error = 0;
   if(strcmp("tcp",value_of_transport_parametr)==0)
      filter->transport = IPPROTO_TCP;
   else
      if(strcmp("udp",value_of_transport_parametr)==0)
         filter->transport = IPPROTO_UDP;
      else
         return error;
   return 1;
}

/*поиск ip адреса, если ip не был задан, то счетчик availability_of_parameters уменьшается на единицу,
если счетчик равен нулю, значит, не был задан ни ip ни port*/

int ip_protocol(char *value_of_ip_parametr,struct netfilter *filter)
{
   int error = 0;
   if(inet_aton(value_of_ip_parametr, &filter->ip) == 0) 
      return error;
   return 1;
}

/*поиск порта, если port не был задан, то счетчик availability_of_parameters уменьшается на единицу,
если счетчик равен нулю, значит, не был задан ни ip ни port*/

int port(char *value_of_port,struct netfilter *filter)
{
   int error = 0;
   filter->port = atoi(value_of_port);
   if(filter->port == 0 || filter->port > 65535)
      return error;
   return 1;
}

/*поиск используемого мода(enable/disable)*/

int mode(char *value_of_mode, struct netfilter *filter)
{
   int error = 0;
   if(strcmp("enable",value_of_mode)==0)
      filter->mode = FILTER_ENABLE;
   else
      if(strcmp("disable",value_of_mode)==0)
         filter->mode = FILTER_DISABLE;
      else 
         return error;
   return 1;
}

/*функция отправки фильтра*/

void send_rule_of_filter(struct netfilter *filter, int fd_dev)
{
   if(ioctl( fd_dev, IOCTL_SET_FILTER, filter))
      printf("Error IOCTL_SET_FILTER \n");
}

/*функция выдача статистики*/

void show_stat(char **argv,int fd_proc)
{
   struct all_netfilters stat_filters[NUMBER_FILTER_RULE];
   int i,count=0;
   char *transport_protocol,*in_or_out;
   if (ioctl( fd_proc, IOCTL_GET_STAT_FILTER, stat_filters ))
      printf( "Error IOCTL_GET_STAT_FILTER\n" );
   for(i=0; i<NUMBER_FILTER_RULE; i++)
   {
      if(stat_filters[i].free_cell_of_filter == 1)
      { 
         if( stat_filters[i].transport == IPPROTO_UDP) 
            transport_protocol = "UDP";
         else
            if( stat_filters[i].transport == IPPROTO_TCP) 
               transport_protocol = "TCP";
         if(stat_filters[i].in_or_out_packet == INPUT_PACKET)
            in_or_out = "Input";
         else
            if(stat_filters[i].in_or_out_packet == OUTPUT_PACKET)
               in_or_out = "Output";
         if(stat_filters[i].port == 0)
            printf("%d. | Transport: %s | IP = %s | Type of traffic: %s | Number of packets dropped: %d\n", 
                  i+1-count, transport_protocol, (char *)inet_ntoa(stat_filters[i].ip), in_or_out, stat_filters[i].number_drop_packet);
         else
            if(stat_filters[i].ip.s_addr == 0)
               printf("%d. | Transport: %s | Port: %d | Type of traffic: %s  | Number of packets dropped:%d\n", 
                     i+1-count, transport_protocol, stat_filters[i].port, in_or_out, stat_filters[i].number_drop_packet); 
            else           
               printf("%d. | Transport: %s | Port: %d IP = %s | Type of traffic: %s | Number of packets dropped: %d\n",
                     i+1-count, transport_protocol, stat_filters[i].port, (char *)inet_ntoa(stat_filters[i].ip),in_or_out ,stat_filters[i].number_drop_packet);
      }
      else
         count++;
   }
}

int main(int argc, char *argv[]) 
{
   int fd_proc,fd_dev,error,parametr,option_index,flag_transport,flag_in_out_packet,flag_mode;
   struct netfilter filter;
   const char* short_options = "hio";
   const struct option long_options[] = {
        {"help",no_argument,NULL,'h'},
        {"transport",required_argument,NULL,0},
        {"ip",required_argument,NULL,1},
        {"port",required_argument,NULL,2},
        {"filter",required_argument,NULL,3},
        {"show",required_argument,NULL,4},
        {NULL,0,NULL,0}
    };

   /*Если значение availability_of_parameters == 0, значит не был задан ни ip ни port*/

   availability_of_parameters = 0;
   flag_transport = 0;
   flag_in_out_packet = 0;
   flag_mode = 0;
   memset (&filter, 0, sizeof(struct netfilter));

   /*открываем два файла, через один отправляем правило спомощью ioctl/dev, через другой получаем статистику с помощью ioctl/procfs*/

   if( ( fd_dev = open( DEV_PATH, O_RDWR ) ) < 0 ) 
   {
      printf("Open device error\n");
      return 0;
   }
   if( ( fd_proc = open( PROC_PATH, O_RDWR ) ) < 0 ) 
   {
      printf( "Open proc error\n" );
      return 0;
   }
   
   while ((parametr = getopt_long(argc,argv,short_options,long_options,&option_index))!=-1)
   {
      switch(parametr){



         case 0: 
            error = transport_protocol( optarg, &filter );
            if(error == 0)
            {
               printf("Incorrect transport protocol(-h/--help)!!!\n");
               return 0;
            }
            flag_transport = 1;
            break;



         case 1: 
            error = ip_protocol( optarg, &filter );
            if(error == 0)
            {
               printf("IP incorrect!!!\n");
               return 0;
            }
            availability_of_parameters++;
            break;



         case 2: 
            error = port( optarg, &filter );
            if(error == 0)
            {
               printf("Port incorrect!!!\n");
               return 0;
            }
            availability_of_parameters++;
            break;



         case 3: 
            error = mode( optarg, &filter );
            if(error == 0)
            {
               printf("Incorrect value of mode(-h/--help)!!!\n");
               return 0;
            }
            flag_mode = 1;
            break;

         /*команда show, которая выводит статистику по всем фильтрам*/

         case 4:
            show_stat( argv, fd_proc );
            break;



         case 'h': 
            printf("\nSystem for blocking network packets.\n\nParameters:\n    --transport [value]    protocol of the transport layer. Available protocols udp/tcp.\n    --filter [value]    mode of operation of the filter(enable/disable)\n    --port [value]    port\n    --ip [value]    ip address\n    -i    input packet\n    -o    output packet\n\n");       
            return 0;
            break;



         case 'i':
            filter.in_or_out_packet = INPUT_PACKET;
            flag_in_out_packet = 1;
            break;



         case 'o':
            filter.in_or_out_packet = OUTPUT_PACKET;
            flag_in_out_packet = 1;
            break;



         default: 
            printf("Found unknown option(-h/--help)\n");
            return 0;
            
      }
   }
   printf(" | Transport: %d | IP = %s port = %d mode = %d i/o = %d\n", filter.transport, (char *)inet_ntoa(filter.ip),filter.port,filter.mode,filter.in_or_out_packet);
   if(flag_transport == 0)
      printf("Select transport protocol(-h/--help)\n");
   else 
      if( availability_of_parameters == 0 )
         printf("No ip address and no port in rule of filter\n");
      else
         if(flag_in_out_packet == 0)
            printf("Select input or output packet(-h/--help)\n");
         else
            if(flag_mode == 0)
               printf("Select mode of filter(-h/--help)\n");
            else
               send_rule_of_filter( &filter,fd_dev );
   close( fd_dev );
   close( fd_proc );
   return EXIT_SUCCESS;
}