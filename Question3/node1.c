#include <stdio.h>
#include "distance_vector.c"
extern struct rtpkt
{
    int sourceid;   /* id of sending router sending this pkt */
    int destid;     /* id of router to which pkt being sent
                       (must be an immediate neighbor) */
    int mincost[4]; /* min cost to node 0 ... 3 */
};
extern void tolayer2(struct rtpkt packet);
extern float clocktime;
extern int TRACE;
extern int YES;
extern int NO;

int connectcosts1[4] = {1, 0, 1, 999};

struct distance_table
{
    int costs[4][4];
} dt1;

/* students to write the following two routines, and maybe some others */

void rtinit1()
{
    int i, j;
    int directcost[4] = {1, 0, 1, INFINITY};
    int mincost[4];
    /* Clear all entries */
    for (i = 0; i < 4; i++)
    {
        for (j = 0; j < 4; j++)
            dt1.costs[i][j] = INFINITY;
    }
    /* For node 1, only neighbor columns 0 and 2 are used */
    dt1.costs[0][0] = directcost[0]; /* cost to node 0 via node 0 */
    dt1.costs[1][0] = directcost[1]; /* self cost (0) placed in col 0 */
    dt1.costs[2][2] = directcost[2]; /* cost to node 2 via node 2 */
    dt1.costs[3][0] = directcost[3]; /* cost to node 3 via neighbor 0 (if any) */
    dt1.costs[3][2] = directcost[3]; /* also initialize col2 for destination 3 */

    /* Current mincost is the direct cost (taking the minimum from the two columns) */
    for (i = 0; i < 4; i++)
    {
        if (i == 0)
            mincost[i] = dt1.costs[i][0];
        else if (i == 2)
            mincost[i] = dt1.costs[i][2];
        else
        { /* for destinations not directly in our neighbor set, choose the lower */
            int temp = dt1.costs[i][0];
            if (dt1.costs[i][2] < temp)
                temp = dt1.costs[i][2];
            mincost[i] = temp;
        }
    }

    printf("At time %.3f, rtinit1() called.\n", clocktime);
    printdt1(&dt1);

    /* Send initial packets to directly connected neighbors: node0 and node2 */
    int nbr;
    struct rtpkt pkt;
    for (nbr = 0; nbr < 4; nbr++)
    {
        /* Only send if nbr is a neighbor of node1 (neighbors: 0 and 2) */
        if ((nbr == 0 || nbr == 2) && (directcost[nbr] != INFINITY))
        {
            pkt.sourceid = 1;
            pkt.destid = nbr;
            for (i = 0; i < 4; i++)
                pkt.mincost[i] = mincost[i];
            printf("At time %.3f, node 1 sends packet to node %d with mincost { %d %d %d %d }\n",
                   clocktime, nbr, pkt.mincost[0], pkt.mincost[1], pkt.mincost[2], pkt.mincost[3]);
            tolayer2(pkt);
        }
    }
}

void rtupdate1(rcvdpkt) struct rtpkt *rcvdpkt;

{
    int i, updated = 0;
    int src = rcvdpkt->sourceid;
    int cost_to_src = connectcosts1[src];
    int curr_mincost[4], new_mincost[4];
    int nbrIndex;

    printf("At time %.3f, rtupdate1() called, received packet from node %d\n",
           clocktime, src);

    /* Compute current mincost vector (only consider neighbor columns 0 and 2) */
    for (i = 0; i < 4; i++)
    {
        int cost_via0 = dt1.costs[i][0];
        int cost_via2 = dt1.costs[i][2];
        curr_mincost[i] = (cost_via0 < cost_via2) ? cost_via0 : cost_via2;
    }

    /* Update: for each destination, cost via src = direct cost to src + src's mincost */
    for (i = 0; i < 4; i++)
    {
        int newcost = cost_to_src + rcvdpkt->mincost[i];
        /* Determine proper column index: node1 uses column 0 if src==0 and column 2 if src==2 */
        nbrIndex = (src == 0) ? 0 : (src == 2) ? 2
                                               : -1;
        if (nbrIndex == -1)
            continue; /* ignore packets from non-neighbors */
        if (newcost < dt1.costs[i][nbrIndex])
        {
            dt1.costs[i][nbrIndex] = newcost;
            updated = 1;
        }
    }

    /* Recompute mincost after update */
    for (i = 0; i < 4; i++)
    {
        int cost_via0 = dt1.costs[i][0];
        int cost_via2 = dt1.costs[i][2];
        new_mincost[i] = (cost_via0 < cost_via2) ? cost_via0 : cost_via2;
    }

    if (updated)
    {
        printf("At time %.3f, node 1's distance table updated.\n", clocktime);
    }
    else
    {
        printf("At time %.3f, node 1's distance table unchanged by packet from node %d.\n", clocktime, src);
    }
    printdt1(&dt1);

    /* If the mincost vector changed, send updates to neighbors */
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
            if ((nbr == 0 || nbr == 2) && (connectcosts1[nbr] != INFINITY))
            {
                pkt.sourceid = 1;
                pkt.destid = nbr;
                for (i = 0; i < 4; i++)
                    pkt.mincost[i] = new_mincost[i];
                printf("At time %.3f, node 1 sends packet to node %d with updated mincost { %d %d %d %d }\n",
                       clocktime, nbr,
                       pkt.mincost[0], pkt.mincost[1], pkt.mincost[2], pkt.mincost[3]);
                tolayer2(pkt);
            }
        }
    }
}

/*
 * printdt1 - Pretty print node 1's distance table.
 * Only neighbor columns 0 and 2 are printed.
 */
void printdt1(struct distance_table *dtptr)

{
    printf("             via   \n");
    printf("   D1 |    0     2 \n");
    printf("  ----|-----------\n");
    printf("     0|  %3d   %3d\n", dtptr->costs[0][0], dtptr->costs[0][2]);
    printf("dest 2|  %3d   %3d\n", dtptr->costs[2][0], dtptr->costs[2][2]);
    printf("     3|  %3d   %3d\n", dtptr->costs[3][0], dtptr->costs[3][2]);
}

void linkhandler1(linkid, newcost) int linkid, newcost;
/* called when cost from 1 to linkid changes from current value to newcost*/
/* You can leave this routine empty if you're an undergrad. If you want */
/* to use this routine, you'll need to change the value of the LINKCHANGE */
/* constant definition in prog3.c from 0 to 1 */

{
}
