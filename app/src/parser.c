#include "ft_traceroute.h"

bool	str_isdigit(char *str)
{
	for (size_t i = 0; i < ft_strlen(str); i++) {
		if (!ft_isdigit(str[i])) {
			return (false);
		}
	}
	return (true);
}

int	parse_number(char *str) {
	int	num;

	if (!str_isdigit(str)) {
		dprintf(STDERR_FILENO, "%s invalid numeric argument: '%s'\n", PROG_NAME, str);
		exit(EXIT_FAILURE);
	}
	num = ft_atoi(str);
	if (num <= 0) {
		dprintf(STDERR_FILENO, "%s invalid argument: '%s' Numerical result out of range\n", PROG_NAME, str);
		exit(EXIT_FAILURE);
	}
	return num;
}

void    parse_hostaddress(char *host, t_host *dest) {
    struct addrinfo	hints;
	struct addrinfo	*res;
	int				ret;

	ft_memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET,
	hints.ai_socktype = SOCK_RAW;
	hints.ai_protocol = IPPROTO_ICMP;
	hints.ai_flags = AI_CANONNAME;
	ret = getaddrinfo(host, NULL, &hints, &res);
    if (ret != 0) {
        log_error(gai_strerror(ret));
        exit(EXIT_FAILURE);
    }
	dest->hostname = ft_strdup(host);
    ft_memcpy(&dest->inet_addr, res->ai_addr, sizeof(struct sockaddr));
	inet_ntop(AF_INET, &(dest->inet_addr.sin_addr), dest->ip_addr, INET_ADDRSTRLEN);
	freeaddrinfo(res);
}

void	parse_options(char *option, char *option_value, t_traceroute *trace) {
	if (ft_strcmp(option, OPTION_QUERY) == 0) {
		if (!option_value) {
			log_error(OPTION_QUERY_ERROR);
			clean_up(trace);
			exit(EXIT_FAILURE);
		}
		trace->options.max_queries = parse_number(option_value);
	} else if (ft_strcmp(option, OPTION_PORT) == 0) {
		if (!option_value) {
			log_error(OPTION_PORT_ERROR);
			clean_up(trace);
			exit(EXIT_FAILURE);
		}
		trace->options.port = parse_number(option_value);
	} else if (ft_strcmp(option, OPTION_TTL) == 0) {
		if (!option_value) {
			log_error(OPTION_TTL_ERROR);
			clean_up(trace);
			exit(EXIT_FAILURE);
		}
		trace->options.ttl = parse_number(option_value);
	} else {
		log_error("Bad option.");
		clean_up(trace);
		exit(EXIT_FAILURE);
	}
}

void    parse_input(int argc, char **argv, t_traceroute *trace)
{
	trace->options.ttl = DEFAULT_MAX_HOPS;
	trace->options.max_queries = DEFAULT_MAX_PROBES;
	trace->options.port = DEFAULT_HIGH_PORT;
	int	i = 1;
	char *str = NULL;
	while (i < argc) {
		if (argv[i] && argv[i][0] == '-') {
			if (argv[i + 1]) {
				str = argv[i + 1];
			}
			parse_options(argv[i], str, trace);
			i++;
		} else {
			if (!trace->destination.hostname) {
				parse_hostaddress(argv[i], &(trace->destination));
			} else if (str_isdigit(argv[i]) && !trace->packet_len) {
				trace->packet_len = parse_number(argv[i]);
			} else {
				log_error("Extra arg.");
				clean_up(trace);
				exit(EXIT_FAILURE);
			}
		}
		i++;
		str = NULL;
	}
	trace->packet_len = trace->packet_len > 0 ? trace->packet_len : DEFAULT_UDP_MAX_PACKET_SIZE;
}