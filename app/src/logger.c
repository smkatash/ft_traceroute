#include "ft_traceroute.h"

void	log_error(const char *err) {
	dprintf(STDERR_FILENO, "%s: %s\n", PROG_NAME, err);
}

void    log_message(const char *str) {
	dprintf(STDOUT_FILENO, str);
}

void    log_header(t_traceroute *data) {
    dprintf(STDOUT_FILENO, "%s to %s (%s), %d hops max, %d byte packets\n", PROG_NAME, data->destination.hostname, \
        data->destination.ip_addr, DEFAULT_MAX_HOPS, DEFAULT_UDP_MAX_PACKET_SIZE);
}

void    log_hop(int hop) {
    if (hop < 10) {
        dprintf(STDOUT_FILENO, " %d ", hop);
    } else {
        dprintf(STDOUT_FILENO, "%d ", hop);
    }
}

void log_source_address(void *msg) {
    struct iphdr *iphdr;
    struct in_addr source_addr;
    char hostname[NI_MAXHOST];
    struct sockaddr_in sa;

    iphdr = (struct iphdr *)msg;
    source_addr.s_addr = iphdr->saddr;
    const char *ip_str = inet_ntoa(source_addr);
    sa.sin_family = AF_INET;
    sa.sin_addr = source_addr;
    int reverse_dna = getnameinfo((struct sockaddr *)&sa, sizeof(sa), hostname, sizeof(hostname), NULL, 0, NI_NAMEREQD);
     if (reverse_dna != 0) {
        dprintf(STDOUT_FILENO, "%s (%s)", ip_str, ip_str);
    } else {
        dprintf(STDOUT_FILENO, "%s (%s)", hostname, ip_str);
    }
}
