#include "ft_trace.h"
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
t_trace_stats *gptr_trace_infos = NULL;
t_trace_options *gptr_trace_options = NULL;

void init_infos(t_trace_stats *trace_infos)
{
    memset(trace_infos->ip_addr, 0, INET_ADDRSTRLEN);
    trace_infos->recv_size = 0;
    trace_infos->timestamp = 0;
    trace_infos->ttl = 1;
    trace_infos->icmp_sequence = 0;

    trace_infos->max_hops = 30;
}

int main(int argc, char **argv)
{
    int opt;
    t_trace_options trace_options;
    t_trace_stats trace_infos;
    gptr_trace_infos = &trace_infos;
    gptr_trace_options = &trace_options;
    trace_options.verbose = 0;
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
                trace_options.verbose = 1;
                break;
            case '?':
                fprintf(stderr, "Usage: %s [-v] hostname\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    init_infos(&trace_infos);
    if ((trace_infos.hostname = strdup(argv[optind])) == NULL)
    {
        fprintf(stderr, "Error copying hostname: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    if ((ret = trace_loop(&trace_infos)) == -1)
    {
        fprintf(stderr, "Exiting with error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    if (trace_options.verbose == 1)
        printf("VERBOSE!\n");

    return (ret);
}
