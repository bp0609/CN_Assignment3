#include <stdio.h>
#include "distance_vector.c"
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
extern void tolayer2(struct rtpkt packet);
extern float clocktime;
struct distance_table
{
    int costs[4][4];
} dt2;

/* students to write the following two routines, and maybe some others */

void rtinit2()
{
    int i, j;
    int directcost[4] = {3, 1, 0, 2};
    int mincost[4];
    /* Initialize table: use only neighbor columns 0, 1, and 3 */
    for (i = 0; i < 4; i++)
    {
        for (j = 0; j < 4; j++)
            dt2.costs[i][j] = INFINITY;
    }
    /* Set direct link costs in the appropriate column:
       For node 2, we assume:
         - neighbor 0: column 0
         - neighbor 1: column 1
         - neighbor 3: column 3
    */
    dt2.costs[0][0] = directcost[0]; /* cost to node 0 via node 0 */
    dt2.costs[1][1] = directcost[1]; /* cost to node 1 via node 1 */
    dt2.costs[2][0] = directcost[2]; /* self cost can be set in one column */
    dt2.costs[2][1] = directcost[2];
    dt2.costs[2][3] = directcost[2];
    dt2.costs[3][3] = directcost[3]; /* cost to node 3 via node 3 */

    /* Compute current mincost vector */
    for (i = 0; i < 4; i++)
    {
        /* For each destination, take the minimum over the used columns */
        int mc = INFINITY;
        if (dt2.costs[i][0] < mc)
            mc = dt2.costs[i][0];
        if (dt2.costs[i][1] < mc)
            mc = dt2.costs[i][1];
        if (dt2.costs[i][3] < mc)
            mc = dt2.costs[i][3];
        mincost[i] = mc;
    }

    printf("At time %.3f, rtinit2() called.\n", clocktime);
    printdt2(&dt2);

    /* Send initial packets to neighbors 0, 1, and 3 */
    int nbr;
    struct rtpkt pkt;
    for (nbr = 0; nbr < 4; nbr++)
    {
        if ((nbr == 0 || nbr == 1 || nbr == 3) && (directcost[nbr] != INFINITY))
        {
            pkt.sourceid = 2;
            pkt.destid = nbr;
            for (i = 0; i < 4; i++)
                pkt.mincost[i] = mincost[i];
            printf("At time %.3f, node 2 sends packet to node %d with mincost { %d %d %d %d }\n",
                   clocktime, nbr,
                   pkt.mincost[0], pkt.mincost[1], pkt.mincost[2], pkt.mincost[3]);
            tolayer2(pkt);
        }
    }
}

void rtupdate2(rcvdpkt) struct rtpkt *rcvdpkt;

{
    int i, updated = 0;
    int directcost[4] = {3, 1, 0, 2};
    int src = rcvdpkt->sourceid;
    int cost_to_src = directcost[src]; /* direct cost from node2 to src */
    int curr_mincost[4], new_mincost[4];
    int nbrIndex;

    printf("At time %.3f, rtupdate2() called, received packet from node %d\n",
           clocktime, src);

    /* Compute current mincost over neighbor columns 0, 1, and 3 */
    for (i = 0; i < 4; i++)
    {
        int cost0 = dt2.costs[i][0];
        int cost1 = dt2.costs[i][1];
        int cost3 = dt2.costs[i][3];
        curr_mincost[i] = (cost0 < cost1 ? cost0 : cost1);
        if (cost3 < curr_mincost[i])
            curr_mincost[i] = cost3;
    }

    /* Determine which neighbor column to update based on source */
    /* For node 2:
         if src == 0, use column 0;
         if src == 1, use column 1;
         if src == 3, use column 3.
    */
    nbrIndex = (src == 0) ? 0 : (src == 1) ? 1
                            : (src == 3)   ? 3
                                           : -1;
    if (nbrIndex == -1)
        return; /* ignore any unexpected source */

    /* Update table: for each destination d, new cost via src = cost(2,src) + rcvdpkt->mincost[d] */
    for (i = 0; i < 4; i++)
    {
        int newcost = cost_to_src + rcvdpkt->mincost[i];
        if (newcost < dt2.costs[i][nbrIndex])
        {
            dt2.costs[i][nbrIndex] = newcost;
            updated = 1;
        }
    }

    /* Recompute mincost after update */
    for (i = 0; i < 4; i++)
    {
        int cost0 = dt2.costs[i][0];
        int cost1 = dt2.costs[i][1];
        int cost3 = dt2.costs[i][3];
        new_mincost[i] = (cost0 < cost1 ? cost0 : cost1);
        if (cost3 < new_mincost[i])
            new_mincost[i] = cost3;
    }

    if (updated)
    {
        printf("At time %.3f, node 2's distance table updated.\n", clocktime);
    }
    else
    {
        printf("At time %.3f, node 2's distance table unchanged by packet from node %d.\n", clocktime, src);
    }
    printdt2(&dt2);

    /* If mincost vector changed, send updated packets to neighbors 0, 1, and 3 */
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
        int nbr;
        struct rtpkt pkt;
        for (nbr = 0; nbr < 4; nbr++)
        {
            if ((nbr == 0 || nbr == 1 || nbr == 3) && (directcost[nbr] != INFINITY))
            {
                pkt.sourceid = 2;
                pkt.destid = nbr;
                for (i = 0; i < 4; i++)
                    pkt.mincost[i] = new_mincost[i];
                printf("At time %.3f, node 2 sends packet to node %d with updated mincost { %d %d %d %d }\n",
                       clocktime, nbr,
                       pkt.mincost[0], pkt.mincost[1], pkt.mincost[2], pkt.mincost[3]);
                tolayer2(pkt);
            }
        }
    }
}

void printdt2(struct distance_table *dtptr)

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

void linkhandler2(int linkid, int newcost)
{
    /* Optional for undergrad assignment */
}