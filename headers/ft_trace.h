#ifndef FT_trace_H
#define FT_trace_H

#include <arpa/inet.h>
#include <bits/types/struct_timeval.h>
#include <errno.h>
#include <math.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip_icmp.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <threads.h>
#include <unistd.h>

// ICMP MESSAGE TYPE
#define ECHO_REPLY 0
#define ECHO_REQUEST 8
#define DEST_UNREACH 3
#define SRC_QUENCH 4
#define REDIRECT 5
#define TTL_EXCEED 11
#define PRM_PROBLEM 12
#define TIMESTAMP_REPLY 14

#define IP_ADDR "127.0.0.1"
#define PORT 12345
#define PAYLOAD_SIZE 24

typedef struct s_trace_stats {
  char *hostname;                // domain or ip address given by the user
  char ip_addr[INET_ADDRSTRLEN]; // ip address of the destination
  int recv_size;                 // size of the rx message
  int ttl;
  struct timeval *send_time;
  struct timeval *rec_time;
  double probes_rtt[3];
  double timestamp;
  double time_max;
  double time_min;
  double time_total;
  long long ltime_total;

  unsigned short max_hops;
  unsigned short current_ttl;

  unsigned short icmp_sequence;
  unsigned int pckt_tx;  // nb of packet transmitted
  unsigned int pckt_rx;  // nb of packet received
  unsigned int error_nb; // nb of error

  // for error handling
  unsigned short icmp_type;
  unsigned short icmp_code;
  unsigned short id;
  char from_ip[INET_ADDRSTRLEN]; // ip address of the error emitter, not
                                 // necesserly the target.
} t_trace_stats;

typedef struct s_trace_options {
  int verbose;
} t_trace_options;

extern int g_running;
extern t_trace_stats *gptr_trace_infos;
extern t_trace_options *gptr_trace_options;

unsigned short checksum(void *b, int size);
void fill_icmp(struct icmphdr *packet);
int trace_loop(t_trace_stats *trace_infos);
int resolve_hostname(char *hostname, char *ip);

void parse_reply(char *buf, t_trace_stats *trace_infos);

void handle_echo_reply(t_trace_stats *trace_infos);
void handle_ttl_exceeded(t_trace_stats *trace_infos);

void init_infos(t_trace_stats *trace_infos);
double time_diff(struct timeval *send_time, struct timeval *rec_time);

void display_transmit(t_trace_stats *trace_infos);
void display_received(t_trace_stats *trace_infos, int timeout);
void display_stats(t_trace_stats *trace_infos);

void handle_sigint(int signum);

#endif
