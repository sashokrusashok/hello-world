#include <linux/skbuff.h>
#include <linux/if_ether.h>
#include <linux/ip.h>
#include <linux/icmp.h>
#include <net/net_namespace.h>
#include <net/route.h>
#include <net/ip.h>
#include <linux/netdevice.h>

#include <linux/module.h>
#include <linux/kernel.h>

#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>

#define SOURCE_IP "10.0.2.15"
#define DEST_IP "8.8.8.8"
#define MTU 1500

u_char source_mac[6] = {0x08, 0x00, 0x27, 0xc7, 0x63, 0x40};
u_char dest_mac[6] = {0x08, 0x00, 0x27, 0xc3, 0x63, 0x40};

struct nf_hook_ops bundle;

unsigned int inet_addr(char *str) 
{ 
    int a,b,c,d; 
    char arr[4]; 
    sscanf(str,"%d.%d.%d.%d",&a,&b,&c,&d); 
    arr[0] = a; arr[1] = b; arr[2] = c; arr[3] = d; 
    return *(unsigned int*)arr; 
} 

void icmp_ping(void) {
    struct net_device *net_dev = NULL;
    struct net *net = NULL;
    struct rtable *route_table = NULL;
    struct flowi4 fl4;
    struct sk_buff *skb = NULL;
    
    struct ethhdr *eth;
    struct iphdr* ip = NULL;
    struct icmphdr* icmp = NULL;
    int hdr_size = 0,ret;

    hdr_size = sizeof(struct ethhdr) + sizeof(struct iphdr) + sizeof(struct icmphdr);

    skb = alloc_skb(hdr_size, GFP_ATOMIC);
    printk(KERN_ALERT "after alloc\n");
    if (skb == NULL) {
        printk(KERN_ALERT "alloc_skb");
        return;
    }

    skb_reserve(skb, hdr_size);
    
    skb_push(skb, sizeof(struct icmphdr));
    //skb_set_transport_header(skb,sizeof(struct ethhdr) + sizeof(struct iphdr));
    //skb_reset_network_header(skb);
    skb_push(skb, sizeof(struct iphdr));
    //skb_set_network_header(skb,sizeof(struct ethhdr));
    //skb_reset_mac_header(skb);
    skb_push(skb, sizeof(struct ethhdr));
    skb_set_mac_header(skb,0);
    skb_set_network_header(skb,sizeof(struct ethhdr) );
    skb_set_transport_header(skb,sizeof(struct ethhdr) +sizeof(struct iphdr));
    //skb_reset_transport_header(skb);
    /*
    skb_set_mac_header(skb, 0);
    skb_set_network_header(skb, 0);
    skb_set_transport_header(skb, sizeof(struct iphdr));*/
    eth = eth_hdr(skb);
    memcpy(eth->h_source, source_mac, ETH_ALEN);
    memcpy(eth->h_dest, dest_mac, ETH_ALEN);
    eth->h_proto = htons(ETH_P_IP);

    ip = ip_hdr(skb);

    ip->ihl = sizeof(struct iphdr)/4;
    ip->version = 4;
    ip->tos = 0;
    ip->tot_len = sizeof(struct iphdr) + sizeof(struct icmphdr);//htons(
    ip->id = htons(1234);
    ip->frag_off = 0;
    ip->ttl = 255;
    ip->protocol = IPPROTO_ICMP;
    ip->check = 0;
    ip->saddr = inet_addr(SOURCE_IP);
    ip->daddr = inet_addr(DEST_IP);
    //ip->check = ip_fast_csum(ip, ip->ihl);
//in_aton
    icmp = icmp_hdr(skb);
    
    icmp->type = ICMP_ECHO;
    icmp->code = 0;
    icmp->checksum = 0;
    icmp->un.echo.sequence = 0;
    icmp->un.echo.id = 0;

    /*iph->check = ip_fast_csum(iph, iph->ihl);*/
    icmp->checksum = ip_compute_csum(icmp, sizeof(struct icmphdr));

    net_dev = dev_get_by_name(&init_net, "eth0");
    /*net = dev_net(net_dev);*/
    skb->dev = net_dev;
    //ret = dev_queue_xmit(skb);
    fl4.flowi4_oif = net_dev->ifindex;
    fl4.daddr = inet_addr(DEST_IP);
    fl4.saddr = inet_addr(SOURCE_IP);
/*struct rtable ip_route_output_key(struct net *net, struct flowi4 *flp)*/
    route_table = ip_route_output_key(&init_net, &fl4);//ключ маршрута, __mkroute _ output создает маршрут и запись кэша назначения
/*static inline void skb_dst_set(struct sk_buff *skb, struct dst_entry *dst)*/
    skb_dst_set(skb, &route_table->dst);//устанавливам в skb запись назначения dst_entry *dst – маршрут для sk_buff

    printk(KERN_ALERT "before ip_local_out\n");
    /*
//ip_local_out выз функц __ip_local_out, которая вызывает функцию nf_hook-, которая вызывает nf_hook_thresh, которая сначала проверяет, 
установлены ли какие-либо фильтры для указанного семейства протоколов (NFPROTO_IPV4 and NF_INET_LOCAL_OUT)
    */
    pr_info("ip_local_out: %d", ip_local_out(&init_net, NULL, skb));

}

int __init pf_init(void)
{
    printk(KERN_INFO "start module port filter\n");

    icmp_ping();
    return 0;
}

void __exit pf_exit(void)
{
    printk(KERN_INFO "end module port filter\n");
}

module_init(pf_init);
module_exit(pf_exit);

MODULE_LICENSE("GPL");
MODULE_LICENSE("GPL v2");