#include "ft_trace.h"
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>

// This is the main logic file, here the trace probe will be sent and will wait
// for a response for a given duration

void fill_icmp(struct icmphdr *hdr) {
  hdr->type = ECHO_REQUEST;
  hdr->checksum = 0;
  hdr->code = 0;
  hdr->un.echo.id = htons(getpid());
  hdr->un.echo.sequence = htons(0);
}

int trace_loop(t_trace_stats *trace_infos) {
  struct icmphdr header; // this is the header of our message
  struct sockaddr_in dest, from;
  int raw_icmp, sock_udp;
  int datasize;
  int current_ttl = 1;
  // char payload[PAYLOAD_SIZE]; // this is the data of our message
  char recvbuf[1024];
  char buffer[64] = {0}; // this is the buffer that will concat the header +
                         // data and will be sent
  socklen_t fromlen = sizeof(from);
  // char ip_addr[INET_ADDRSTRLEN];

  struct timeval send_time[3];
  struct timeval rec_time[3];

  struct timeval tv;
  tv.tv_sec = 3;
  tv.tv_usec = 0;

  fill_icmp(&header);
  trace_infos->id = header.un.echo.id;
  // Creating a raw socket for IPV4 and ICMP protocol
  if ((raw_icmp = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) == -1) {
    fprintf(stderr, "Socket creation failed with error: %s\n", strerror(errno));
    exit(EXIT_FAILURE);
  }
  setsockopt(raw_icmp, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);

  //creating a raw socket for IPV4 and UDP protocol
  if ((sock_udp = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
      fprintf(stderr, "Socket creation failed with error: %s\n", strerror(errno));
      exit(EXIT_FAILURE);
  }


  // filling dest sockaddr struct so it can send the packet to the right place
  dest.sin_port = htons(PORT);
  dest.sin_family = AF_INET;

  if (resolve_hostname(trace_infos->hostname, trace_infos->ip_addr)) {
    fprintf(stderr, "%s: Name or service not known\n", trace_infos->hostname);
    close(raw_icmp);
    exit(EXIT_FAILURE);
  }

  // converting address from text to binary so it can be used with sendto()
  if (inet_pton(AF_INET, trace_infos->ip_addr, &dest.sin_addr) <= 0) {
    fprintf(stderr, "Invalid address / address not supported\n");
    close(raw_icmp);
    exit(EXIT_FAILURE);
  }

  display_transmit(trace_infos);

  datasize = sizeof(struct icmphdr) + PAYLOAD_SIZE;

  int timeout = 0;
  while (current_ttl <= trace_infos->max_hops) {
      timeout = 0;
      if (setsockopt(sock_udp, IPPROTO_IP, IP_TTL, &current_ttl, sizeof(current_ttl)) < 0) {
          close(sock_udp);
          exit(EXIT_FAILURE);
      }

      for (int i = 0; i < 3; i++) {
          sendto(sock_udp, buffer, datasize, 0, (struct sockaddr *)&dest, sizeof(dest));
          gettimeofday(&send_time[i], NULL);
          //here maybe store send timestamp for each packets
      }

      //may have to wait for the 3 replies
      int replies = 0;
      while (replies < 3) {
          if ((trace_infos->recv_size = recvfrom(raw_icmp, recvbuf, sizeof(recvbuf), 0, (struct sockaddr *)&from, &fromlen)) == -1) {
              if (errno == EAGAIN) {
                  timeout = 1;
                  break;
              }
              fprintf(stderr, "Error receiving on raw socket with error: %s\n", strerror(errno));
              return (-1);
          } else if (trace_infos->recv_size > 0) {
              gettimeofday(&rec_time[replies], NULL);
              parse_reply(recvbuf, trace_infos);
              replies++;
          }
      }

      if (!timeout) {
          for (int i = 0; i < 3; i++) {
             trace_infos->probes_rtt[i] = time_diff(&send_time[i], &rec_time[i]);
          }
      }

      trace_infos->current_ttl = current_ttl;
      display_received(trace_infos, timeout);
      current_ttl++;
  }

//   while (g_running) {
//     for (int i = 0; i < 3; i++) {
//       header.checksum = 0;
//       memset(payload, 0, sizeof(payload));
//       // printf("checksum: %d\n", header.checksum);
//       datasize = sizeof(struct icmphdr) + PAYLOAD_SIZE;
//       // printf("datasize = %d\n", datasize);
//
//       memcpy(buffer, &header, sizeof(struct icmphdr));
//       memcpy(buffer + sizeof(struct icmphdr), payload, sizeof(payload));
//
//       header.checksum = checksum((void *)buffer, datasize);
//       *(unsigned short *)(buffer + 2) = header.checksum;
//
//       gettimeofday(&send_time, NULL);
//       sendto(raw_icmp, buffer, datasize, 0, (struct sockaddr *)&dest, sizeof(dest));
//       trace_infos->send_time = &send_time;
//       trace_infos->pckt_tx++;
//       // NEED TO USE SOME SORT OF TIMEOUT ON THE RECVFROM
//       if ((trace_infos->recv_size = recvfrom(raw_icmp, recvbuf, sizeof(recvbuf), 0, (struct sockaddr *)&from, &fromlen)) == -1) {
//         // CHANGE SO IT CONTINUE WHEN TIMEOUT REACH
//         fprintf(stderr, "Error receiving on raw socket with error: %s\n",
//                 strerror(errno));
//         return (-1);
//       } else if (trace_infos->recv_size > 0) {
//         gettimeofday(&rec_time, NULL);
//         trace_infos->rec_time = &rec_time;
//         // MAYBE NOT HERE FOR THE ASSIGNATION OF PROBE_RTT
//         trace_infos->probes_rtt[i] = time_diff(trace_infos->send_time, trace_infos->rec_time);
//         parse_reply(recvbuf, trace_infos);
//       }
//     }
//     // printf("seq: %d\n", header.un.echo.sequence);
//     trace_infos->icmp_sequence = ++header.un.echo.sequence;
//   }

  return (1);
}
