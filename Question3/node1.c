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
} dt1;

#define INFINITY 999
extern float clocktime;
/* students to write the following two routines, and maybe some others */
int connectcosts1[4] = {1, 0, 1, 999};
struct rtpkt pkt1[4];
int min_cost_1[4];

void calc_min_cost1()
{
    // calculate the min cost for every other node
    for (int i = 0; i < 4; i++)
    {
        min_cost_1[i] = INFINITY;
        for (int j = 0; j < 4; j++)
        {
            if (dt1.costs[i][j] < min_cost_1[i])
            {
                min_cost_1[i] = dt1.costs[i][j];
            }
        }
    }
}

void sendpkt1()
{
    // make the pkts
    for (int i = 0; i < 4; i++)
    {
        pkt1[i].sourceid = 1;
        pkt1[i].destid = i; // to all 4 coz all are connected
        memcpy(pkt1[i].mincost, min_cost_1, sizeof(min_cost_1));
    }
    // SEND PKTS TO ALL NEIGHBOURS
    for (int i = 0; i < 3; i++)
    {
        if (i != 1 && connectcosts1[i] != INFINITY)
        {
            printf("At time t=%.3f, node %d sends packet to node %d with: (%d  %d  %d  %d)\n",
                   clocktime, pkt1[i].sourceid, pkt1[i].destid, pkt1[i].mincost[0], pkt1[i].mincost[1],
                   pkt1[i].mincost[2], pkt1[i].mincost[3]);
            tolayer2(pkt1[i]);
        }
    }
}

rtinit1()
{
    printf("\nrtint1() is called at time t=:%0.3f\n", clocktime);

    // Initialize the distance table for the first time.
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            if (i == j)
            {
                dt1.costs[i][j] = connectcosts1[i];
            }
            else
            {
                dt1.costs[i][j] = INFINITY;
            }
        }
    }
    printf("Distance table for node 0 is initialized.\n");
    printdt1(&dt1);
    calc_min_cost1();
    sendpkt1();
}

void rtupdate1(rcvdpkt) struct rtpkt *rcvdpkt;

{
    int src = rcvdpkt->sourceid;
    int dst = rcvdpkt->destid;
    int dist_vec_rcvd[4];
    memcpy(dist_vec_rcvd, rcvdpkt->mincost, sizeof(rcvdpkt->mincost));
    printf("rtupdate1() is called at time t=: %0.3f as node %d sent a pkt with (%d  %d  %d  %d)\n", clocktime, src,
           dist_vec_rcvd[0], dist_vec_rcvd[1], dist_vec_rcvd[2], dist_vec_rcvd[3]);

    for (int i = 0; i < 4; i++)
    {
        int new_cost = connectcosts1[src] + dist_vec_rcvd[i];
        if (new_cost < INFINITY)
        {
            dt1.costs[i][src] = new_cost;
        }
        else
        {
            dt1.costs[i][src] = INFINITY;
        }
    }
    printf("At time %.3f, node 1's distance table updated.\n", clocktime);
    printdt1(&dt1);

    // Check if the min cost changed
    int old_min_cost[4];
    memcpy(old_min_cost, min_cost_1, sizeof(min_cost_1));
    int update = 0;
    calc_min_cost1();
    for (int i = 0; i < 4; i++)
    {
        if (old_min_cost[i] != min_cost_1[i])
        {
            update = 1;
            break;
        }
    }
    if (update == 1)
    {
        sendpkt1();
    }
    else
    {
        printf("Minimum cost didn't change. No new packets are sent\n");
    }
}

printdt1(dtptr) struct distance_table *dtptr;

{
    printf("             via   \n");
    printf("   D1 |    0     2 \n");
    printf("  ----|-----------\n");
    printf("     0|  %3d   %3d\n", dtptr->costs[0][0], dtptr->costs[0][2]);
    printf("dest 2|  %3d   %3d\n", dtptr->costs[2][0], dtptr->costs[2][2]);
    printf("     3|  %3d   %3d\n", dtptr->costs[3][0], dtptr->costs[3][2]);
}

printmincost1()
{
    printf("Minimum cost from %d to other nodes are: %d %d %d %d\n", 1, min_cost_1[0], min_cost_1[1],
           min_cost_1[2], min_cost_1[3]);
}

void linkhandler1(linkid, newcost) int linkid, newcost;
/* called when cost from 1 to linkid changes from current value to newcost*/
/* You can leave this routine empty if you're an undergrad. If you want */
/* to use this routine, you'll need to change the value of the LINKCHANGE */
/* constant definition in prog3.c from 0 to 1 */

{
    printf(" linkhandler1() is called at time t=%.3f\n", clocktime);
    int old_dist_linkid_to_others[4];
    for (int i = 0; i < 4; i++)
    {
        old_dist_linkid_to_others[i] = dt1.costs[i][linkid] - dt1.costs[linkid][linkid];
    }

    int new_dist_0_to_linkid = newcost;

    for (int i = 0; i < 4; i++)
    {
        dt1.costs[i][linkid] = new_dist_0_to_linkid + old_dist_linkid_to_others[i]; // = new_dist_1_to_i (1 to i via linkid)
    }

    printdt1(&dt1);
    int old_min_cost[4];
    memcpy(old_min_cost, min_cost_1, sizeof(min_cost_1));
    int update = 0;
    calc_min_cost1();
    for (int i = 0; i < 4; i++)
    {
        if (old_min_cost[i] != min_cost_1[i])
        {
            update = 1;
            break;
        }
    }
    if (update == 1)
    {
        sendpkt1();
    }
    else
    {
        printf("\nMinimum cost didn't change. No new packets are sent\n");
    }
}
