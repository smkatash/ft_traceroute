#ifndef FT_TRACEROUTE
#define FT_TRACEROUTE

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
# define DEFAULT_PAYLOAD_SIZE 8
# define MAX_PACKET_SIZE 60
# define DEFAULT_MAX_HOPS 30
# define DEFAULT_MAX_PROBES 3
# define VALID_RESPONSE 0
# define INVALID_RESPONSE 1
# define DESTINATION_REACHED 2


 // If the IP_HDRINCL option is set on it, then the application would need to provide the IP header too.

typedef struct s_host {
    char    *hostname;
    char    *canonname;
    char    *server_hostname;
    struct  sockaddr_in inet_addr;
    char    ip_addr[INET_ADDRSTRLEN];
} t_host;

typedef struct s_traceroute {
    int ttl;
    pid_t id;
    int fd_udp;
    int fd_icmp;
    t_host  destination;
    uint16_t sent_port;
    size_t  packet_len;
} t_traceroute;

void    parse_input(int argc, char **argv, t_traceroute *trace);
void    init_socket(t_traceroute *data);
void    ft_traceroute(t_traceroute *data);
void    print_time_diff(struct timeval *tv_start, struct timeval *tv_end);
void    log_header(t_traceroute *data);
void	log_error(const char *err);
void    log_hop(int hop);
void    log_source_address(void *msg);

#endif