#include "ft_trace.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <stdio.h>
#include <stdlib.h>

/**********************************************************
 *
 * This file should contains the functions used to display three part of the reference output:
 *
 *  -display_transmit(...) should contains the first line composed of:
 *      "trace domain_name (ip_address): PAYLOAD_SIZE data bytes" 
 *  so it uses 3 variable named hostname, ip_addr and PAYLOAD_SIZE.
 *
 *  -display_received(...) should run each time an ECHO_REPLY message is received
 *      "datasize bytes from ip_addr: icmp_seq=n ttl=n time=n ms"
 *
 *  -display_stats(...) should run at the end of the program and display
 *
 *  Since the start I've added the display_verbose_err function for the verbose option. 
 ***********************************************************/

static const char *unreach_msg[6] =
{
    "Destination Net Unreachable",
    "Destination Host Unreachable",
    "Destination Protocol Unreachable",
    "Destination Port Unreachable",
    "Fragmentation needed and DF set",
    "Source route failed"
};

static const char *exceeded_msg[2] =
{
    "Time to live exceeded",
    "Frag reassembly time exceeded"
};

static const char *prm_problem_msg[1] =
{
    "Parameter problem"
};

static const char *redirect_msg[4] =
{
    "Redirect Network",
    "Redirect Host",
    "Redirect Type of Service and Network",
    "Redirect Type of Service and Host"
};

void display_transmit(t_trace_stats *trace_infos)
{
    printf("traceroute to %s (%s), %d hops max, %d byte packets",
           trace_infos->hostname, 
           trace_infos->ip_addr, 
           trace_infos->max_hops,
           PAYLOAD_SIZE);  
    printf("\n");
}

void display_received(t_trace_stats *trace_infos)
{
    printf("%d bytes from %s: ", trace_infos->recv_size, trace_infos->ip_addr);
    switch (trace_infos->icmp_type)
    {
        case ECHO_REPLY:
            printf("icmp_seq=%d ttl=%d time=%.3f ms\n", trace_infos->icmp_sequence, trace_infos->ttl, trace_infos->timestamp);
            break;
        case DEST_UNREACH:
            printf("%s\n", unreach_msg[trace_infos->icmp_code]);
            break;
        case TTL_EXCEED:
            printf("%s\n", exceeded_msg[trace_infos->icmp_code]);
            break;
        case REDIRECT:
            printf("%s\n", redirect_msg[trace_infos->icmp_code]);
            break;
        case PRM_PROBLEM:
            printf("%s\n", prm_problem_msg[trace_infos->icmp_code]);
            break;
        default:
            printf("Type %d message received\n", trace_infos->icmp_type);
            break;
    };
}

void display_stats(t_trace_stats *trace_infos)
{
    float error_rate = 0;
    double time_avg = 0;
    double time_dev = 0;
    
    if (trace_infos->pckt_rx)
    {
        printf("%f\n", trace_infos->time_total);
        time_avg = trace_infos->time_total / trace_infos->pckt_rx;
        time_dev = sqrt(trace_infos->ltime_total - time_avg * time_avg); //calculation of direct standard derivation
        time_dev /= trace_infos->pckt_rx;
        error_rate = fabs(((float)trace_infos->pckt_rx / (float)trace_infos->pckt_tx) * 100.0 - 100.0);
    }
    
    printf("--- %s trace statistics ---\n", trace_infos->hostname);
    printf("%d packets transmitted, %d packets received, %.0f%% packet loss\n", trace_infos->pckt_tx, trace_infos->pckt_rx, error_rate); //here I have to actually calculate the percentage of packet loss
    printf("round-trip min/avg/max/stddev = %.3f/%.3f/%.3f/%.3f ms\n", trace_infos->time_min, time_avg, trace_infos->time_max, time_dev);
}
