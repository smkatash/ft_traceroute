#include "ft_traceroute.h"

void    debug_input(t_traceroute *data) {
    printf("Hostname %s\n", data->destination.hostname);
    printf("Hostname %s\n", data->destination.ip_addr);
    printf("Packet len %zu\n", data->packet_len);
}

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Usage msg\n");
        exit(1);
    }
    t_traceroute    data;
    memset(&data, 0, sizeof(t_traceroute));
    parse_input(argc, argv, &data);
    //debug_input(&data);
    ft_traceroute(&data);
}