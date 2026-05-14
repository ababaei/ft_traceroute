#include "ft_trace.h"

double time_diff(struct timeval *send_time, struct timeval *rec_time)
{
    return(((rec_time->tv_sec - send_time->tv_sec) * 1000.0) + ((rec_time->tv_usec - send_time->tv_usec) / 1000.0));  
}

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

int resolve_hostname(char *hostname, char *ip)
{
    struct addrinfo hints;
    struct addrinfo *result, *rp;
    struct sockaddr_in *ipv4;
    int status;
    
    //printf("host: %s\n", hostname);
    //printf("ip: %s\n", ip);
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    if ((status = getaddrinfo(hostname, NULL, &hints, &result)) != 0)
    {
       fprintf(stderr, "getaddrinfo with error: %s\n", gai_strerror(status)); 
       return (1);
    }
    
    for (rp = result; rp != NULL; rp = rp->ai_next)
    {
        ipv4 = (struct sockaddr_in *)rp->ai_addr;
        void *addr = &(ipv4->sin_addr);

        if (inet_ntop(AF_INET, addr, ip, INET_ADDRSTRLEN) != NULL)
        {
            freeaddrinfo(result);
            return (0);
        }
    }

    freeaddrinfo(result);
    return (1);
}
