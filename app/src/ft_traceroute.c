#include "ft_traceroute.h"

void    init_traceroute(t_traceroute *data) {
    data->hops = DEFAULT_MAX_HOPS; // needed?
    data->ttl = 1;
    data->id = getpid();
}

void    send_udp_probe(t_traceroute *data) {
    char packet[DEFAULT_PAYLOAD_SIZE];
    struct udphdr udp;
    char *pdata = packet + sizeof(struct udphdr);

    memset(&udp, 0, sizeof(udp));
    udp.uh_dport = data->destination.inet_addr.sin_port;
    udp.uh_sport = htons(0);
    udp.uh_ulen = htons(sizeof(struct udphdr));
    udp.uh_sum =  htons(0);
    memcpy(packet, &udp, sizeof(udp));
    strcpy(pdata, "Hello!");
    printf("Setting Destination Port: %hu\n", ntohs(udp.uh_dport));
    printf("Setting Source Port: %hu\n", ntohs(udp.uh_sport));
    ssize_t bytes_sent = sendto(data->fd_udp, packet, sizeof(packet),
            0, (struct sockaddr *)&data->destination.inet_addr, sizeof(data->destination.inet_addr));
    if (bytes_sent < 0) {
        printf(strerror(errno));
        exit(EXIT_FAILURE);
    }
}

void log_response(void *msg) {
    struct iphdr *iphdr;
    struct icmphdr *icmp;
    iphdr = (struct iphdr *)msg;

    struct in_addr src_ip, dest_ip;
    src_ip.s_addr = iphdr->saddr;
    dest_ip.s_addr = iphdr->daddr;

    printf("IP Header:\n");
    printf("Version: %d\n", iphdr->version);
    printf("Header Length: %d (bytes)\n", iphdr->ihl * 4);
    printf("Total Length: %d\n", ntohs(iphdr->tot_len));
    printf("Identification: 0x%X\n", ntohs(iphdr->id));
    printf("TTL: %d\n", iphdr->ttl);
    printf("Protocol: %d\n", iphdr->protocol);
    printf("Source IP: %s\n", inet_ntoa(src_ip));
    printf("Destination IP: %s\n", inet_ntoa(dest_ip));
    printf("Checksum: 0x%X\n", ntohs(iphdr->check));

    // Log ICMP header if the protocol is ICMP (1)
    if (iphdr->protocol == IPPROTO_ICMP) {
        // Calculate the ICMP header position, which is after the IP header
        icmp = (struct icmphdr *)(msg + (iphdr->ihl * 4));

        printf("ICMP Header:\n");
        printf("ICMP Type: %d\n", icmp->type);
        if (icmp->type == ICMP_TIME_EXCEEDED) {
             printf("Time to lve expired !!!\n");
        }
        printf("ICMP Code: %d\n", icmp->code);
        printf("ICMP Checksum: 0x%X\n", ntohs(icmp->checksum));
        struct iphdr *orig_iphdr = (struct iphdr *)((char *)icmp + sizeof(struct icmphdr));
        printf("Original Source IP: %s\n", inet_ntoa(*(struct in_addr *)&orig_iphdr->saddr));
        printf("Original Destination IP: %s\n", inet_ntoa(*(struct in_addr *)&orig_iphdr->daddr));

        // The original UDP header starts after the original IP header
        struct udphdr *udp = (struct udphdr *)((char *)orig_iphdr + (orig_iphdr->ihl * 4));

        // Now you can log the UDP ports correctly
        printf("Original UDP Destination Port: %hu\n", ntohs(udp->uh_dport));
        printf("Original UDP Source Port: %hu\n", ntohs(udp->uh_sport));
    } else {
        printf("Non-ICMP packet received, protocol: %d\n", iphdr->protocol);
    }
}

void receive_icmp_packet(void *msg, size_t msglen, t_traceroute *data)
{
    ssize_t ret = 0;

	if ((ret = recvfrom(data->fd_icmp, (void*)msg, msglen, 0, NULL, NULL)) == -1)
	{
		//if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR) {
		if (errno == 11 || errno == EWOULDBLOCK  || errno == EINTR)
		{
			printf("ERROR: %d ", errno);
		}
        printf(strerror(errno));
		printf("\n");
        printf("Return %zu\n", ret);
	}
    log_response(msg);
}

void    ft_traceroute(t_traceroute *data) {
    unsigned char	msg[MAX_PACKET_SIZE];
    fd_set          rfds;
    int             port = 0;
    struct timeval timeout;
    init_traceroute(data);
    create_socket(data);


    for (int hop = 1; hop <= DEFAULT_MAX_HOPS; hop++) {
        printf("Hop %d\n", hop);
        if (setsockopt(data->fd_udp, IPPROTO_IP, IP_TTL, &hop, sizeof(hop)) < 0) {
            printf(strerror(errno));
            exit(EXIT_FAILURE);
        }
        for (int probe = 0; probe < DEFAULT_MAX_PROBES; probe++) {
            data->destination.inet_addr.sin_port = htons(DEFAULT_HIGH_PORT + hop + port + probe);
            send_udp_probe(data);
            FD_ZERO(&rfds);
			FD_SET(data->fd_icmp, &rfds);
            timeout.tv_sec = 2;
			timeout.tv_usec = 0;
            int fds = select(data->fd_icmp + 1, &rfds, NULL, NULL, &timeout);
            if (fds == -1) {
                perror("select");
				exit(EXIT_FAILURE);
            } else if (fds == 0) {
                printf(strerror(errno));
                printf(" * ");
            } else {
                if (FD_ISSET(data->fd_icmp, &rfds)) {
                    receive_icmp_packet(msg, sizeof(msg), data);
                }
            }
        }
        port += 2;
    }
}