#include "ft_traceroute.h"

static void    send_udp_probe(t_traceroute *data) {
    char packet[DEFAULT_PAYLOAD_SIZE];
    char *pdata = "Hello!";
    ft_memcpy(packet, pdata, strlen(pdata));
    ssize_t bytes_sent = sendto(data->fd_udp, packet, sizeof(packet),
            0, (struct sockaddr *)&data->destination.inet_addr, sizeof(data->destination.inet_addr));
    if (bytes_sent < 0) {
        log_error(strerror(errno));
    }
}

static int bind_socket(t_traceroute *data) {
    struct sockaddr_in source_addr;
    memset(&source_addr, 0, sizeof(source_addr));
    source_addr.sin_family = AF_INET;
    source_addr.sin_addr.s_addr = INADDR_ANY;
    source_addr.sin_port = htons((unsigned short)(data->id | 0x8000));

    if (bind(data->fd_udp, (struct sockaddr *)&source_addr, sizeof(source_addr)) < 0) {
        log_error(strerror(errno));
        return (-1);
    }
    return (0);
}

static int    get_response_type(void *msg, t_traceroute *data) {
    struct iphdr *iphdr;
    struct icmphdr *icmp;
    iphdr = (struct iphdr *)msg;

    if (iphdr->protocol != IPPROTO_ICMP) {
        return INVALID_RESPONSE;
    }

    icmp = (struct icmphdr *)(msg + (iphdr->ihl * 4));
    struct iphdr *orig_iphdr = (struct iphdr *)((char *)icmp + sizeof(struct icmphdr));
    if (ft_memcmp(&orig_iphdr->daddr, &data->destination.inet_addr.sin_addr.s_addr, sizeof(orig_iphdr->daddr))) {
        return INVALID_RESPONSE;
    }
    if (orig_iphdr->protocol != IPPROTO_UDP) {
        return INVALID_RESPONSE;
    }
    struct udphdr *udp = (struct udphdr *)((char *)orig_iphdr + (orig_iphdr->ihl * 4));
    unsigned short expected_source_port = htons((data->id | 0x8000));
    if (ft_memcmp(&udp->uh_dport, &data->dest_port, sizeof(udp->uh_dport)) == 0 &&
        ft_memcmp(&udp->uh_sport, &expected_source_port, sizeof(udp->uh_dport)) == 0) {
        if (icmp->type == ICMP_TIME_EXCEEDED) {
            return VALID_RESPONSE;
        } else if (icmp->type == ICMP_DEST_UNREACH) {
            return DESTINATION_REACHED;
        }
    }

    return INVALID_RESPONSE;
}

static int receive_icmp_packet(void *msg, size_t msglen, int sockfd) {
    ssize_t ret;

	if ((ret = recvfrom(sockfd, (void*)msg, msglen, 0, NULL, NULL)) == -1)
	{
		//if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR) {
		if (errno == 11 || errno == EWOULDBLOCK  || errno == EINTR) {
            log_error(strerror(errno));
		}
        return (1);
	}
    return (0);
}

static void	sighandler(int sig)
{
	if (sig == SIGINT) {
        exit(128 + sig);
	}
}

int    ft_traceroute(t_traceroute *data) {
    unsigned char	msg[MAX_PACKET_SIZE];
    struct timeval  tv;
    struct timeval  tv_end;
    int             prev_port_num;
    int             destination_reached;
    int             fds;

    destination_reached = 0;
    fds = 0;
    prev_port_num = 0;
    init_socket(data);
    if (bind_socket(data) == -1) {
        clean_up(data);
        return (0);
    }
    log_header(data);
    signal(SIGINT, &sighandler);
    for (int hop = 1; hop <= DEFAULT_MAX_HOPS; hop++) {
        if (setsockopt(data->fd_udp, IPPROTO_IP, IP_TTL, &hop, sizeof(hop)) < 0) {
            log_error(strerror(errno));
            close(fds);
            clean_up(data);
            return (0);
        }
        log_hop(hop);
        struct timeval timeout = {SHORT_TIMEOUT_SEC, SHORT_TIMEOUT_USEC};
        fd_set  rfds;
        for (int probe = 0; probe < DEFAULT_MAX_PROBES; probe++) {
            data->destination.inet_addr.sin_port = htons(DEFAULT_HIGH_PORT + hop + prev_port_num + probe);
            data->dest_port = data->destination.inet_addr.sin_port;
            if (gettimeofday(&tv, NULL) == -1) {
                log_error(strerror(errno));
                close(fds);
                clean_up(data);
                return (0);
            }
            send_udp_probe(data);
            FD_ZERO(&rfds);
            FD_SET(data->fd_icmp, &rfds);
            fds = select(data->fd_icmp + 1, &rfds, NULL, NULL, &timeout);
            if (fds == -1) {
                log_error(strerror(errno));
                close(fds);
                clean_up(data);
                return (0);
            } else if (fds == 0) {
                dprintf(STDOUT_FILENO, " *");
            } else if (FD_ISSET(data->fd_icmp, &rfds)) {
                if (!receive_icmp_packet(msg, sizeof(msg), data->fd_icmp)) {
                    gettimeofday(&tv_end, NULL);
                    switch (get_response_type(msg, data)) {
                    case VALID_RESPONSE:
                        if (probe == 0) log_source_address(msg);
                        print_time_diff(&tv, &tv_end);
                        break;
                    case DESTINATION_REACHED:
                        if (probe == 0) log_source_address(msg);
                        print_time_diff(&tv, &tv_end);
                        destination_reached = 1;
                        break;
                    default:
                    }
                }
            }
        }
        dprintf(STDOUT_FILENO, "\n");
        prev_port_num += 2;
        if (destination_reached) {
            break;
        }
    }
    clean_up(data);
    return (1);
}