#include "ft_traceroute.h"

void	log_error(const char *err) {
	dprintf(STDERR_FILENO, "ft_traceroute: %s\n", err);
	exit(EXIT_FAILURE);
}

void    log_header(t_traceroute *data) {
    dprintf(STDIN_FILENO, "%s to %s (%s), %d hops max, %d byte packets\n", PROG_NAME, data->destination.hostname, \
        data->destination.ip_addr, DEFAULT_MAX_HOPS, MAX_PACKET_SIZE);
}

void    log_hop(int hop) {
    if (hop < 10) {
        dprintf(STDIN_FILENO, " %d ", hop);
    } else {
        dprintf(STDIN_FILENO, "%d ", hop);
    }
}

void log_source_address(void *msg) {
    struct iphdr *iphdr;
    iphdr = (struct iphdr *)msg;
    dprintf(STDIN_FILENO, "%s (%s)", inet_ntoa(*(struct in_addr *)&iphdr->saddr), inet_ntoa(*(struct in_addr *)&iphdr->saddr));
}
