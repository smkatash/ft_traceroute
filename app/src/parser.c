#include "ft_traceroute.h"

bool	str_isdigit(char *str)
{
	for (size_t i = 0; i < strlen(str); i++)
	{
		if (isdigit(str[i]))
		{
			return (false);
		}
	}
	return (true);
}

int	parse_number(char *str)
{
	int	num;

	if (!str_isdigit(str))
	{
		dprintf(STDERR_FILENO, "ft_traceroute invalid argument: '%s'", str);
		exit(EXIT_FAILURE);
	}
	num = atoi(str);
	if (num <= 0)
	{
		dprintf(STDERR_FILENO, "ft_traceroute invalid argument: '%s' Numerical result out of range\n", str);
		exit(EXIT_FAILURE);
	}
	return num;
}

void    parse_hostaddress(char *host, t_host *dest) {
    struct addrinfo	hints;
	struct addrinfo	*res;
	int				ret;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET,
	hints.ai_socktype = SOCK_RAW;
	hints.ai_protocol = IPPROTO_ICMP;
	hints.ai_flags = AI_CANONNAME;
	ret = getaddrinfo(host, NULL, &hints, &res);
    if (ret != 0) {
        printf((gai_strerror(ret)));
        exit(EXIT_FAILURE);
    }
	dest->hostname = strdup(host);
    memcpy(&dest->inet_addr, res->ai_addr, sizeof(struct sockaddr));
	if (strcmp(host, res->ai_canonname) != 0) {
		dest->canonname= strdup(res->ai_canonname);
	}
	inet_ntop(AF_INET, &(dest->inet_addr.sin_addr), dest->ip_addr, INET_ADDRSTRLEN);
	freeaddrinfo(res);
}

void    parse_input(int argc, char **argv, t_traceroute *trace)
{
    parse_hostaddress(argv[1], &(trace->destination));
	if (argc == 3) {
        // TODO check for - in the front
        trace->packet_len = parse_number(argv[argc - 1]);
    }
	//parse_options(argc,argv,p);
}