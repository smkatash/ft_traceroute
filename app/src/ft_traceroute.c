#include "ft_traceroute.h"

void    init_traceroute(t_traceroute *data) {
    data->ttl = 1;
    data->id = getpid();
    init_socket(data);
}

void    send_udp_probe(t_traceroute *data) {
    char packet[DEFAULT_PAYLOAD_SIZE];
    char *pdata = "Hello!";
    memcpy(packet, pdata, strlen(pdata));
    ssize_t bytes_sent = sendto(data->fd_udp, packet, sizeof(packet),
            0, (struct sockaddr *)&data->destination.inet_addr, sizeof(data->destination.inet_addr));
    if (bytes_sent < 0) {
        log_error(strerror(errno));
    }
}

int    get_response_type(void *msg, t_traceroute *data) {
    struct iphdr *iphdr;
    struct icmphdr *icmp;
    iphdr = (struct iphdr *)msg;

    if (iphdr->protocol == IPPROTO_ICMP) {
        icmp = (struct icmphdr *)(msg + (iphdr->ihl * 4));

        struct iphdr *orig_iphdr = (struct iphdr *)((char *)icmp + sizeof(struct icmphdr));
        if (memcmp(&orig_iphdr->daddr, &data->destination.inet_addr.sin_addr.s_addr, sizeof(orig_iphdr->daddr)) == 0) {
            if (orig_iphdr->protocol == IPPROTO_UDP) {
                struct udphdr *udp = (struct udphdr *)((char *)orig_iphdr + (orig_iphdr->ihl * 4));
                if (memcmp(&udp->uh_dport, &data->sent_port, sizeof(udp->uh_dport)) == 0) {
                    if (icmp->type == ICMP_TIME_EXCEEDED) {
                        return (VALID_RESPONSE);
                    } else if (icmp->type == ICMP_DEST_UNREACH) {
                        return (DESTINATION_REACHED);
                    }
                }
            }
            return (INVALID_RESPONSE);
        }
    }
    return (INVALID_RESPONSE);
}

int receive_icmp_packet(void *msg, size_t msglen, int sockfd)
{
    ssize_t ret;

	if ((ret = recvfrom(sockfd, (void*)msg, msglen, 0, NULL, NULL)) == -1)
	{
		//if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR) {
		if (errno == 11 || errno == EWOULDBLOCK  || errno == EINTR)
		{
            dprintf(STDERR_FILENO, strerror(errno));
            dprintf(STDERR_FILENO, "\n");
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

void    ft_traceroute(t_traceroute *data) {
    unsigned char	msg[MAX_PACKET_SIZE];
    struct timeval  timeout;
    struct timeval  tv;
    fd_set          rfds;
    int             ports;


    ports = 0;
    init_traceroute(data);
    log_header(data);
    signal(SIGINT, &sighandler);
    for (int hop = 1; hop <= DEFAULT_MAX_HOPS; hop++) {
        if (setsockopt(data->fd_udp, IPPROTO_IP, IP_TTL, &hop, sizeof(hop)) < 0) {
            log_error(strerror(errno));
        }
        log_hop(hop);
        for (int probe = 0; probe < DEFAULT_MAX_PROBES; probe++) {
            data->destination.inet_addr.sin_port = htons(DEFAULT_HIGH_PORT + hop + ports + probe);
            data->sent_port = data->destination.inet_addr.sin_port;
            if (gettimeofday(&tv, NULL) == -1) {
                log_error(strerror(errno));
            }
            send_udp_probe(data);
            FD_ZERO(&rfds);
            FD_SET(data->fd_icmp, &rfds);
            timeout.tv_sec = 2;
            timeout.tv_usec = 0;
            int fds = select(data->fd_icmp + 1, &rfds, NULL, NULL, &timeout);
            if (fds == -1) {
                log_error(strerror(errno));
            } else if (fds == 0) {
                dprintf(STDIN_FILENO, " *");
            } else {
                if (FD_ISSET(data->fd_icmp, &rfds)) {
                    if (!receive_icmp_packet(msg, sizeof(msg), data->fd_icmp)) {
                        struct timeval  tv_end;
                        gettimeofday(&tv_end, NULL);
                        switch (get_response_type(msg, data))
                        {
                        case VALID_RESPONSE:
                            if (probe == 0) {
                                log_source_address(msg);
                            }
                            print_time_diff(&tv, &tv_end);
                            break;
                        case DESTINATION_REACHED:
                            if (probe == 0) {
                                log_source_address(msg);
                            }
                            print_time_diff(&tv, &tv_end);
                            if (probe == DEFAULT_MAX_PROBES - 1) {
                                dprintf(STDIN_FILENO, "\n");
                                return ;
                            }
                            break;
                        default:
                            break;
                        }
                    }   
                }
            }
        }
        dprintf(STDIN_FILENO, "\n");
        ports += 2;
    }
}