#include "ft_ping.h"

// This file should loop until SIGINT, it will send ping request to HOSTNAME and listen to reply, decode them 
// and fill the t_ping_stat structure so when the loop end it can display stats.

void fill_icmp(struct icmphdr *hdr)
{
    hdr->type = ECHO_REQUEST;
    hdr->checksum = 0;
    hdr->code = 0;
    hdr->un.echo.id = htons(getpid());
    hdr->un.echo.sequence = htons(0);
}

int ping_loop(t_ping_stats *ping_infos)
{
    struct icmphdr header; //this is the header of our message
    struct sockaddr_in dest, from;
    int raw_sock;
    int datasize;
    char payload[PAYLOAD_SIZE]; //this is the data of our message
    char recvbuf[1024];
    char buffer[64] = {0}; //this is the buffer that will concat the header + data and will be sent
    socklen_t fromlen = sizeof(from);
    //char ip_addr[INET_ADDRSTRLEN];

    struct timeval send_time;

    fill_icmp(&header);
    ping_infos->id = header.un.echo.id;    
    //Creating a raw socket for IPV4 and ICMP protocol
    if ((raw_sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) == -1)
    {
        fprintf(stderr, "Socket creation failed with error: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    
    //filling dest sockaddr struct so it can send the packet to the right place
    dest.sin_port = htons(PORT);
    dest.sin_family = AF_INET;
    
    if (resolve_hostname(ping_infos->hostname, ping_infos->ip_addr))
    { 
        fprintf(stderr, "%s: Name or service not known\n", ping_infos->hostname);
        close(raw_sock);
        exit(EXIT_FAILURE);
    }
    
    //converting address from text to binary so it can be used with sendto()
    if (inet_pton(AF_INET, ping_infos->ip_addr, &dest.sin_addr) <= 0)
    {
       fprintf(stderr, "Invalid address / address not supported\n"); 
       close(raw_sock);
       exit(EXIT_FAILURE);
    }
     
    display_transmit(ping_infos);

    //In this loop I'm creating the packet to be sent, calculating the checksum, sending the packet
    //waiting for a message to be received and if so parsing it.
    //finally i'm incrementing the sequence so we can follow how many exchange has been done                                  
    while (g_running)
    {
        header.checksum = 0;
        memset(payload, 0, sizeof(payload));
        //printf("checksum: %d\n", header.checksum);
        datasize = sizeof(struct icmphdr) + PAYLOAD_SIZE;
        //printf("datasize = %d\n", datasize);
        
        memcpy(buffer, &header, sizeof(struct icmphdr));
        memcpy(buffer + sizeof(struct icmphdr), payload, sizeof(payload));

        header.checksum = checksum((void *)buffer, datasize);
        *(unsigned short *)(buffer + 2) = header.checksum;
        
        gettimeofday(&send_time, NULL);
        sendto(raw_sock, buffer, datasize, 0, (struct sockaddr *)&dest, sizeof(dest));
        sleep(1); //THIS MAY BE USED TO DO -f BONUS
        ping_infos->send_time = &send_time;
        ping_infos->pckt_tx++;
        if ((ping_infos->recv_size = recvfrom(raw_sock, recvbuf, sizeof(recvbuf), 0, (struct sockaddr *)&from, &fromlen)) == -1)
        {
            fprintf(stderr, "Error receiving on raw socket with error: %s\n", strerror(errno));
            return (-1);
        }
        else if (ping_infos->recv_size > 0)
        {
            parse_reply(recvbuf, ping_infos);
            display_received(ping_infos);
            if (gptr_ping_options->verbose && ping_infos->icmp_type != 0)
                display_verbose_err((struct iphdr *)recvbuf, ping_infos);
        }
        //printf("seq: %d\n", header.un.echo.sequence);
        ping_infos->icmp_sequence = ++header.un.echo.sequence;
    }

    return (1);
}
