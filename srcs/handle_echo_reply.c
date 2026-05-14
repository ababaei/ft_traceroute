#include "ft_trace.h"

//This file is exclusivly to handle the logic of the echo reply message.
//It must only:
//  -compute rtt
//  -update stats

void handle_echo_reply(t_trace_stats *trace_infos)
{
    struct timeval end;
    double rtt = 0;
    trace_infos->pckt_rx++;

    gettimeofday(&end, NULL);
    rtt = time_diff(trace_infos->send_time, &end);
    trace_infos->timestamp = rtt;
    trace_infos->time_total += rtt;
    //trace_infos->time_avg = trace_infos->time_total / trace_infos->pckt_rx;
    trace_infos->ltime_total += (long long)rtt * (long long)rtt; //for stddev calculation
    if (rtt > trace_infos->time_max) { trace_infos->time_max = rtt; }
    if (rtt < trace_infos->time_min) { trace_infos->time_min = rtt; }
}
