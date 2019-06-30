#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/slab.h>                 
#include <linux/uaccess.h>             
#include <linux/sysfs.h> 
#include <linux/kobject.h> 
#include <linux/netfilter_ipv4.h>
#include <linux/ip.h>
#include <net/ip.h>
#include <linux/udp.h> 
#include <linux/netfilter.h>
#include <linux/netfilter_bridge.h> 
#include <linux/skbuff.h>
#include <linux/net.h>
#include <linux/icmp.h>
#include <linux/icmp.h>

#define BLOCK_PORT 7777

MODULE_LICENSE("GPL");
#define DEVICE_NAME "tyu"
#define LEN_MSG 160
static char buf_msg[ LEN_MSG + 1 ] = "Hello from module!\n";

struct nf_hook_ops nfho;
static struct class *fw_class;

static ssize_t foo_show( struct class *class, struct class_attribute *attr, char *buf );
static ssize_t foo_store( struct class *class, struct class_attribute *attr,const char *buf, size_t count );
static int value;
static char *p;
// hook out packets, accept packet
/*unsigned int hook_func_out(unsigned int hooknum, struct sk_buff *skb,const struct net_device *in, const struct net_device *out, int (*okfn)(struct sk_buff *)) 
{
	accepted_num++;
	return NF_ACCEPT;
}

// hook in packets, accept packet
unsigned int hook_func_in(unsigned int hooknum, struct sk_buff *skb,const struct net_device *in, const struct net_device *out,int (*okfn)(struct sk_buff *)) 
{
	accepted_num ++;
	return NF_ACCEPT;
}

// hook forward packets, drop packet
unsigned int hook_func_forward(unsigned int hooknum, struct sk_buff *skb,const struct net_device *in, const struct net_device *out, int (*okfn)(struct sk_buff *)) 
{
	dropped_num++;
	return NF_DROP;
}

static struct nf_hook_ops nfho_forward;
static struct nf_hook_ops nfho_out;
static struct nf_hook_ops nfho_in;*/
uint32_t netfilter_hook(void *priv,struct sk_buff *skb,const struct nf_hook_state *state)
{
    struct iphdr *ip;
    struct tcphdr *tcp;
    struct udphdr *udp;
    struct icmphdr *icmp;
    struct ethhdr *eth;
    unsigned char *b = NULL;
    int i;
    //b = skb->head;
    eth = (struct ethhdr *)skb_mac_header(skb);
   // printk(KERN_INFO "\n");
    for(i=0; i<ETH_ALEN;i++)
    {
    	printk(" %x ",eth->h_source[i]);
    	//printk(KERN_INFO " %x",ntohs(b[i+skb->mac_header]));
    }
    printk(KERN_INFO "\n eth_type =%x %d %d %d\n",ntohs(eth->h_proto),skb->mac_header,skb->network_header,skb->transport_header);
 	ip = (struct iphdr *) skb_network_header(skb);
 	printk(KERN_INFO "\nip_id = %d\n", ntohs(ip->id));
	if(ip->protocol == IPPROTO_UDP) 
	{
		//udp_h = (struct udphdr *) (skb_transport_header(skb) + ip_hdrlen(skb));
		udp = (struct udphdr *) skb_transport_header(skb);
		printk(KERN_INFO "port of pack = %d\n", ntohs(udp->dest));
		printk(KERN_INFO "port of pack(not mod) = %d\n", udp->dest);
		//sprintf (p, "%d", ntohs(udp->dest));
		//printk(KERN_INFO "\n\np = %s\n\n", p);
		if(ntohs(udp->dest) == value) 
		{
			printk(KERN_INFO "package dropped \n");
			return NF_DROP;
		}
				if(ntohs(udp->dest) == value) 
		{
			printk(KERN_INFO "package dropped \n");
			return NF_DROP;
		}
	}
	if(ip->protocol == IPPROTO_ICMP) 
	{
		icmp = icmp_hdr(skb);
                    /*if (icmp->un.echo.id == htons(2))*/
                        printk(KERN_ALERT "ICMP packet\n");
	}
	/*if(ip->protocol == IPPROTO_TCP) 
	{
		//udp_h = (struct udphdr *) (skb_transport_header(skb) + ip_hdrlen(skb));
		tcp = (struct udphdr *) skb_transport_header(skb);
		printk(KERN_INFO "port of pack = %d\n", ntohs(udp->dest));
		printk(KERN_INFO "port of pack(not mod) = %d\n", udp->dest);
		if(ntohs(udp->dest) == etx_value) {
			printk(KERN_INFO "package dropped \n");
			return NF_DROP;
		}
	}*/
	return NF_ACCEPT;
}
// Метод show(чтения) из sysfs API.Вызываетcя при вызове метода метод show() конкретного файла в системе sysfs 
static ssize_t foo_show( struct class *class, struct class_attribute *attr, char *buf ) 
{
   strcpy( buf, buf_msg );
   printk( "read %d\n", strlen( buf ) );
   return strlen( buf );
   //hreturn simple_read_from_buffer(buf, len, offset, msg_ptr, len_ker_buf);
}
 
// Метод store() из sysfs API. Вызываетcя при вызове метода store() конкретного файла в системе sysfs 
static ssize_t foo_store( struct class *class, struct class_attribute *attr,const char *buf, size_t count ) 
{
	//printk( "write %d\n" , count );
	strncpy( buf_msg, buf, count );
	buf_msg[ count ] = '\0';
	sscanf(buf,"%d",&value);
	printk(KERN_INFO "write %s value = %d\n" , buf_msg,value );
	//value = atoi(buf_msg);
	//sprintf (value, "%d", buf_msg);
	return count;
	//simple_read_from_buffer(buf, count, offset, msg_ptr, len_ker_buf);
}

struct class_attribute class_attr_foo = __ATTR(foo, 0660, foo_show,foo_store); 
int __init x_init( void ) 
{
	int ret ;
   	fw_class = class_create(THIS_MODULE, DEVICE_NAME);    
   	if (fw_class == NULL) 
        printk("failed to register device class '%s'\n", "x-class");
    class_create_file( fw_class, &class_attr_foo);

    nfho.hook = netfilter_hook;
    nfho.pf = PF_INET;
    nfho.hooknum = NF_INET_LOCAL_OUT;
    nfho.priority = NF_IP_PRI_FIRST;
    ret = nf_register_hook(&nfho);
    if (ret) 
    {
		printk(KERN_INFO "could not register netfilter hook\n");
	}
    printk( "'xxx' module initialized\n" );
    return 0;
}
 
void x_cleanup( void ) 
{
	nf_unregister_hook(&nfho);
    class_remove_file( fw_class, &class_attr_foo );
  	//unregister_chrdev(fw_major , DEVICE_NAME);
  	class_destroy( fw_class );
   
   return;
}
 
module_init( x_init );
module_exit( x_cleanup );