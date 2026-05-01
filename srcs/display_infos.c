#include "ft_ping.h"
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
 *      "PING domain_name (ip_address): PAYLOAD_SIZE data bytes" 
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

void display_transmit(t_ping_stats *ping_infos)
{
    printf("PING %s (%s): %d data bytes", ping_infos->hostname, ping_infos->ip_addr, PAYLOAD_SIZE);  
    if (gptr_ping_options->verbose)
        printf(", id 0x%x = %d", ping_infos->id, ping_infos->id);
    printf("\n");
}

void display_received(t_ping_stats *ping_infos)
{
    printf("%d bytes from %s: ", ping_infos->recv_size, ping_infos->ip_addr);
    switch (ping_infos->icmp_type)
    {
        case ECHO_REPLY:
            printf("icmp_seq=%d ttl=%d time=%.3f ms\n", ping_infos->icmp_sequence, ping_infos->ttl, ping_infos->timestamp);
            break;
        case DEST_UNREACH:
            printf("%s\n", unreach_msg[ping_infos->icmp_code]);
            break;
        case TTL_EXCEED:
            printf("%s\n", exceeded_msg[ping_infos->icmp_code]);
            break;
        case REDIRECT:
            printf("%s\n", redirect_msg[ping_infos->icmp_code]);
            break;
        case PRM_PROBLEM:
            printf("%s\n", prm_problem_msg[ping_infos->icmp_code]);
            break;
        default:
            printf("Type %d message received\n", ping_infos->icmp_type);
            break;
    };
}

void display_stats(t_ping_stats *ping_infos)
{
    float error_rate = 0;
    double time_avg = 0;
    double time_dev = 0;
    
    if (ping_infos->pckt_rx)
    {
        printf("%f\n", ping_infos->time_total);
        time_avg = ping_infos->time_total / ping_infos->pckt_rx;
        time_dev = sqrt(ping_infos->ltime_total - time_avg * time_avg); //calculation of direct standard derivation
        time_dev /= ping_infos->pckt_rx;
        error_rate = fabs(((float)ping_infos->pckt_rx / (float)ping_infos->pckt_tx) * 100.0 - 100.0);
    }
    
    printf("--- %s ping statistics ---\n", ping_infos->hostname);
    printf("%d packets transmitted, %d packets received, %.0f%% packet loss\n", ping_infos->pckt_tx, ping_infos->pckt_rx, error_rate); //here I have to actually calculate the percentage of packet loss
    printf("round-trip min/avg/max/stddev = %.3f/%.3f/%.3f/%.3f ms\n", ping_infos->time_min, time_avg, ping_infos->time_max, time_dev);
}

void display_verbose_err(struct iphdr *ip, t_ping_stats *ping_infos)
{
    unsigned int *buf = (unsigned int *)ip;
    
    char src_addr[INET_ADDRSTRLEN];
    char dst_addr[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(ip->saddr), src_addr, INET_ADDRSTRLEN);
    inet_ntop(AF_INET, &(ip->daddr), dst_addr, INET_ADDRSTRLEN);
    //To run inside send_request file just after the display_received function
    //list of whats needed to match the inetutils ping
    printf("IP Hdr Dump:\n");

    for (int i=0; i < 10; i++) 
    {
        printf("%04x ", ntohs(*(buf + i)));
    }
    printf("\nVr HL TOS  Len   ID Flg  off TTL Pro cks      Src    Dest    Data\n");
    printf(" %d  %d  %02x %04x %04x   %d %04x  %02x  %02x %04x %s  %s\n",
            ip->version,
            ip->ihl,
            ip->tos,
            ntohs(ip->tot_len),
            ntohs(ip->id),
            ntohs(ip->frag_off) >> 13,
            ntohs(ip->frag_off) & 0x1fff,
            ip->ttl,
            ip->protocol,
            ntohs(ip->check),
            src_addr,
            dst_addr);
            
    printf("ICMP: type %d, code %d, size %d, id 0x%04x, seq 0x%04x\n",
            ping_infos->icmp_type,
            ping_infos->icmp_code,
            ntohs(ip->tot_len) - (ip->ihl * 4),
            ping_infos->id,
            ping_infos->icmp_sequence);
}
