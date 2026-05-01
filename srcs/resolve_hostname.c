#include "ft_ping.h"

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
