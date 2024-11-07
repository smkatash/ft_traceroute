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

# define PROG_NAME "traceroute"
# define DEFAULT_HIGH_PORT 33433
# define RECV_TIMEOUT_SEC 5
# define DEFAULT_PAYLOAD_SIZE 32
# define MAX_PACKET_SIZE 60
# define DEFAULT_MAX_HOPS 30
# define DEFAULT_MAX_PROBES 3
# define VALID_RESPONSE 0
# define INVALID_RESPONSE 1
# define DESTINATION_REACHED 2
#define SHORT_TIMEOUT_SEC 0
#define SHORT_TIMEOUT_USEC 500000

#define USAGE_MESSAGE "Usage: \ntraceroute host\n\nArguments:\n\
+    host\t\tThe host to trace the route to\n"

// PID + probe can be set as source port and then bind
// If the IP_HDRINCL option is set on it, then the application would need to provide the IP header too.
//  MAX Packet size (60 bytes) = IP Header (20 bytes) + UDP Header (8 bytes) + Payload (32 bytes)

typedef struct s_host {
    char    *hostname;
    struct  sockaddr_in inet_addr;
    char    ip_addr[INET_ADDRSTRLEN];
} t_host;

typedef struct s_traceroute {
    int             fd_udp;
    int             fd_icmp;
    t_host          destination;
    uint16_t        dest_port;
    size_t          packet_len;
    pid_t           id;
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