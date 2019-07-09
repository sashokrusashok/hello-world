#include "client_tufilter.h"
/*Поиск транспортного поротокола*/

int search_transport(int argc, char **argv,struct netfilter *filter)
{
   int i;
   for(i=0;i<argc;i++)
   {
      if(strcmp("--transport",argv[i])==0)
      {
         if(strcmp("tcp",argv[i+1])==0)
         {
            filter->transport = IPPROTO_TCP;
            return 1;
         }
         else
            if(strcmp("udp",argv[i+1])==0)
            {
               filter->transport = IPPROTO_UDP;
               return 1;
            }
            else
            {
               printf("Incorrect transport protocol. Enter tcp/udp!!!\n");
               return 0;
            }
      }
   }
   printf("Incorrect input. Enter parameter '--transport'!!!\n");
   return 0;
}

/*поиск ip адреса, если ip не был задан, то счетчик availability_of_parameters уменьшается на единицу,
если счетчик равен нулю, значит, не был задан ни ip ни port*/

int search_ip(int argc, char **argv,struct netfilter *filter)
{
   int i;
   for(i=0;i<argc;i++)
   {
      if(strcmp("--ip",argv[i])==0)
      {
         if(inet_aton(argv[i + 1], &filter->ip) == 0) 
         {
            printf("ip incorrect!!!\n");
            return 0;
         }
         else
            return 1;
      }
   }
   availability_of_parameters--;
   return 1;
}

/*поиск порта, если port не был задан, то счетчик availability_of_parameters уменьшается на единицу,
если счетчик равен нулю, значит, не был задан ни ip ни port*/

int search_port(int argc, char **argv,struct netfilter *filter)
{
   int i;
   for(i=0;i<argc;i++)
   {
      if(strcmp("--port",argv[i])==0)
      {
         filter->port = atoi(argv[i+1]);
         if(filter->port == 0 || filter->port > 65535)
         {
            printf("port incorrect!!!\n");
            return 0;
         }
         else 
            return 1;
      }
   }
   availability_of_parameters--;
   return 1;
}

/*поиск используемого мода(enable/disable)*/

int search_mode(int argc, char **argv,struct netfilter *filter)
{
   int i;
   for(i=0;i<argc;i++)
   {
      if(strcmp("--filter",argv[i])==0)
      {
         if(strcmp("enable",argv[i+1])==0)
         {
            filter->mode = FILTER_ENABLE;
            return 1;
         }
         else
            if(strcmp("disable",argv[i+1])==0)
            {
               filter->mode = FILTER_DISABLE;
               return 1;
            }
            else 
            {
               printf("Enter parameter '--filter' with value enable/disable!!!\n");
               return 0;
            }
      }
   }
   printf("Enter parameter '--filter' with value enable/disable!!!\n");
   return 0;
}

int search_input_or_output_packet(int argc, char **argv,struct netfilter *filter)
{
   int i;
   for(i=0; i<argc; i++)
   {
      if(strcmp("--i",argv[i])==0)
      {
         filter->in_or_out_packet = INPUT_PACKET;
         return 1;
      }
      else 
         if(strcmp("--o",argv[i])==0)
         {
            filter->in_or_out_packet = OUTPUT_PACKET;
            return 1;
         }
   }
   printf("Select parameter input or output packets(--i/--o)\n");
   return 0;
}

/*функция отправки фильтра*/

void send_rule_of_filter(struct netfilter *filter, int fd_dev)
{
   if(ioctl( fd_dev, IOCTL_SET_FILTER, filter))
      printf("Error IOCTL_SET_FILTER \n");
}

/*функция выдача статистики*/

int show_stat(char **argv,int fd_proc)
{
   struct all_netfilters stat_filters[NUMBER_FILTER_RULE];
   if(strcmp("--show",argv[1])== 0)
   {
      int i,count=0;
      char *transport_protocol,*in_or_out;
      if (ioctl( fd_proc, IOCTL_GET_STAT_FILTER, stat_filters ))
         printf( "Error IOCTL_GET_STAT_FILTER\n" );
      for(i=0; i<NUMBER_FILTER_RULE; i++)
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
      return 1;
   }
   else
      return 0;
}
char *read_file_ip_interface(int fd)
{
   int size_of_file,i=0;
   char *file,*ip_interface;;
   size_of_file = lseek( fd , 0 , SEEK_END );
   printf("\nsize = %d\n",size_of_file);
   file = malloc(size_of_file*sizeof(char));
   lseek(fd, 0, SEEK_SET);
   read(fd,file,size_of_file);
   if( strncmp(file, "ip_interface:",13) == 0) 
   {
      while(1)
      {
         if(file[13+i] != ' ')
         {
            ip_interface = &file[13+i];
            break;
         }
         i++;
      }    
   }
   free(file);
   return ip_interface;
}

int main(int argc, char *argv[]) 
{
   int fd_proc,fd_dev,fd_ip_eth,error;
   struct netfilter filter;
   char *ip_interface;

   /*Если значение availability_of_parameters == 0, значит не был задан ни ip ни port*/

   availability_of_parameters = 2;
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
   if( ( fd_ip_eth = open( "ip_interface", O_RDWR ) ) < 0 ) 
   {
      printf("Open device error\n");
      return 0;
   }
   ip_interface = read_file_ip_interface(fd_ip_eth);
   



   /*если ввели команду show, то выводится статистика по всем фильтрам*/

   if( argc == 2 )
   {
      if( show_stat( argv, fd_proc ) == 0 )
         printf( "Command incorrect!!!" );
   }
   else
   {
      error = search_transport( argc, argv, &filter );
      if(error != 0)
      {
         error = search_ip( argc, argv, &filter );
         if(error != 0)
         {
            error = search_port( argc, argv, &filter );
            if(error != 0)
            {
               error = search_mode( argc, argv, &filter );
               if(error != 0)
               {
                  error = search_input_or_output_packet( argc, argv, &filter );
                  if (error != 0)
                  {
                     if(inet_aton(ip_interface, &filter.address_of_interface) == 0) 
                        printf("ip of interface incorrect!!!\n");
                     else
                        if( availability_of_parameters == 0 )
                           printf("No ip address and no port in rule of filter\n");
                        else
                           send_rule_of_filter( &filter,fd_dev );
                  }
               }
            }
         }
      }
   }
   close( fd_ip_eth );
   close( fd_dev );
   close( fd_proc );
   return EXIT_SUCCESS;
}