#include "ft_traceroute.h"

void print_time_diff(struct timeval *tv_start, struct timeval *tv_end) {
    struct timeval tv_diff;
    tv_diff.tv_sec = tv_end->tv_sec - tv_start->tv_sec;
    tv_diff.tv_usec = tv_end->tv_usec - tv_start->tv_usec;
    if (tv_diff.tv_usec < 0) {
        tv_diff.tv_sec--;
        tv_diff.tv_usec += 1000000;
    }
    double milliseconds = (tv_diff.tv_sec * 1000.0) + (tv_diff.tv_usec / 1000.0);
    dprintf(STDOUT_FILENO," %.3f ms ", milliseconds);
}

void    clean_up(t_traceroute *data) {
    close(data->fd_icmp);
    close(data->fd_udp);
    free(data->destination.hostname);
}
