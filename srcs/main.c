#include "ft_ping.h"
#include <netinet/in.h>
#include <string.h>

/*****************************************************
 *
 * Main function file, take one argument as the hostname.
 * The idea is to first resolve the domain name to have an IP address
 * then we can create the socket and bind it.
 * With our ICMP packet prepared we can then send it over in a loop, and
 * read for replies. After analyzing a reply we can display the result and fill the stats
 *
 * Here we are only checking the arg, the options and running the loop if everything is ok
 *****************************************************/

int g_running = 1;
t_ping_stats *gptr_ping_infos = NULL;
t_ping_options *gptr_ping_options = NULL;

void init_infos(t_ping_stats *ping_infos)
{
    memset(ping_infos->ip_addr, 0, INET_ADDRSTRLEN);
    ping_infos->recv_size = 0;
    ping_infos->timestamp = 0;
    ping_infos->ttl = 0;
    ping_infos->icmp_sequence = 0;
}

int main(int argc, char **argv)
{
    int opt;
    t_ping_options ping_options;
    t_ping_stats ping_infos;
    gptr_ping_infos = &ping_infos;
    gptr_ping_options = &ping_options;
    ping_options.verbose = 0;
    int ret;

    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s [-v] hostname\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    signal(SIGINT, handle_sigint);

    while ((opt = getopt(argc, argv, "v?")) != -1)
    {
        switch (opt)
        {
            case 'v':
                ping_options.verbose = 1;
                break;
            case '?':
                fprintf(stderr, "Usage: %s [-v] hostname\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    init_infos(&ping_infos);
    if ((ping_infos.hostname = strdup(argv[optind])) == NULL)
    {
        fprintf(stderr, "Error copying hostname: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    if ((ret = ping_loop(&ping_infos)) == -1)
    {
        fprintf(stderr, "Exiting with error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    if (ping_options.verbose == 1)
        printf("VERBOSE!\n");

    return (ret);
}
