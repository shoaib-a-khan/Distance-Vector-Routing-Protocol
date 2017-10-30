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

int connectcosts1[4] = { 1,  0,  1, 999 };

struct distance_table 
{
  int costs[4][4];
} dt1;


/* students to write the following two routines, and maybe some others */


rtinit1() 
{
	printf("\nrtinit1 invoked at time %f \n", clocktime);
	
	/*initialize distance table*/
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			dt1.costs[i][j] = 999;

	/*updating costs based on given initial network topology*/
	dt1.costs[0][0] = 1;
	dt1.costs[0][1] = 1;
	dt1.costs[1][1] = 0;
	dt1.costs[2][1] = 1;
	dt1.costs[2][2] = 1;

	/*printing the initial distance table*/
	printdt1(&dt1);

	/*make dv packets to send to neighbours*/
	struct rtpkt pkt0, pkt2;
	pkt0.sourceid = 1;
	pkt0.destid = 0;
	pkt2.sourceid = 1;
	pkt2.destid = 2;
	
	for (int i = 0; i < 4; i++)
	{
		pkt0.mincost[i] = dt1.costs[i][1];
		pkt2.mincost[i] = dt1.costs[i][1];
	}

	/*sending dv packets to neighbours*/
	tolayer2(pkt0);
	tolayer2(pkt2);

}


rtupdate1(rcvdpkt)
  struct rtpkt *rcvdpkt;
  
{
	printf("\nrtupdate1 invoked at time %f \n", clocktime);
	if (rcvdpkt->destid != 1)	//wrong destination check
		return;

	int j = rcvdpkt->sourceid;	/*srouce id signifies the column vector in dt0 that could be updated potentially*/
	int flag = 0;		/*flag is set to 1 in case there is an update in the distance table*/

	printf("\nPacket rcvd at node 1 from node %d \n", j);

	/*traversing column j of distance table to see if it needs to be updated*/
	for (int i = 0; i < 4; i++)
	{
		if (dt1.costs[i][j] > dt1.costs[j][1] + rcvdpkt->mincost[i])
		{
			dt1.costs[i][j] = dt1.costs[j][1] + rcvdpkt->mincost[i];
			if(i != 1)
				flag = 1;
		}
	}
	if (flag == 1) //change in dt1 detected!
	{
		printf("\nDistance table at node 1 updated: \n");
		printdt1(&dt1);

		struct rtpkt pkt0, pkt2;
		pkt0.sourceid = 1;
		pkt0.destid = 0;
		pkt2.sourceid = 1;
		pkt2.destid = 2;
		pkt0.mincost[1] = 0;
		pkt2.mincost[1] = 0;
		
		for (int i = 0; i < 4; i++)
		{
			if (i == 1)
				continue;
			pkt0.mincost[i] = min(dt1.costs[i][0], dt1.costs[i][2], dt1.costs[i][3]);
			pkt2.mincost[i] = min(dt1.costs[i][0], dt1.costs[i][2], dt1.costs[i][3]);
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


printdt1(dtptr)
  struct distance_table *dtptr;
  
{
  printf("             via   \n");
  printf("   D1 |    0     2 \n");
  printf("  ----|-----------\n");
  printf("     0|  %3d   %3d\n",dtptr->costs[0][0], dtptr->costs[0][2]);
  printf("dest 2|  %3d   %3d\n",dtptr->costs[2][0], dtptr->costs[2][2]);
  printf("     3|  %3d   %3d\n",dtptr->costs[3][0], dtptr->costs[3][2]);

}



linkhandler1(linkid, newcost)   
int linkid, newcost;   
/* called when cost from 1 to linkid changes from current value to newcost*/
/* You can leave this routine empty if you're an undergrad. If you want */
/* to use this routine, you'll need to change the value of the LINKCHANGE */
/* constant definition in prog3.c from 0 to 1 */
  
{
	printf("\nlinkhandler1 invoked at time %f \n", clocktime);
	if (linkid != 0) //checking valid neighbour id
		return;

	if (dt1.costs[0][0] == min(dt1.costs[0][0], dt1.costs[0][2], dt1.costs[0][3]) || newcost <= min(dt1.costs[0][0], dt1.costs[0][2], dt1.costs[0][3]))
	{
		dt1.costs[0][0] = newcost;
		dt1.costs[0][1] = newcost;

		printf("\nDistance table at node 1 updated: \n");
		printdt1(&dt1);

		struct rtpkt pkt0, pkt2;
		pkt0.sourceid = 1;
		pkt0.destid = 0;
		pkt2.sourceid = 1;
		pkt2.destid = 2;
		pkt0.mincost[1] = 0;
		pkt2.mincost[1] = 0;
		
		for (int i = 0; i < 4; i++)
		{
			if (i == 1)
				continue;
			pkt0.mincost[i] = min(dt1.costs[i][0], dt1.costs[i][2], dt1.costs[i][3]);
			pkt2.mincost[i] = min(dt1.costs[i][0], dt1.costs[i][2], dt1.costs[i][3]);

		}

		printf("\nSending routing packets to nodes 0 and 2 with following mincost vector: \n");
		for (int i = 0; i < 4; i++)
			printf("%d\t", pkt0.mincost[i]);
		printf("\n");
		/*sending dv packets to neighbours*/
		tolayer2(pkt0);
		tolayer2(pkt2);
	}
	else
	{
		printf("\nDistance table at node 1 updated without effecting mincost vector\n");
		dt1.costs[0][0] = newcost;
		dt1.costs[0][1] = newcost;

	}
}

