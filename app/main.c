#include "ft_traceroute.h"

int main(int argc, char **argv) {
    if (argc < 2) {
       log_message(USAGE_MESSAGE);
       exit(EXIT_SUCCESS);
    }
    t_traceroute    data;
    ft_memset(&data, 0, sizeof(t_traceroute));
    parse_input(argc, argv, &data);
    if (!ft_traceroute(&data)) {
        return (EXIT_FAILURE);
    }
    return (EXIT_SUCCESS);
}