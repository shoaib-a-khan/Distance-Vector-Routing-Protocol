#include <stdio.h>

extern struct rtpkt {
  int sourceid;       /* id of sending router sending this pkt */
  int destid;         /* id of router to which pkt being sent 
                         (must be an immediate neighbor) */
  int mincost[4];    /* min cost to node 0 ... 3 */
  };

extern int TRACE;
extern int YES;
extern int NO;
extern float clocktime;

struct distance_table 
{
  int costs[4][4];
} dt2;


/* students to write the following two routines, and maybe some others */

void rtinit2() 
{
	printf("\nrtinit2 invoked at time %f \n", clocktime);
	
	/*initialize distance table*/
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			dt2.costs[i][j] = 999;

	/*updating costs based on given initial network topology*/
	dt2.costs[0][0] = 3;
	dt2.costs[0][2] = 3;
	dt2.costs[1][1] = 1;
	dt2.costs[1][2] = 1;
	dt2.costs[2][2] = 0;
	dt2.costs[3][2] = 2;
	dt2.costs[3][3] = 2;
	
	/*printing the initial distance table*/
	printdt2(&dt2);

	/*make dv packets to send to neighbours*/
	struct rtpkt pkt0, pkt1, pkt3;
	pkt0.sourceid = 2;
	pkt0.destid = 0;
	pkt1.sourceid = 2;
	pkt1.destid = 1;
	pkt3.sourceid = 2;
	pkt3.destid = 3;

	for (int i = 0; i < 4; i++)
	{
		pkt0.mincost[i] = dt2.costs[i][2];
		pkt1.mincost[i] = dt2.costs[i][2];
		pkt3.mincost[i] = dt2.costs[i][2];
	}

	/*sending dv packets to neighbours*/
	tolayer2(pkt0);
	tolayer2(pkt1);
	tolayer2(pkt3);

}


void rtupdate2(rcvdpkt)
  struct rtpkt *rcvdpkt;
  
{
	printf("\nrtupdate2 invoked at time %f \n", clocktime);
	if (rcvdpkt->destid != 2)	//wrong destination check
		return;

	int j = rcvdpkt->sourceid;	/*srouce id signifies the column vector in dt2 that could be updated potentially*/
	int flag = 0;		/*flag is set to 1 in case there is an update in the distance table*/

	printf("\nPacket rcvd at node 2 from node %d \n", j);

	/*traversing column j of distance table to see if it needs to be updated*/
	for (int i = 0; i < 4; i++)
	{
		if (dt2.costs[i][j] > dt2.costs[j][2] + rcvdpkt->mincost[i])
		{
			dt2.costs[i][j] = dt2.costs[j][2] + rcvdpkt->mincost[i];
			if(i != 2)
				flag = 1;
		}
	}
	if (flag == 1) //change in dt2 detected!
	{
		printf("\nDistance table at node 2 updated: \n");
		printdt2(&dt2);

		struct rtpkt pkt0, pkt1, pkt3;
		pkt0.sourceid = 2;
		pkt0.destid = 0;
		pkt1.sourceid = 2;
		pkt1.destid = 1;
		pkt3.sourceid = 2;
		pkt3.destid = 3;
		pkt0.mincost[2] = 0;
		pkt1.mincost[2] = 0;
		pkt3.mincost[2] = 0;

		for (int i = 0; i < 4; i++)
		{
			if (i == 2)
				continue;
			pkt0.mincost[i] = min(dt2.costs[i][0], dt2.costs[i][1], dt2.costs[i][3]);
			pkt1.mincost[i] = min(dt2.costs[i][0], dt2.costs[i][1], dt2.costs[i][3]);
			pkt3.mincost[i] = min(dt2.costs[i][0], dt2.costs[i][1], dt2.costs[i][3]);

		}

		printf("\nSending routing packets to nodes 0, 1 and 3 with following mincost vector: \n");
		for (int i = 0; i < 4; i++)
			printf("%d\t", pkt0.mincost[i]);
		printf("\n");
		/*sending dv packets to neighbours*/
		tolayer2(pkt0);
		tolayer2(pkt1);
		tolayer2(pkt3);
	}

}


printdt2(dtptr)
  struct distance_table *dtptr;
  
{
  printf("                via     \n");
  printf("   D2 |    0     1    3 \n");
  printf("  ----|-----------------\n");
  printf("     0|  %3d   %3d   %3d\n",dtptr->costs[0][0],
   dtptr->costs[0][1],dtptr->costs[0][3]);
  printf("dest 1|  %3d   %3d   %3d\n",dtptr->costs[1][0],
   dtptr->costs[1][1],dtptr->costs[1][3]);
  printf("     3|  %3d   %3d   %3d\n",dtptr->costs[3][0],
   dtptr->costs[3][1],dtptr->costs[3][3]);
}






