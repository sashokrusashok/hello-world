#include <linux/netfilter_bridge.h> 
#include <linux/skbuff.h>
#include <linux/net.h>
#include <linux/in.h> 
#include <linux/module.h>
#include <linux/uaccess.h>
#include <asm/uaccess.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/string.h>
#include <linux/kernel.h>
#include <linux/slab.h>                 
#include <linux/ip.h>
#include <linux/init.h>
#include <net/ip.h>
#include <linux/sched.h>
#include <linux/udp.h> 
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>             
#include <linux/proc_fs.h> 
#include <linux/kobject.h>
#include "../common.h"

#define DEV_FILE_NAME "ioctl_dev"
#define PROC_FILE_NAME "ioctl_proc"

struct all_netfilters all_filters[NUMBER_FILTER_RULE];
struct netfilter *filter;
struct proc_dir_entry *proc_file;
struct nf_hook_ops hook_local_in, hook_local_out;
int major_num;

uint32_t netfilter_hook(void *priv,struct sk_buff *skb,const struct nf_hook_state *state);
int exist_filter(struct netfilter *filter);
int enable_rule_of_filter(struct netfilter *filter);
int disable_rule_of_filter(struct netfilter *filter);
long filter_ioctl(  struct file *file,unsigned int ioctl_num,unsigned long ioctl_param);