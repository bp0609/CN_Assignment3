#include <stdio.h>
#include <string.h>
extern int TRACE;
extern int YES;
extern int NO;

#define INFINITY 999
extern struct rtpkt
{
    int sourceid;   /* id of sending router sending this pkt */
    int destid;     /* id of router to which pkt being sent
                        (must be an immediate neighbor) */
    int mincost[4]; /* min cost to node 0 ... 3     DISTANCE VECTOR*/
};
struct distance_table
{
    int costs[4][4];
} dt0;

/* students to write the following two routines, and maybe some others */
extern float clocktime;
int connectcosts0[4] = {0, 1, 3, 7};
struct rtpkt pkt0[4];
int min_cost_0[4]; // min_distance to each node from node 0 ie. DISTANCE VECTOR

void calc_min_cost0()
{
    // calculate the min cost for every other node
    for (int i = 0; i < 4; i++)
    {
        min_cost_0[i] = INFINITY;
        for (int j = 0; j < 4; j++)
        {
            if (dt0.costs[i][j] < min_cost_0[i])
            {
                min_cost_0[i] = dt0.costs[i][j];
            }
        }
    }
}

void sendpkt0()
{
    for (int i = 0; i < 4; i++)
    {
        pkt0[i].sourceid = 0;
        pkt0[i].destid = i;
        memcpy(pkt0[i].mincost, min_cost_0, sizeof(min_cost_0));
    }

    // SEND pkts to all neighbours
    for (int i = 0; i < 4; i++)
    {
        if (i != 0 && connectcosts0[i] != INFINITY)
        { // not sending self
            printf("At time t=%.3f, node %d sends packet to node %d with: (%d  %d  %d  %d)\n",
                   clocktime, pkt0[i].sourceid, pkt0[i].destid, pkt0[i].mincost[0], pkt0[i].mincost[1],
                   pkt0[i].mincost[2], pkt0[i].mincost[3]);
            tolayer2(pkt0[i]);
        }
    }
}

void rtinit0()
{
    printf("\nrtint0() is called at time t=:%0.3f\n", clocktime);

    // Initialize the distance table for the first time.
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            if (i == j)
            {
                dt0.costs[i][j] = connectcosts0[i];
            }
            else
            {
                dt0.costs[i][j] = INFINITY;
            }
        }
    }
    printf("Distance table for node 0 is initialized.\n");
    printdt0(&dt0);
    calc_min_cost0();
    sendpkt0();
}

void rtupdate0(rcvdpkt) struct rtpkt *rcvdpkt;
{
    int src = rcvdpkt->sourceid;
    int dst = rcvdpkt->destid;
    int dist_vec_rcvd[4];
    memcpy(dist_vec_rcvd, rcvdpkt->mincost, sizeof(rcvdpkt->mincost));
    printf("rtupdate0() is called at time t=: %0.3f as node %d sent a pkt with (%d  %d  %d  %d)\n", clocktime, src,
           dist_vec_rcvd[0], dist_vec_rcvd[1], dist_vec_rcvd[2], dist_vec_rcvd[3]);

    for (int i = 0; i < 4; i++)
    {
        int new_cost = connectcosts0[src] + dist_vec_rcvd[i];
        if (new_cost < INFINITY)
        {
            dt0.costs[i][src] = new_cost;
        }
        else
        {
            dt0.costs[i][src] = INFINITY;
        }
    }
    printf("At time %.3f, node 0's distance table updated.\n", clocktime);
    printdt0(&dt0);

    // Check if the min cost changed
    int old_min_cost[4];
    memcpy(old_min_cost, min_cost_0, sizeof(min_cost_0));
    int update = 0;
    calc_min_cost0();
    for (int i = 0; i < 4; i++)
    {
        if (old_min_cost[i] != min_cost_0[i])
        {
            update = 1;
            break;
        }
    }
    if (update == 1)
    {
        sendpkt0();
    }
    else
    {
        printf("Minimum cost didn't change. No new packets are sent\n");
    }
}

printdt0(dtptr) struct distance_table *dtptr;

{
    printf("                via     \n");
    printf("   D0 |    1     2    3 \n");
    printf("  ----|-----------------\n");
    printf("     1|  %3d   %3d   %3d\n", dtptr->costs[1][1],
           dtptr->costs[1][2], dtptr->costs[1][3]);
    printf("dest 2|  %3d   %3d   %3d\n", dtptr->costs[2][1],
           dtptr->costs[2][2], dtptr->costs[2][3]);
    printf("     3|  %3d   %3d   %3d\n", dtptr->costs[3][1],
           dtptr->costs[3][2], dtptr->costs[3][3]);
}

linkhandler0(linkid, newcost) int linkid, newcost;

/* called when cost from 0 to linkid changes from current value to newcost*/
/* You can leave this routine empty if you're an undergrad. If you want */
/* to use this routine, you'll need to change the value of the LINKCHANGE */
/* constant definition in prog3.c from 0 to 1 */

{
    printf(" linkhandler0() is called at time t=%.3f\n", clocktime);
    int old_dist_linkid_to_others[4];
    for (int i = 0; i < 4; i++)
    {
        old_dist_linkid_to_others[i] = dt0.costs[i][linkid] - dt0.costs[linkid][linkid];
    }

    int new_dist_0_to_linkid = newcost;

    for (int i = 0; i < 4; i++)
    {
        dt0.costs[i][linkid] = new_dist_0_to_linkid + old_dist_linkid_to_others[i]; // = new_dist_0_to_i (0 to i via linkid)
    }

    printdt0(&dt0);

    int old_min_cost[4];
    memcpy(old_min_cost, min_cost_0, sizeof(min_cost_0));
    int update = 0;
    calc_min_cost0();
    for (int i = 0; i < 4; i++)
    {
        if (old_min_cost[i] != min_cost_0[i])
        {
            update = 1;
            break;
        }
    }
    if (update == 1)
    {
        sendpkt0();
    }
    else
    {
        printf("Minimum cost didn't change. No new packets are sent\n");
    }
}
