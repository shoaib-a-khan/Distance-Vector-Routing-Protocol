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
  int costs[4][4]; /*costs[i][j] := cost to go to node i via direct neighbour j*/
} dt0;

/*utility min funciton*/
int min(int a, int b, int c)
{
	int temp = (a <= b ? a : b);
	temp = (temp <= c ? temp : c);
	return temp;

}

/* students to write the following two routines, and maybe some others */

void rtinit0() 
{
	printf("\nrtinit0 invoked at time %f \n", clocktime);
	
	/*initialize distance table*/
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			dt0.costs[i][j] = 999;
	
	/*updating costs based on given initial network topology*/
	dt0.costs[0][0] = 0;
	dt0.costs[1][0] = 1;
	dt0.costs[1][1] = 1;
	dt0.costs[2][0] = 3;
	dt0.costs[2][2] = 3;
	dt0.costs[3][0] = 7;
	dt0.costs[3][3] = 7;
	
	/*printing the initial distance table*/
	printdt0(&dt0);

	/*make dv packets to send to neighbours*/
	struct rtpkt pkt1, pkt2, pkt3;
	pkt1.sourceid = 0;
	pkt1.destid = 1;
	pkt2.sourceid = 0;
	pkt2.destid = 2;
	pkt3.sourceid = 0;
	pkt3.destid = 3;

	for (int i = 0; i < 4; i++)
	{
		pkt1.mincost[i] = dt0.costs[i][0];
		pkt2.mincost[i] = dt0.costs[i][0];
		pkt3.mincost[i] = dt0.costs[i][0];
	}
	
	/*sending dv packets to neighbours*/
	tolayer2(pkt1);
	tolayer2(pkt2);
	tolayer2(pkt3);
}


void rtupdate0(rcvdpkt)
  struct rtpkt *rcvdpkt;
{
	printf("\nrtpdate0 invoked at time %f \n", clocktime);
	
	if (rcvdpkt->destid != 0)	//wrong destination check
		return;
	
	int j = rcvdpkt->sourceid;	/*srouce id signifies the column vector in dt0 that could be updated potentially*/
	int flag = 0;		/*flag is set to 1 in case there is an update in the distance table*/
	
	printf("\nPacket rcvd at node 0 from node %d \n", j);
	
	/*traversing column j of distance table to see if it needs to be updated*/
	for (int i = 0; i < 4; i++)
	{
		if (dt0.costs[i][j] > dt0.costs[j][0]+ rcvdpkt->mincost[i])
		{
			dt0.costs[i][j] = dt0.costs[j][0] + rcvdpkt->mincost[i];
			if(i != 0)
				flag = 1;
		}
	}
	if (flag == 1) //change in dt0 detected!
	{
		printf("\nDistance table at node 0 updated: \n");
		printdt0(&dt0);
		
		struct rtpkt pkt1, pkt2, pkt3;
		pkt1.sourceid = 0;
		pkt1.destid = 1;
		pkt2.sourceid = 0;
		pkt2.destid = 2;
		pkt3.sourceid = 0;
		pkt3.destid = 3;
		pkt1.mincost[0] = 0;
		pkt2.mincost[0] = 0;
		pkt3.mincost[0] = 0;

		for (int i = 1; i < 4; i++)
		{
			pkt1.mincost[i] = min(dt0.costs[i][1], dt0.costs[i][2], dt0.costs[i][3]);
			pkt2.mincost[i] = min(dt0.costs[i][1], dt0.costs[i][2], dt0.costs[i][3]);
			pkt3.mincost[i] = min(dt0.costs[i][1], dt0.costs[i][2], dt0.costs[i][3]);
			
		}

		printf("\nSending routing packets to nodes 1, 2 and 3 with following mincost vector: \n");
		for (int i = 0; i < 4; i++)
			printf("%d\t", pkt1.mincost[i]);
		printf("\n");
		/*sending dv packets to neighbours*/
		tolayer2(pkt1);
		tolayer2(pkt2);
		tolayer2(pkt3);
	}
}


printdt0(dtptr)
  struct distance_table *dtptr;
  
{
  printf("                via     \n");
  printf("   D0 |    1     2    3 \n");
  printf("  ----|-----------------\n");
  printf("     1|  %3d   %3d   %3d\n",dtptr->costs[1][1],
   dtptr->costs[1][2],dtptr->costs[1][3]);
  printf("dest 2|  %3d   %3d   %3d\n",dtptr->costs[2][1],
   dtptr->costs[2][2],dtptr->costs[2][3]);
  printf("     3|  %3d   %3d   %3d\n",dtptr->costs[3][1],
   dtptr->costs[3][2],dtptr->costs[3][3]);
}

linkhandler0(linkid, newcost)   
  int linkid, newcost;

/* called when cost from 0 to linkid changes from current value to newcost*/
/* You can leave this routine empty if you're an undergrad. If you want */
/* to use this routine, you'll need to change the value of the LINKCHANGE */
/* constant definition in prog3.c from 0 to 1 */
  
{
	printf("\nlinkhandler0 invoked at time %f \n", clocktime);
	if (linkid != 1) //checking valid neighbour id
		return;
	
	if (dt0.costs[1][1] == min(dt0.costs[1][1], dt0.costs[1][2], dt0.costs[1][3])|| newcost <= min(dt0.costs[1][1], dt0.costs[1][2], dt0.costs[1][3]))
	{
		dt0.costs[1][0] = newcost;
		dt0.costs[1][1] = newcost;

		printf("\nDistance table at node 0 updated: \n");
		printdt0(&dt0);

		struct rtpkt pkt1, pkt2, pkt3;
		pkt1.sourceid = 0;
		pkt1.destid = 1;
		pkt2.sourceid = 0;
		pkt2.destid = 2;
		pkt3.sourceid = 0;
		pkt3.destid = 3;
		pkt1.mincost[0] = 0;
		pkt2.mincost[0] = 0;
		pkt3.mincost[0] = 0;

		for (int i = 1; i < 4; i++)
		{
			pkt1.mincost[i] = min(dt0.costs[i][1], dt0.costs[i][2], dt0.costs[i][3]);
			pkt2.mincost[i] = min(dt0.costs[i][1], dt0.costs[i][2], dt0.costs[i][3]);
			pkt3.mincost[i] = min(dt0.costs[i][1], dt0.costs[i][2], dt0.costs[i][3]);

		}

		printf("\nSending routing packets to nodes 1, 2 and 3 with following mincost vector: \n");
		for (int i = 0; i < 4; i++)
			printf("%d\t", pkt1.mincost[i]);
		printf("\n");
		/*sending dv packets to neighbours*/
		tolayer2(pkt1);
		tolayer2(pkt2);
		tolayer2(pkt3);
	}
	else
	{
		printf("\nDistance table at node 0 updated without effecting mincost vector\n");
		dt0.costs[1][0] = newcost;
		dt0.costs[1][1] = newcost;
	}
	
}
