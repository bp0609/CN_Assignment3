#include <stdio.h>
#include <stdlib.h>
#include "distance_vector.c"
// #include "distance_vector.c"

extern int TRACE;
extern int YES;
extern int NO;
extern void tolayer2(struct rtpkt packet);
extern float clocktime;
struct distance_table
{
    int costs[4][4];
} dt0;

/* students to write the following two routines, and maybe some others */

void rtinit0()
{
    int i, j;
    int directcost[4] = {0, 1, 3, 7};
    int mincost[4];

    /* initialize all entries in dt0 to INFINITY */
    for (i = 0; i < 4; i++)
        for (j = 0; j < 4; j++)
            dt0.costs[i][j] = INFINITY;

    /* for a direct route, we place the direct cost in the "via self" column.
       Here we treat column 0 as representing the route via node 0 (i.e., direct link)
       Note: Only directly connected nodes are updated; other columns remain INFINITY. */
    for (i = 0; i < 4; i++)
    {
        dt0.costs[i][0] = directcost[i];
    }

    /* current minimum cost to each destination is simply the direct cost */
    for (i = 0; i < 4; i++)
    {
        mincost[i] = directcost[i];
    }

    /* Print an informative message and the distance table */
    printf("At time %.3f, rtinit0() called.\n", clocktime);
    printdt0(&dt0);

    /* send initial packets to all directly connected neighbors (neighbors 1, 2, 3) */
    int neighbor;
    struct rtpkt pkt;
    for (neighbor = 1; neighbor < 4; neighbor++)
    {
        if (directcost[neighbor] != INFINITY)
        {
            pkt.sourceid = 0;
            pkt.destid = neighbor;
            for (i = 0; i < 4; i++)
                pkt.mincost[i] = mincost[i];
            printf("At time %.3f, node 0 sends packet to node %d with mincost { %d %d %d %d }\n",
                   clocktime, neighbor,
                   pkt.mincost[0], pkt.mincost[1], pkt.mincost[2], pkt.mincost[3]);
            tolayer2(pkt);
        }
    }
}

void rtupdate0(rcvdpkt) struct rtpkt *rcvdpkt;
{
    int i, updated = 0;
    int source = rcvdpkt->sourceid;
    int cost_to_source = dt0.costs[source][0]; // direct cost from node 0 to source
    int curr_mincost[4];
    int new_mincost[4];

    printf("At time %.3f, rtupdate0() called, received packet from node %d\n", clocktime, source);

    /* Compute current mincost vector from dt0 */
    for (i = 0; i < 4; i++)
    {
        int mc = INFINITY, j;
        for (j = 0; j < 4; j++)
        {
            if (dt0.costs[i][j] < mc)
                mc = dt0.costs[i][j];
        }
        curr_mincost[i] = mc;
    }

    /* Update the distance table for each destination using the incoming packet's info */
    for (i = 0; i < 4; i++)
    {
        int newcost = cost_to_source + rcvdpkt->mincost[i];
        if (newcost < dt0.costs[i][source])
        {
            dt0.costs[i][source] = newcost;
            updated = 1;
        }
    }

    /* Recompute mincost vector after update */
    for (i = 0; i < 4; i++)
    {
        int mc = INFINITY, j;
        for (j = 0; j < 4; j++)
        {
            if (dt0.costs[i][j] < mc)
                mc = dt0.costs[i][j];
        }
        new_mincost[i] = mc;
    }

    /* Print the updated distance table */
    if (updated)
    {
        printf("At time %.3f, node 0's distance table updated.\n", clocktime);
        printdt0(&dt0);
    }
    else
    {
        printf("At time %.3f, node 0's distance table unchanged by packet from node %d.\n", clocktime, source);
        printdt0(&dt0);
    }

    /* If any mincost value has changed, send updated packets */
    int diff = 0;
    for (i = 0; i < 4; i++)
    {
        if (new_mincost[i] != curr_mincost[i])
        {
            diff = 1;
            break;
        }
    }

    if (updated && diff)
    {
        int neighbor;
        struct rtpkt pkt;
        for (neighbor = 1; neighbor < 4; neighbor++)
        {
            /* Send to only those neighbors that are directly connected */
            if (dt0.costs[neighbor][0] != INFINITY)
            {
                pkt.sourceid = 0;
                pkt.destid = neighbor;
                for (i = 0; i < 4; i++)
                    pkt.mincost[i] = new_mincost[i];
                printf("At time %.3f, node 0 sends packet to node %d with updated mincost { %d %d %d %d }\n",
                       clocktime, neighbor,
                       pkt.mincost[0], pkt.mincost[1], pkt.mincost[2], pkt.mincost[3]);
                tolayer2(pkt);
            }
        }
    }
}

void printdt0(struct distance_table *dtptr)

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
}
