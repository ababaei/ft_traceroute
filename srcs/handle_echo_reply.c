#include "ft_ping.h"

//This file is exclusivly to handle the logic of the echo reply message.
//It must only:
//  -compute rtt
//  -update stats

void handle_echo_reply(t_ping_stats *ping_infos)
{
    struct timeval end;
    double rtt = 0;
    ping_infos->pckt_rx++;

    gettimeofday(&end, NULL);
    rtt = time_diff(ping_infos->send_time, &end);
    ping_infos->timestamp = rtt;
    ping_infos->time_total += rtt;
    //ping_infos->time_avg = ping_infos->time_total / ping_infos->pckt_rx;
    ping_infos->ltime_total += (long long)rtt * (long long)rtt; //for stddev calculation
    if (rtt > ping_infos->time_max) { ping_infos->time_max = rtt; }
    if (rtt < ping_infos->time_min) { ping_infos->time_min = rtt; }
}
