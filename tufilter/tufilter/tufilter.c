#include "tufilter.h"

MODULE_LICENSE("GPL");

static struct file_operations file_ops = {
  .owner = THIS_MODULE,
  .unlocked_ioctl = filter_ioctl,
};

/*Hook функция, которая перехватывает входящте и исходящие пакеты, хранящиеся в структуре sk_buff*/

uint32_t netfilter_hook(void *priv,struct sk_buff *skb,const struct nf_hook_state *state)
{
  struct iphdr *ip;
  struct udphdr *udp_or_tcp;
  int i;

  /*функции, которые возвращаю адреса на соответствующие заголовки в пакете*/

  ip = (struct iphdr *) skb_network_header(skb);
  udp_or_tcp = (struct udphdr *) skb_transport_header(skb);
  
  /*фильтрация только udp и tcp пакетов*/

  if(ip->protocol == IPPROTO_UDP || ip->protocol == IPPROTO_TCP)
  {
    for(i=0; i < NUMBER_FILTER_RULE; i++)
    {
      if(all_filters[i].free_cell_of_filter == 1)
      {

        /*если в фильтре отсутствует порт, то фильтруем только по ip, как по source так и по destination*/

        if(all_filters[i].port == 0)
        {
          if(all_filters[i].ip.s_addr == ip->daddr)  
          {
            printk(KERN_INFO "package dropped by destination ip\n");
            all_filters[i].number_drop_packet++;
            return NF_DROP;
          }
          else
          {
            if(all_filters[i].ip.s_addr == ip->saddr)  
            {
              printk(KERN_INFO "package dropped by source ip\n");
              all_filters[i].number_drop_packet++;
              return NF_DROP;
            }
          }
        }
        else
        {

          /*если в фильтре отсутствует ip, то фильтруем только по порту, как по source так и по destination*/

          if(all_filters[i].ip.s_addr == 0)
          {
            if(all_filters[i].port == ntohs(udp_or_tcp->dest))
            {
              printk(KERN_INFO "package dropped by destination port\n");
              all_filters[i].number_drop_packet++;
              return NF_DROP;
            }
            else
            {
              if(all_filters[i].port == ntohs(udp_or_tcp->source))
              {
                printk(KERN_INFO "package dropped by source port\n");
                all_filters[i].number_drop_packet++;
                return NF_DROP;
              }
            }
          }
          else
          {

            /*если в фильтре присутствует ip и port, то фильтруем по их связки, как по source так и по destination*/

            if(all_filters[i].port == ntohs(udp_or_tcp->dest) && all_filters[i].ip.s_addr == ip->daddr)
            {
              printk(KERN_INFO "package dropped by destination port and destination ip\n");
              all_filters[i].number_drop_packet++;
              return NF_DROP;
            }
            else
            {
              if(all_filters[i].port == ntohs(udp_or_tcp->source) && all_filters[i].ip.s_addr == ip->saddr)
              {
                printk(KERN_INFO "package dropped by source port and source ip\n");
                all_filters[i].number_drop_packet++;
                return NF_DROP;
              }
            }
          }
        }  
      }
    }
  }
  return NF_ACCEPT;
}

/*Функция, проверяющая существование правила*/

int exist_filter(struct netfilter *filter)
{
  int i;
  for(i=0; i<NUMBER_FILTER_RULE; i++)
  {
    if(all_filters[i].free_cell_of_filter == 1 && all_filters[i].transport == filter->transport 
    && all_filters[i].port == filter->port && all_filters[i].ip.s_addr == filter->ip.s_addr)
    {
        return 0;
    }
  }
    return 1;
}

/*Функция, которая добавляет правило в структуру, хранящей все фильтры, если данное правило уже имеется, 
то оно не добавляется и выводится соответствующая информация в логи*/

int enable_rule_of_filter(struct netfilter *filter)
{
  int i,error;
  error = exist_filter(filter);
  if (error == 0)
  {
    printk("Filter already exist: transport = %d port = %d mode = %d ip = %d\n", filter->transport, filter->port,  filter->mode,  filter->ip.s_addr);
    return 0;
  }
  for(i=0; i<NUMBER_FILTER_RULE; i++)
  {
    if(all_filters[i].free_cell_of_filter == 0)
    { 
      all_filters[i].free_cell_of_filter = 1;
      all_filters[i].transport = filter->transport;
      all_filters[i].port = filter->port;
      all_filters[i].mode = filter->mode;
      all_filters[i].ip.s_addr = filter->ip.s_addr;
      printk("enable filter: transport = %d port = %d ip = %d\n", filter->transport, filter->port,  filter->ip.s_addr);
      return 0;
    }
  }
  printk(KERN_INFO "Exceeded max number of filters");
  return 0;
}

/*Функция, которая стирает правило из структуры, хранящей все фильтры, если такого правила нет, в логи выводится информация, оповещающая об этом*/

int disable_rule_of_filter(struct netfilter *filter)
{
  int i;
  for(i=0; i<NUMBER_FILTER_RULE; i++)
  {
    if(all_filters[i].free_cell_of_filter == 1 && all_filters[i].transport == filter->transport &&  all_filters[i].port == filter->port && all_filters[i].ip.s_addr == filter->ip.s_addr)
    {
      all_filters[i].free_cell_of_filter = 0;
      all_filters[i].number_drop_packet = 0;
      printk("disable filter: transport = %d port = %d ip = %d\n", filter->transport, filter->port,  filter->ip.s_addr);
      return 1;
    }
  }
  printk("No this filter: transport = %d port = %d mode = %d ip = %d\n", filter->transport, filter->port,  filter->mode,  filter->ip.s_addr);
  return 0;
}

/*ioctl функция модуля*/

long filter_ioctl(  struct file *file,unsigned int ioctl_num,unsigned long ioctl_param)
{
  	filter = kmalloc(sizeof(struct netfilter), GFP_KERNEL);
  	if( ( _IOC_TYPE( ioctl_num ) != IOC_MAGIC ) )
  	{ 
    	kfree(filter);
    	return 0;
  	}
  	switch( ioctl_num ) 
    {
        /*Параметр при котором происходит копирование фильтра из userspace в kernelspace через ioctl/dev */

        case IOCTL_SET_FILTER:
          	if (copy_from_user(filter, (void*)ioctl_param, sizeof(struct netfilter)))
            	printk(KERN_INFO "ERROR1\n"); 
          	else
            	if(filter->mode == FILTER_ENABLE)
              	enable_rule_of_filter(filter);
            else 
              	if(filter->mode == FILTER_DISABLE)
                	disable_rule_of_filter(filter);
          	break;

        /*Параметр при котором происходит копирование фильтра из kernelspace в userspace через ioctl/procfs*/

        case IOCTL_GET_STAT_FILTER:
          	if( copy_to_user( (void*)ioctl_param, all_filters, sizeof(struct all_netfilters)*NUMBER_FILTER_RULE )) 
            	printk(KERN_INFO "ERROR2\n"); 
          	break;
   	}
    kfree(filter);
    return 0;
}

/*Функция загрузки модуля*/

static int __init tufilter_init_module(void) 
{
  int i,error;

  /*Регистрация устройства, через которое модуль будет принимать правила.*/

  major_num = register_chrdev(0, DEV_FILE_NAME, &file_ops);
  if (major_num < 0) 
  {
    printk(KERN_INFO "Could not register device: %d\n", major_num);
    return 0;
  } 
  else 
    printk(KERN_INFO "tufilter module loaded with device major number %d\n", major_num);

  /*Регистрация устройства, через которое модуль будет выдавать статистику по текущим заблокированным правилам*/

  proc_file = proc_create(PROC_FILE_NAME, S_IFREG | S_IRUGO, NULL,&file_ops);
  if (proc_file == NULL) 
  {
      printk(KERN_INFO "Error: Could not initialize /proc/ioct_proc\n");
      return 0;
  } 

  /*Начальная инициялизация поля free_cell_of_filter, которое определяет свободное место в структуре, которая хранит фильтры,
    number_drop_packet хранит количество дропнутых пакетов, принадлежащих данному правилу*/

  for(i=0; i<NUMBER_FILTER_RULE; i++)
  {
    all_filters[i].free_cell_of_filter = 0;
    all_filters[i].number_drop_packet = 0;
  }

  /*Определение и регитрация Hook функции, которая бедет перехватывать входящие и исходящие пакеты*/

  hook_local_out.hook = netfilter_hook;
  hook_local_out.pf = PF_INET;
  hook_local_out.hooknum = NF_INET_LOCAL_OUT;
  hook_local_out.priority = NF_IP_PRI_FIRST;

  hook_local_in.hook = netfilter_hook;
  hook_local_in.pf = PF_INET;
  hook_local_in.hooknum = NF_INET_LOCAL_IN;
  hook_local_in.priority = NF_IP_PRI_FIRST;

  error = nf_register_net_hook(&init_net, &hook_local_out);
  if (error)
  { 
    printk(KERN_INFO "could not register netfilter hook\n");
    return 0;
  }
  else
  {
    error = nf_register_net_hook(&init_net, &hook_local_in);
    if (error) 
    {
      printk(KERN_INFO "could not register LOCAL_IN netfilter hook\n");
      return 0;
    }
  }
  printk(KERN_INFO " module initialized\n" );
  return 0;
}

/*Функция выгрузки модуля*/

static void __exit tufilter_exit_module(void) 
{
  nf_unregister_net_hook(&init_net, &hook_local_out);
  nf_unregister_net_hook(&init_net, &hook_local_in);
  unregister_chrdev(major_num, DEV_FILE_NAME);
  remove_proc_entry(PROC_FILE_NAME, NULL);
  printk(KERN_INFO "Goodbye, World!\n");
}


module_init(tufilter_init_module);
module_exit(tufilter_exit_module);