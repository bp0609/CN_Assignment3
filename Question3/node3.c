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
} dt3;

/* students to write the following two routines, and maybe some others */

void rtinit3()
{
    int i, j;
    int directcost[4] = {7, INFINITY, 2, 0};
    int mincost[4];
    /* Clear the table */
    for (i = 0; i < 4; i++)
    {
        for (j = 0; j < 4; j++)
            dt3.costs[i][j] = INFINITY;
    }
    /* For node 3, use only neighbor columns 0 and 2 (neighbors 0 and 2) */
    dt3.costs[0][0] = directcost[0]; /* cost to node 0 via node 0 */
    dt3.costs[2][2] = directcost[2]; /* cost to node 2 via node 2 */
    dt3.costs[1][0] = directcost[1]; /* cost to node 1 via node 0 (will be INFINITY) */
    dt3.costs[3][0] = directcost[3]; /* self cost (0) */
    dt3.costs[3][2] = directcost[3]; /* duplicate in col2 for consistency */

    /* Compute current mincost vector */
    for (i = 0; i < 4; i++)
    {
        int cost_via0 = dt3.costs[i][0];
        int cost_via2 = dt3.costs[i][2];
        mincost[i] = (cost_via0 < cost_via2) ? cost_via0 : cost_via2;
    }

    printf("At time %.3f, rtinit3() called.\n", clocktime);
    printdt3(&dt3);

    /* Send initial packets to neighbors 0 and 2 */
    int nbr;
    struct rtpkt pkt;
    for (nbr = 0; nbr < 4; nbr++)
    {
        if ((nbr == 0 || nbr == 2) && (directcost[nbr] != INFINITY))
        {
            pkt.sourceid = 3;
            pkt.destid = nbr;
            for (i = 0; i < 4; i++)
                pkt.mincost[i] = mincost[i];
            printf("At time %.3f, node 3 sends packet to node %d with mincost { %d %d %d %d }\n",
                   clocktime, nbr,
                   pkt.mincost[0], pkt.mincost[1], pkt.mincost[2], pkt.mincost[3]);
            tolayer2(pkt);
        }
    }
}

void rtupdate3(rcvdpkt) struct rtpkt *rcvdpkt;

{
    int i, updated = 0;
    int src = rcvdpkt->sourceid;
    int connectcosts3[4] = {7, INFINITY, 2, 0};
    int cost_to_src = connectcosts3[src]; /* cost from node3 to src */
    int curr_mincost[4], new_mincost[4];
    int nbrIndex;

    printf("At time %.3f, rtupdate3() called, received packet from node %d\n",
           clocktime, src);

    /* Compute current mincost vector over neighbor columns 0 and 2 */
    for (i = 0; i < 4; i++)
    {
        int cost0 = dt3.costs[i][0];
        int cost2 = dt3.costs[i][2];
        curr_mincost[i] = (cost0 < cost2) ? cost0 : cost2;
    }

    /* Determine column: for node3, if src==0 use col0; if src==2 use col2 */
    nbrIndex = (src == 0) ? 0 : (src == 2) ? 2
                                           : -1;
    if (nbrIndex == -1)
        return;

    for (i = 0; i < 4; i++)
    {
        int newcost = cost_to_src + rcvdpkt->mincost[i];
        if (newcost < dt3.costs[i][nbrIndex])
        {
            dt3.costs[i][nbrIndex] = newcost;
            updated = 1;
        }
    }

    /* Recompute new mincost vector */
    for (i = 0; i < 4; i++)
    {
        int cost0 = dt3.costs[i][0];
        int cost2 = dt3.costs[i][2];
        new_mincost[i] = (cost0 < cost2) ? cost0 : cost2;
    }

    if (updated)
    {
        printf("At time %.3f, node 3's distance table updated.\n", clocktime);
    }
    else
    {
        printf("At time %.3f, node 3's distance table unchanged by packet from node %d.\n", clocktime, src);
    }
    printdt3(&dt3);

    /* If the vector changed, send updates to neighbors (nodes 0 and 2) */
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
            if ((nbr == 0 || nbr == 2) && (connectcosts3[nbr] != INFINITY))
            {
                pkt.sourceid = 3;
                pkt.destid = nbr;
                for (i = 0; i < 4; i++)
                    pkt.mincost[i] = new_mincost[i];
                printf("At time %.3f, node 3 sends packet to node %d with updated mincost { %d %d %d %d }\n",
                       clocktime, nbr,
                       pkt.mincost[0], pkt.mincost[1], pkt.mincost[2], pkt.mincost[3]);
                tolayer2(pkt);
            }
        }
    }
}

void printdt3(struct distance_table *dtptr)

{
    printf("             via     \n");
    printf("   D3 |    0     2 \n");
    printf("  ----|-----------\n");
    printf("     0|  %3d   %3d\n", dtptr->costs[0][0], dtptr->costs[0][2]);
    printf("dest 1|  %3d   %3d\n", dtptr->costs[1][0], dtptr->costs[1][2]);
    printf("     2|  %3d   %3d\n", dtptr->costs[2][0], dtptr->costs[2][2]);
}

/* Optional: link handler */
void linkhandler3(int linkid, int newcost)
{
    /* For an undergrad assignment, can be left empty */
}