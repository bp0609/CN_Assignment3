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

#define INFINITY 999
extern float clocktime;
struct distance_table
{
    int costs[4][4];
} dt3;

/* students to write the following two routines, and maybe some others */
int connectcosts3[4] = {7, 999, 2, 0};
struct rtpkt pkt3[4];
int min_cost_3[4];

void calc_min_cost3()
{
    // calculate the minimum cost to every other node
    //  calculate the min cost for every other node
    for (int i = 0; i < 4; i++)
    {
        min_cost_3[i] = INFINITY;
        for (int j = 0; j < 4; j++)
        {
            if (dt3.costs[i][j] < min_cost_3[i])
            {
                min_cost_3[i] = dt3.costs[i][j];
            }
        }
    }
}

void sendpkt3()
{
    // make the pkts
    for (int i = 0; i < 4; i++)
    {
        pkt3[i].sourceid = 3;
        pkt3[i].destid = i; // to all 4 coz all are connected
        memcpy(pkt3[i].mincost, min_cost_3, sizeof(min_cost_3));
    }
    // SEND PKTS TO ALL NEIGHBOURS
    for (int i = 0; i < 4; i++)
    {
        if (i != 3 && connectcosts3[i] != INFINITY)
        {
            printf("At time t=%.3f, node %d sends packet to node %d with: (%d  %d  %d  %d)\n",
                   clocktime, pkt3[i].sourceid, pkt3[i].destid, pkt3[i].mincost[0], pkt3[i].mincost[1],
                   pkt3[i].mincost[2], pkt3[i].mincost[3]);
            tolayer2(pkt3[i]);
        }
    }
}

void rtinit3()
{
    printf("\nrtint3() is called at time t=:%0.3f\n", clocktime);

    // Initialize the distance table for the first time.
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            if (i == j)
            {
                dt3.costs[i][j] = connectcosts3[i];
            }
            else
            {
                dt3.costs[i][j] = INFINITY;
            }
        }
    }
    printf("Distance table for node 0 is initialized.\n");
    printdt3(&dt3);
    calc_min_cost3();
    sendpkt3();
}

void rtupdate3(rcvdpkt) struct rtpkt *rcvdpkt;

{
    int src = rcvdpkt->sourceid;
    int dst = rcvdpkt->destid;
    int dist_vec_rcvd[4];
    memcpy(dist_vec_rcvd, rcvdpkt->mincost, sizeof(rcvdpkt->mincost));
    printf("rtupdate3() is called at time t=: %0.3f as node %d sent a pkt with (%d  %d  %d  %d)\n", clocktime, src,
           dist_vec_rcvd[0], dist_vec_rcvd[1], dist_vec_rcvd[2], dist_vec_rcvd[3]);

    for (int i = 0; i < 4; i++)
    {
        int new_cost = connectcosts3[src] + dist_vec_rcvd[i];
        if (new_cost < INFINITY)
        {
            dt3.costs[i][src] = new_cost;
        }
        else
        {
            dt3.costs[i][src] = INFINITY;
        }
    }
    printf("At time %.3f, node 3's distance table updated.\n", clocktime);
    printdt3(&dt3);

    // Check if the min cost changed
    int old_min_cost[4];
    memcpy(old_min_cost, min_cost_3, sizeof(min_cost_3));
    int update = 0;
    calc_min_cost3();
    for (int i = 0; i < 4; i++)
    {
        if (old_min_cost[i] != min_cost_3[i])
        {
            update = 1;
            break;
        }
    }
    if (update == 1)
    {
        sendpkt3();
    }
    else
    {
        printf("Minimum cost didn't change. No new packets are sent\n");
    }
}

printdt3(dtptr) struct distance_table *dtptr;

{
    printf("             via     \n");
    printf("   D3 |    0     2 \n");
    printf("  ----|-----------\n");
    printf("     0|  %3d   %3d\n", dtptr->costs[0][0], dtptr->costs[0][2]);
    printf("dest 1|  %3d   %3d\n", dtptr->costs[1][0], dtptr->costs[1][2]);
    printf("     2|  %3d   %3d\n", dtptr->costs[2][0], dtptr->costs[2][2]);
}