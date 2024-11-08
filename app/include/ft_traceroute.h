#ifndef FT_TRACEROUTE
#define FT_TRACEROUTE

# include "libft.h"
# include <stdio.h>
# include <string.h>
# include <stdlib.h>
# include <unistd.h>
# include <signal.h>
# include <arpa/inet.h>
# include <sys/types.h>
# include <sys/time.h>
# include <sys/socket.h>
# include <netdb.h>
# include <netinet/in.h>
# include <netinet/ip_icmp.h>
# include <errno.h>
# include <math.h>
# include <limits.h>
#include <stdbool.h>
#include <ctype.h> 
#include <netinet/ip.h>
#include <netinet/udp.h>

# define PROG_NAME "ft_traceroute"
# define DEFAULT_HIGH_PORT 33433
# define RECV_TIMEOUT_SEC 5
# define MAX_ICMP_PACKET_SIZE 1500
# define DEFAULT_UDP_MAX_PACKET_SIZE 60
# define IP_HEADER_SIZE 20
# define UDP_HEADER_SIZE 8
# define DEFAULT_PAYLOAD_SIZE 32
# define DEFAULT_MAX_HOPS 30
# define DEFAULT_MAX_PROBES 3
# define VALID_RESPONSE 0
# define INVALID_RESPONSE 1
# define DESTINATION_REACHED 2
# define SHORT_TIMEOUT_SEC 0
# define SHORT_TIMEOUT_USEC 500000
# define OPTION_QUERY "-q"
# define OPTION_PORT "-p"
# define OPTION_TTL "-m"
# define OPTION_QUERY_ERROR "Option `-q' requires an argument: `-q nqueries'"
# define OPTION_PORT_ERROR "Option `-p' requires an argument: `-p port'"
# define OPTION_TTL_ERROR "Option `-m' requires an argument: `-m max_ttl'"

#define USAGE_MESSAGE "Usage: \ntraceroute host\n\nArguments:\n\
+    host\t\tThe host to trace the route to\nOptions:\n\
-q nqueries\t\tSets the number of probe packets per hop. Default is 3.\n\
-p port\t\t\tSet the destination port to use. It is either\n\t\t\tinitial udp port value for \"default\" method\n\t\t\t\
(incremented by each probe, default is 33434), or\n\t\t\tinitial seq for \"icmp\" (incremented as well,\n\t\t\t\
default from 1), or some constant destination\n\t\t\tport for other methods (with default of 80\n\t\t\tfor \"tcp\",\
53 for \"udp\", etc.)\n-m max_ttl\t\tSet the max number of hops (max TTL to be reached).\n\t\t\tDefault is 30.\n"

// PID + probe can be set as source port and then bind
// If the IP_HDRINCL option is set on it, then the application would need to provide the IP header too.
//  MAX Packet size (60 bytes) = IP Header (20 bytes) + UDP Header (8 bytes) + Payload (32 bytes)

typedef struct s_host {
    char    *hostname;
    struct  sockaddr_in inet_addr;
    char    ip_addr[INET_ADDRSTRLEN];
} t_host;

typedef struct s_trace_option {
    int  port;
    int  ttl;
    int  max_queries;
} t_trace_option;

typedef struct s_traceroute {
    int             fd_udp;
    int             fd_icmp;
    t_host          destination;
    uint16_t        dest_port;
    size_t          packet_len;
    pid_t           id;
    t_trace_option  options;
} t_traceroute;

void    parse_input(int argc, char **argv, t_traceroute *trace);
void    init_socket(t_traceroute *data);
void    clean_up(t_traceroute *data);
int     ft_traceroute(t_traceroute *data);
void    print_time_diff(struct timeval *tv_start, struct timeval *tv_end);
void    log_header(t_traceroute *data);
void	log_error(const char *err);
void    log_hop(int hop);
void    log_source_address(void *msg);
void    log_message(const char *str);

#endif