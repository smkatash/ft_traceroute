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

# define DEFAULT_HIGH_PORT 33433
# define RECV_TIMEOUT_SEC 5
# define DEFAULT_PAYLOAD_SIZE 68
# define MAX_PACKET_SIZE 512
# define DEFAULT_MAX_HOPS 30
# define DEFAULT_MAX_PROBES 3

 // If the IP_HDRINCL option is set on it, then the application would need to provide the IP header too.

typedef struct s_host {
    char    *hostname;
    char    *canonname;
    char    *server_hostname;
    struct  sockaddr_in inet_addr;
    char    ip_addr[INET_ADDRSTRLEN];
} t_host;

typedef struct s_pseudo_udphdr {
	u_int32_t source_address;
	u_int32_t dest_address;
	u_int8_t placeholder;
	u_int8_t protocol;
	u_int16_t udp_length;
} t_pseudo_udphdr;

typedef struct s_traceroute {
    int hops;
    int probe_count;
    int ttl;
    pid_t id;
    int fd_udp;
    int fd_icmp;
    int seq;
    t_host  destination;
    size_t  packet_len;
} t_traceroute;

void    parse_input(int argc, char **argv, t_traceroute *trace);
void    create_socket(t_traceroute *data);
void    ft_traceroute(t_traceroute *data);
unsigned short checksum(void *header, int len);

#endif