#include "ft_trace.h"

//This is used so we can quit cleanly on CTRL+C
// gptr_trace_infos is eclared inside the main function
void handle_sigint(int signum)
{
    (void)signum;
    g_running = false;
    //printf("Here print stats\n");
    display_stats(gptr_trace_infos);

    exit(0);
}
