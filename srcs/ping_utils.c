#include "ft_ping.h"

double time_diff(struct timeval *send_time, struct timeval *rec_time)
{
    return((rec_time->tv_sec - send_time->tv_sec) + 1e-3 * (rec_time->tv_usec - send_time->tv_usec));  
}
