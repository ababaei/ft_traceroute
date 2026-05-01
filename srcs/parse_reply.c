#include "ft_ping.h"

/**********************************
 *
 * This function should be used to parle a reply after an ICMP request
 * to do so I am casting the buffer into a icmphdr struct so the data are easily accessible
 * Then depending on the type I can choose what to do
 *
 *  ECHO_REPLY 0
 *  DEST_UNREACH 3
 *  SRC_QUENCH 4
 *  REDIRECT 5
 *  TTL_EXCEED 11
 *  PRM_PROBLEM 12
 *  TIMESTAMP_REPLY 14 //BONUS
 **********************************/

void parse_reply(char *buf, t_ping_stats *ping_infos)
{
    struct icmphdr *icmp_ptr;
    struct iphdr *ip = (struct iphdr *)buf;

    icmp_ptr = (struct icmphdr *)(buf + (ip->ihl * 4));  
    
    if (ping_infos->recv_size == 0)
        printf("ERROR\n");
    
    if (icmp_ptr->un.echo.id != htons(getpid()))
    {
        printf("Not our packet !\n");
        return;
    }
    ping_infos->icmp_type = icmp_ptr->type;

    if (ping_infos->icmp_type == ECHO_REPLY)
        handle_echo_reply(ping_infos);
    else if (ping_infos->icmp_type == TIMESTAMP_REPLY)
        handle_timestamp_reply(icmp_ptr, ping_infos);
    else
    {
        ping_infos->pckt_rx++;
        ping_infos->error_nb++;
    }

    /*
    switch(icmp_ptr->type)
    {
        case ECHO_REPLY:
            handle_echo_reply(ping_infos);
            break;
        case DEST_UNREACH:
            handle_unreachable(icmp_ptr, ping_infos);
            break;
        case SRC_QUENCH:
            handle_src_quench(icmp_ptr, ping_infos);
            break;
        case REDIRECT:
            handle_redirect(icmp_ptr, ping_infos);
            break;
        case TTL_EXCEED:
            handle_ttl_exceeded(icmp_ptr,ping_infos);
            break;
        case PRM_PROBLEM:
            handle_prm_problem(icmp_ptr, ping_infos);
            break;
        case TIMESTAMP_REPLY:
            handle_timestamp_reply(icmp_ptr, ping_infos);
            break;
        default :
    }
*/
}
