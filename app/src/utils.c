#include "ft_traceroute.h"

unsigned short checksum(void *header, int len)
{
	unsigned short *ptr = (unsigned short *)header;
	unsigned int sum = 0;
	unsigned short checksum;

	while (len > 1)
	{
		sum += *ptr++;
		len -= 2;
	}
	if (len == 1)
	{
		sum += *((unsigned char *)ptr);
	}
	sum = (sum >> 16) + (sum & 0xFFFF);
	sum += (sum >> 16);
	checksum = (unsigned short)~sum; 
	return (checksum);
}
