#include <stdio.h>
#include <string.h>
extern struct rtpkt
{
    int sourceid;   /* id of sending router sending this pkt */
    int destid;     /* id of router to which pkt being sent
                       (must be an immediate neighbor) */
    int mincost[4]; /* min cost to node 0 ... 3 */
};

extern int TRACE;
extern int YES;
extern int NO;
struct distance_table
{
    int costs[4][4];
} dt2;

#define INFINITY 999
extern float clocktime;
/* students to write the following two routines, and maybe some others */

int connectcosts2[4] = {3, 1, 0, 2};
struct rtpkt pkt2[4];
int min_cost_2[4];

void calc_min_cost2()
{
    // calculate the minimum cost to every other node
    //  calculate the min cost for every other node
    for (int i = 0; i < 4; i++)
    {
        min_cost_2[i] = INFINITY;
        for (int j = 0; j < 4; j++)
        {
            if (dt2.costs[i][j] < min_cost_2[i])
            {
                min_cost_2[i] = dt2.costs[i][j];
            }
        }
    }
}

void sendpkt2()
{
    // make the pkts
    for (int i = 0; i < 4; i++)
    {
        pkt2[i].sourceid = 2;
        pkt2[i].destid = i; // to all 4 coz all are connected
        memcpy(pkt2[i].mincost, min_cost_2, sizeof(min_cost_2));
    }
    // SEND PKTS TO ALL NEIGHBOURS
    for (int i = 0; i < 4; i++)
    {
        if (i != 2 && connectcosts2[i] != INFINITY)
        {
            printf("At time t=%.3f, node %d sends packet to node %d with: (%d  %d  %d  %d)\n",
                   clocktime, pkt2[i].sourceid, pkt2[i].destid, pkt2[i].mincost[0], pkt2[i].mincost[1],
                   pkt2[i].mincost[2], pkt2[i].mincost[3]);
            tolayer2(pkt2[i]);
        }
    }
}

void rtinit2()
{
    printf("\nrtint2() is called at time t=:%0.3f\n", clocktime);

    // Initialize the distance table for the first time.
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            if (i == j)
            {
                dt2.costs[i][j] = connectcosts2[i];
            }
            else
            {
                dt2.costs[i][j] = INFINITY;
            }
        }
    }
    printf("Distance table for node 0 is initialized.\n");
    printdt2(&dt2);
    calc_min_cost2();
    sendpkt2();
}

void rtupdate2(rcvdpkt) struct rtpkt *rcvdpkt;

{
    int src = rcvdpkt->sourceid;
    int dst = rcvdpkt->destid;
    int dist_vec_rcvd[4];
    memcpy(dist_vec_rcvd, rcvdpkt->mincost, sizeof(rcvdpkt->mincost));
    printf("rtupdate2() is called at time t=: %0.3f as node %d sent a pkt with (%d  %d  %d  %d)\n", clocktime, src,
           dist_vec_rcvd[0], dist_vec_rcvd[1], dist_vec_rcvd[2], dist_vec_rcvd[3]);

    for (int i = 0; i < 4; i++)
    {
        int new_cost = connectcosts2[src] + dist_vec_rcvd[i];
        if (new_cost < INFINITY)
        {
            dt2.costs[i][src] = new_cost;
        }
        else
        {
            dt2.costs[i][src] = INFINITY;
        }
    }
    printf("At time %.3f, node 2's distance table updated.\n", clocktime);
    printdt2(&dt2);

    // Check if the min cost changed
    int old_min_cost[4];
    memcpy(old_min_cost, min_cost_2, sizeof(min_cost_2));
    int update = 0;
    calc_min_cost2();
    for (int i = 0; i < 4; i++)
    {
        if (old_min_cost[i] != min_cost_2[i])
        {
            update = 1;
            break;
        }
    }
    if (update == 1)
    {
        sendpkt2();
    }
    else
    {
        printf("Minimum cost didn't change. No new packets are sent\n");
    }
}

printdt2(dtptr) struct distance_table *dtptr;

{
    printf("                via     \n");
    printf("   D2 |    0     1    3 \n");
    printf("  ----|-----------------\n");
    printf("     0|  %3d   %3d   %3d\n", dtptr->costs[0][0],
           dtptr->costs[0][1], dtptr->costs[0][3]);
    printf("dest 1|  %3d   %3d   %3d\n", dtptr->costs[1][0],
           dtptr->costs[1][1], dtptr->costs[1][3]);
    printf("     3|  %3d   %3d   %3d\n", dtptr->costs[3][0],
           dtptr->costs[3][1], dtptr->costs[3][3]);
}