#include "ft_traceroute.h"

int    init_icmp_socket(t_traceroute *data) {
	struct timeval	recvtimeout;
    if ((data->fd_icmp = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) == -1) {
		printf(strerror(errno));
        return (1);
	}

	recvtimeout.tv_sec = RECV_TIMEOUT_SEC;
	recvtimeout.tv_usec = 0;
	if (setsockopt(data->fd_icmp, SOL_SOCKET, SO_RCVTIMEO, &recvtimeout, sizeof(recvtimeout)) == -1) {
		printf(strerror(errno));
        return (1);
	}
    return (0);
}

int    init_udp_socket(t_traceroute *data) {

	if ((data->fd_udp = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
		printf(strerror(errno));
        return (1);
	}
    return (0);
}

void init_socket(t_traceroute *data) {
	if (init_icmp_socket(data)) {
        close(data->fd_icmp);
        exit(EXIT_FAILURE);
    }
	if (init_udp_socket(data)) {
        close(data->fd_icmp);
        close(data->fd_udp);
        exit(EXIT_FAILURE);
    }
}
