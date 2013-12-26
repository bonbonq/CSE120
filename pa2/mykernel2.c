/* mykernel2.c: your portion of the kernel (last modified 10/18/09)
 *
 *	Below are procedures that are called by other parts of the kernel.
 *	Your ability to modify the kernel is via these procedures.  You may
 *	modify the bodies of these procedures any way you wish (however,
 *	you cannot change the interfaces).
 * 
 */

#include "aux.h"
#include "sys.h"
#include "mykernel2.h"

#define TIMERINTERVAL 1	/* in ticks (tick = 10 msec) */

/*	A sample process table.  You may change this any way you wish.
 */

static struct {
	int valid;		/* is this entry valid: 1 = yes, 0 = no */
	int pid;		/* process id (as provided by kernel) */
	int position;

	int m;
	int n;

	int requested_cpu;
	int age;
	int running_time;
	float ratio;

} proctab[MAXPROCS];

static int key;
static int position_added;

static int active[MAXPROCS];
static int head;
static int tail;

static float currentReq;


/*	InitSched () is called when kernel starts up.  First, set the
 *	scheduling policy (see sys.h).  Make sure you follow the rules
 *	below on where and how to set it.  Next, initialize all your data
 *	structures (such as the process table).  Finally, set the timer
 *	to interrupt after a specified number of ticks.
 */

void InitSched ()
{
	int i;

	/* First, set the scheduling policy.  You should only set it
	 * from within this conditional statement.  While you are working
	 * on this assignment, GetSchedPolicy will return NOSCHEDPOLICY,
	 * and so the condition will be true and you may set the scheduling
	 * policy to whatever you choose (i.e., you may replace ARBITRARY).
	 * After the assignment is over, during the testing phase, we will
	 * have GetSchedPolicy return the policy we wish to test, and so
	 * the condition will be false and SetSchedPolicy will not be
	 * called, thus leaving the policy to whatever we chose to test.
	 */

	 // Printf("InitSched() is called\n");
	 // Printf("\n");

	if (GetSchedPolicy () == NOSCHEDPOLICY) {	/* leave as is */
		SetSchedPolicy (PROPORTIONAL);		/* set policy here */
	}
		
	/* Initialize all your data structures here */
	head = 0;
	tail = 0;

	currentReq = 0;
	
	key = 0;
	position_added = 0;

	// Printf("MAXPROCS = %d\n",MAXPROCS);

	for (i = 0; i < MAXPROCS; i++) {
		proctab[i].valid = 0;
	}

	/* Set the timer last */
	SetTimer (TIMERINTERVAL);
}


/*	StartingProc (pid) is called by the kernel when the process
 *	identified by pid is starting.  This allows you to record the
 *	arrival of a new process in the process table, and allocate
 *	any resources (if necessary).  Returns 1 if successful, 0 otherwise.
 */

int StartingProc (pid)
	int pid;
{
	int i;
	static int add_to = 0;

	if(GetSchedPolicy() == PROPORTIONAL)
	{
		// //look for the process that had its time requested
		// for(i = 0; i<MAXPROCS; i++)
		// {
		// 	if(proctab[i].pid == pid)
		// 	{
		// 		// Printf("\nUpdated %d in position %d on table\n",pid,i);
		// 		proctab[i].valid = 1;
		// 		proctab[i].running_time = 0;
		// 		proctab[i].requested_cpu = 0;
		// 		float requested = (float) proctab[i].m / (float)proctab[i].n;
		// 		currentReq = currentReq + requested;
		// 		// DoSched();
		// 		return (1);
		// 	}
		// }

		//enter this loop only if process has no declared cpu
		//insert into proctab as a new non-cpu-requesting process
		for(i = 0; i<MAXPROCS; i++)
		{
			if(!proctab[i].valid)
			{
				// Printf("\nPut %d in position %d on table\n",pid,i);
				proctab[i].valid = 1;
				proctab[i].pid = pid;
				proctab[i].m = 0;
				proctab[i].n = 0;
				proctab[i].running_time = 0;
				proctab[i].requested_cpu = 0;
				float requested = (float) proctab[i].m / (float)proctab[i].n;
				currentReq = currentReq + requested;

				active[tail] = pid;
				tail++;
				// DoSched();
				return (1);
			}
		}
	}

	for (i = 0; i < MAXPROCS; i++) {

		if (!proctab[i].valid && tail!=MAXPROCS) {

			if(GetSchedPolicy() == FIFO){

				proctab[i].valid = 1;
				proctab[i].pid = pid;

				active[tail] = pid;
				tail++;

				return(1);

			}


			if(GetSchedPolicy() == LIFO){
				// Printf("\nStartingProc(%d) is called at tail = %d with i = %d\n",pid,tail,i);

				proctab[i].valid = 1;
				proctab[i].pid = pid;

				active[tail] = pid;
				tail++;

				DoSched();
				return (1);
			}

			if(GetSchedPolicy() == ROUNDROBIN){

				proctab[i].valid = 1;
				proctab[i].pid = pid;

				active[tail] = pid;
				tail++;

				return(1);
			}
		}
	}

	Printf ("\nError in StartingProc: no free table entries\n");
	return (0);
}
			

/*	EndingProc (pid) is called by the kernel when the process
 *	identified by pid is ending.  This allows you to update the
 *	process table accordingly, and deallocate any resources (if
 *	necessary).  Returns 1 if successful, 0 otherwise.
 */


int EndingProc (pid)
	int pid;
{
	// Printf("\nEndingProc() is called for %d\n",pid);

	int i;

	for (i = 0; i < MAXPROCS; i++) {

		if (proctab[i].valid && proctab[i].pid == pid) {
			proctab[i].valid = 0;

			if(GetSchedPolicy() == FIFO){
				head++;
			}

			if(GetSchedPolicy() == LIFO){
				tail--;;
			}

			if(GetSchedPolicy() == ROUNDROBIN){
				//shift all following elements in active up by one

				int active_p = 0;
				while(active[active_p]!=pid)
				{
					active_p++;
				}

				for(int p = active_p; p<tail-1; p++)
				{
					active[p] = active[p+1];
				}

				//also moves tail towards head by one
				tail--;
				if(head == tail)
					head = tail-1;

			}

			if(GetSchedPolicy() == PROPORTIONAL){
				float requested = ((float)proctab[i].m/(float)proctab[i].n);
				currentReq = currentReq - requested;

				if(proctab[i].requested_cpu == 0)
				{
					int active_p = 0;
					while(active[active_p]!=pid)
					{
						active_p++;
					}

					for(int p = active_p; p<tail-1; p++)
					{
						active[p] = active[p+1];
					}

					active[tail-1] = 0;
					
					//also moves tail towards head by one
					tail--;
				}
			}

			return (1);
		}
	}

	Printf ("\nError in EndingProc: can't find process %d\n", pid);
	return (0);
}


/*	SchedProc () is called by kernel when it needs a decision for
 *	which process to run next.  It calls the kernel function
 *	GetSchedPolicy () which will return the current scheduling policy
 *	which was previously set via SetSchedPolicy (policy). SchedProc ()
 *	should return a process id, or 0 if there are no processes to run.
 */

int SchedProc ()
{
	// Printf("\n**SchedProc() is called.\n");

	int i;
	static int prev_id = 0;
	static int order = 0;

	static int start_pid = 1;
	static int active_proc;
	static int smallest_ratio_id;
	static int smallest_ratio_id_holder;

	//prints out active[] and head and tail
	// for(int x = 0; x<MAXPROCS; x++)
	// {
	// 	Printf("\nactive[%d] = %d",x,active[x]);
	// }
	// Printf("\nhead = %d",head);
	// Printf("\ntail = %d",tail);
	

	switch (GetSchedPolicy ()) {

	case ARBITRARY:
		for (i = 0; i < MAXPROCS; i++) {
			if (proctab[i].valid) {
				// Printf("\n%d (position: %d) is scheduled next.\n\n",proctab[i].pid,i);
				return (proctab[i].pid);
			}
		}		
		break;

	case FIFO:

		for (i = 0; i < MAXPROCS; i++) {
			// Printf("\nactive[head] = %d",active[head]);
			if (proctab[i].pid == active[head] && proctab[i].valid) {
				// Printf("\n%d (position: %d) is scheduled next.\n",proctab[i].pid,i);
				return (proctab[i].pid);
			}
		}
		break;

	case LIFO:

		for (i = MAXPROCS-1; i >= 0; i--) {
			// Printf("\ni = %d  proctab[i].valid = %d  proctab[i].pid=%d\n",i,proctab[i].valid,proctab[i].pid);
			if (proctab[i].valid && proctab[i].pid == active[tail-1]){
				// Printf("\n%d (position: %d) is scheduled next.\n\n",proctab[i].pid,i);
				return (proctab[i].pid);
			}
		}

		break;

	case ROUNDROBIN:
		// Printf("\nhead = %d",head);
		// Printf("\nactive[head] = %d",active[head]);
		// Printf("\ntail = %d",tail);

		// Note: head will point to the process in active[] that will need to get ran
		for(i = 0; i<MAXPROCS; i++)
		{
			// Printf("\nproctab[%d].pid = %d  proctab[%d].valid = %d",i,proctab[i].pid,i,proctab[i].valid);
			if (proctab[i].pid == active[head] && proctab[i].valid) 
			{
				head++;
				if(head == tail)
					head = 0;	
				return(proctab[i].pid);
			}
		}

		break;

	case PROPORTIONAL:

		//find smallest ratio
		smallest_ratio_id = MAXPROCS; //id position of smallest ratio process in proctable

		for (i = 0; i < MAXPROCS; i++) {
			// Printf("\ni = %d  proctab[i].valid = %d  proctab[i].requested_cpu = %d",i,proctab[i].valid,proctab[i].requested_cpu);
			// Printf(" m = %d  n = %d  ",proctab[i].m,proctab[i].n);
			// Printf(" age = %d  running_time = %d  ",proctab[i].age,proctab[i].running_time);

			if (proctab[i].valid && proctab[i].requested_cpu)
			{
				// Printf("\nProcess #%d found.", proctab[i].pid);
				if(smallest_ratio_id == MAXPROCS)
					smallest_ratio_id = i;
				else{
					// Printf("Comparing: %d and %d",proctab[i].pid,proctab[smallest_ratio_id].pid);
					int left = proctab[i].running_time*proctab[i].n*proctab[smallest_ratio_id].m*proctab[smallest_ratio_id].age;
					int right = proctab[i].m*proctab[i].age*proctab[smallest_ratio_id].running_time*proctab[smallest_ratio_id].n;
					//if new ratio is smaller
					if(left < right)
						smallest_ratio_id = i;
					//if two process have same ratio, choose one with smaller n
					else if(left == right)
					{
						if(proctab[i].n <= proctab[smallest_ratio_id].n)
							smallest_ratio_id = i;
					}
				}
			}
		}	

		// Printf("\n------");

		if(smallest_ratio_id != MAXPROCS)
		{
			// Printf("\nsmallest ratio process is: %d\n",proctab[smallest_ratio_id].pid);
			float utilization = (float)proctab[smallest_ratio_id].running_time / (float)proctab[smallest_ratio_id].age;
			float requested = (float) proctab[smallest_ratio_id].m / (float) proctab[smallest_ratio_id].n;
			int ratio = utilization / requested;
			// Printf("\nratio = %d",ratio);
			if(ratio < 1)
			{
				//update running time of process, smallest_ratio_pid
				proctab[smallest_ratio_id].running_time++;
				// Printf("Returning proctab[smallest_ratio_id].pid = %d\n",proctab[smallest_ratio_id].pid);
				return proctab[smallest_ratio_id].pid;
			}
			else
			{
				smallest_ratio_id_holder = smallest_ratio_id;
				smallest_ratio_id = MAXPROCS;
			}
			
		}

		// enter this only when all processes do not request CPU
		// OR when all the processes' ratios >= 1
		// Execution: round-robin through processes of those that did not make CPU requests
		if(smallest_ratio_id == MAXPROCS)
		{
			// Printf("head = %d   tail = %d\n",head,tail);
			// for(int y = 0; y<MAXPROCS; y++)
			// {
			// 	Printf("\n active[%d] = %d",y,active[y]);
			// }

			for(i = 0; i<MAXPROCS; i++)
			{
				// Printf("\nproctab[%d].pid = %d  proctab[%d].valid = %d",i,proctab[i].pid,i,proctab[i].valid);
				if (proctab[i].pid == active[head] && proctab[i].valid) 
				{
					head++;
					if(head == tail)
						head = 0;	
					return(proctab[i].pid);
				}
			}

			//Case: no valid non-requesting processes
			//Solution: choose a valid requesting process with smalles n
			if(proctab[smallest_ratio_id_holder].valid)
			{
				proctab[smallest_ratio_id_holder].running_time++;
				return proctab[smallest_ratio_id_holder].pid;
			}
		}

		break;

	}
	
	return (0);
}


/*	HandleTimerIntr () is called by the kernel whenever a timer
 *	interrupt occurs.
 */

void HandleTimerIntr ()
{
	SetTimer (TIMERINTERVAL);
	int i;

	// Printf("\nHandleTimerIntr is called **\n");

	switch (GetSchedPolicy ()) {	/* is policy preemptive? */

	case ROUNDROBIN:		/* ROUNDROBIN is preemptive */
	case PROPORTIONAL:		/* PROPORTIONAL is preemptive */

		for (i = 0; i < MAXPROCS; i++) {
			if (proctab[i].valid) 
			{
				proctab[i].age++;
				float utilization = (float)proctab[i].running_time/(float)proctab[i].age;
				float requested = (float) proctab[i].m/ (float) proctab[i].n;
				proctab[i].ratio = utilization/requested;	
			}
		}

		//troubleshooting
		// for(int i = 0; i<MAXPROCS; i++)
		// {
		// 	Printf("\ni = %d  proctab[i].valid = %d  proctab[i].requested_cpu = %d",i,proctab[i].valid,proctab[i].requested_cpu);
		// 	Printf(" m = %d  n = %d  ",proctab[i].m,proctab[i].n);
		// 	Printf(" age = %d  running_time = %d  ",proctab[i].age,proctab[i].running_time);
		// }

		DoSched ();		/* make scheduling decision */
		break;

	default:			/* if non-preemptive, do nothing */
		break;
	}
}

/*	MyRequestCPUrate (pid, m, n) is called by the kernel whenever a process
 *	identified by pid calls RequestCPUrate (m, n).  This is a request for
 *	a fraction m/n of CPU time, effectively running on a CPU that is m/n
 *	of the rate of the actual CPU speed.  m of every n quantums should
 *	be allocated to the calling process.  Both m and n must be greater
 *	than zero, and m must be less than or equal to n.  MyRequestCPUrate
 *	should return 0 if successful, i.e., if such a request can be
 *	satisfied, otherwise it should return -1, i.e., error (including if
 *	m < 1, or n < 1, or m > n).  If MyRequestCPUrate fails, it should
 *	have no effect on scheduling of this or any other process, i.e., as
 *	if it were never called.
 */

int MyRequestCPUrate (pid, m, n)
	int pid;
	int m;
	int n;
{
	// Printf("\nMyRequestCPUrate called for %d\n", pid);

	if(m > n)
	{
		return (-1);
	}
	if(m <= 0 || n <= 0)
	{
		return (-1);
	}

	float requested = (float) m/(float) n;
	// Printf("\nm = %d, n = %d",m,n);
	// Printf("\nIts requested = %f",requested);
	
	// if(currentReq + requested  > 1)
	// 	return (-1);

	//find and update process in proctable
	for(int x = 0; x<MAXPROCS; x++)
	{
		if(proctab[x].pid == pid && proctab[x].valid)
		{
			//update currentReq (total CPU time given out)
			float old_requested = (float) proctab[x].m/(float) proctab[x].n;
			if((currentReq - old_requested + requested)  > 1){
				// Printf("!!!!CPU RATE SHITTED OUT FOR process i=%d \n",x);
				return (-1);
			}
				
			currentReq = (currentReq - old_requested + requested);
			// Printf("****CPU RATE CHANGED FOR process pii=%d \n",proctab[x].pid);

			// Printf("Before: proctab[%d].m = %d\n",x, proctab[x].m);
			// Printf("Before: proctab[%d].n = %d\n",x, proctab[x].n);
			proctab[x].m = m;
			proctab[x].n = n;
			// Printf("After: proctab[%d].m = %d\n",x, proctab[x].m);
			// Printf("After: proctab[%d].n = %d\n",x, proctab[x].n);
			proctab[x].requested_cpu = 1;

			//update active[] by removing the now requesting process
			int active_p = 0;
			while(active[active_p]!=pid)
			{
				active_p++;
			}

			for(int p = active_p; p<tail-1; p++)
			{
				active[p] = active[p+1];
			}
			active[tail-1] = 0;

			//also moves tail towards head by one
			tail--;

			

			// Printf("\nFound pid #%d and UPDATED", pid);

			return(0);
		}
	}

	// //enter this loop if process is not in proctable
	// //find and create an invalid process in proctable
	// for(int x = 0; x<MAXPROCS; x++)
	// {
	// 	if(!proctab[x].valid)
	// 	{
	// 		//update currentReq (total CPU time given out)
	// 		if((currentReq + requested)  > 1)
	// 			return (-1);
	// 		currentReq = currentReq + requested;

	// 		Printf("!!!!INSERTING NEW process i=%d FROM MyRequestCPUrate\n",x);

	// 		proctab[x].m = m;
	// 		proctab[x].n = n;
	// 		proctab[x].requested_cpu = 1;
	// 		// Printf("proctab[x].valid = %d",proctab[x].valid);
	// 		// Printf(" Changed requested_cpu of process %d to %d\n", pid,proctab[x].requested_cpu);

	// 		proctab[x].valid = 0;
	// 		proctab[x].pid = pid;

			

	// 		// Printf("\nCreating process with pid #%d", pid);

	// 		return (0);
	// 	}
	// }

	return (-1);
}
