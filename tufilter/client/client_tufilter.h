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
#include "../common.h"

#define ERR(...) fprintf( stderr, "\7" __VA_ARGS__ ), exit( EXIT_FAILURE )
#define DEV_PATH "/dev/ioctl_dev"
#define PROC_PATH "/proc/ioctl_proc"

static int availability_of_parameters;

int search_transport(int argc, char **argv,struct netfilter *filter);
int search_ip(int argc, char **argv,struct netfilter *filter);
int search_port(int argc, char **argv,struct netfilter *filter);
int search_mode(int argc, char **argv,struct netfilter *filter);
void send_rule_of_filter(struct netfilter *filter, int fd_dev);
int show_stat(char **argv,int fd_proc);