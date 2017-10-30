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
} dt3;

/* students to write the following two routines, and maybe some others */

void rtinit3() 
{
	printf("\n rtinit3 invoked at time %f \n", clocktime);
	
	/*initialize distance table*/
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			dt3.costs[i][j] = 999;

	/*updating costs based on given initial network topology*/
	dt3.costs[0][0] = 7;
	dt3.costs[0][3] = 7;
	dt3.costs[2][2] = 2;
	dt3.costs[2][3] = 2;
	dt3.costs[3][3] = 0;

	/*printing the initial distance table*/
	printdt3(&dt3);

	/*make dv packets to send to neighbours*/
	struct rtpkt pkt0, pkt2;
	pkt0.sourceid = 3;
	pkt0.destid = 0;
	pkt2.sourceid = 3;
	pkt2.destid = 2;
	
	for (int i = 0; i < 4; i++)
	{
		pkt0.mincost[i] = dt3.costs[i][3];
		pkt2.mincost[i] = dt3.costs[i][3];
	}

	/*sending dv packets to neighbours*/
	tolayer2(pkt0);
	tolayer2(pkt2);
}


void rtupdate3(rcvdpkt)
  struct rtpkt *rcvdpkt;
  
{
	printf("\nrtupdate3 invoked at time %f \n", clocktime);
	if (rcvdpkt->destid != 3)	//wrong destination check
		return;

	int j = rcvdpkt->sourceid;	/*srouce id signifies the column vector in dt3 that could be updated potentially*/
	int flag = 0;		/*flag is set to 1 in case there is an update in the distance table*/

	printf("\nPacket rcvd at node 3 from node %d \n", j);

	/*traversing column j of distance table to see if it needs to be updated*/
	for (int i = 0; i < 4; i++)
	{
		if (dt3.costs[i][j] > dt3.costs[j][3] + rcvdpkt->mincost[i])
		{
			dt3.costs[i][j] = dt3.costs[j][3] + rcvdpkt->mincost[i];
			if(i != 3)
				flag = 1;
		}
	}
	if (flag == 1) //change in dt3 detected!
	{
		printf("\nDistance table at node 3 updated: \n");
		printdt3(&dt3);

		struct rtpkt pkt0, pkt2;
		pkt0.sourceid = 3;
		pkt0.destid = 0;
		pkt2.sourceid = 3;
		pkt2.destid = 2;
		pkt0.mincost[3] = 0;
		pkt2.mincost[3] = 0;

		for (int i = 0; i < 4; i++)
		{
			if (i == 3)
				continue;
			pkt0.mincost[i] = min(dt3.costs[i][0], dt3.costs[i][1], dt3.costs[i][2]);
			pkt2.mincost[i] = min(dt3.costs[i][0], dt3.costs[i][1], dt3.costs[i][2]);
		}

		printf("\nSending routing packets to nodes 0 and 2 with following mincost vector: \n");
		for (int i = 0; i < 4; i++)
			printf("%d\t", pkt0.mincost[i]);
		printf("\n");
		/*sending dv packets to neighbours*/
		tolayer2(pkt0);
		tolayer2(pkt2);
	}
}


printdt3(dtptr)
  struct distance_table *dtptr;
  
{
  printf("             via     \n");
  printf("   D3 |    0     2 \n");
  printf("  ----|-----------\n");
  printf("     0|  %3d   %3d\n",dtptr->costs[0][0], dtptr->costs[0][2]);
  printf("dest 1|  %3d   %3d\n",dtptr->costs[1][0], dtptr->costs[1][2]);
  printf("     2|  %3d   %3d\n",dtptr->costs[2][0], dtptr->costs[2][2]);

}






