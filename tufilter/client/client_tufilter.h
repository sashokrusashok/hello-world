#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <linux/udp.h> 
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/types.h>
#include <getopt.h>
#include "../common.h"

#define DEV_PATH "/dev/ioctl_dev"
#define PROC_PATH "/proc/ioctl_proc"

int parser(int argc, char **argv, struct netfilter *filter,int fd_proc);
int transport_protocol(char *value_of_parametr,struct netfilter *filter);
int ip_protocol(char *value_of_ip_parametr,struct netfilter *filter);
int port(char *value_of_port,struct netfilter *filter);
int mode(char *value_of_mode, struct netfilter *filter);
void send_rule_of_filter(struct netfilter *filter, int fd_dev);
void show_stat(char **argv,int fd_proc);