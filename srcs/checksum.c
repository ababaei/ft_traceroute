#include "ft_ping.h"

// This checksum algorithm is describde in the ICMP RFC 

unsigned short checksum(void *b, int size)
{
    unsigned int cksum = 0;
    unsigned short *buf = b;

    while (size > 1)
    {
        cksum += *buf++;
        size -= sizeof(unsigned short);
    }
    
    if (size == 1)
    {
        cksum += *(unsigned char*)buf;
    }

    cksum = (cksum >> 16) + (cksum & 0xFFFF);
    cksum += (cksum >> 16);

    return (~cksum);
}
