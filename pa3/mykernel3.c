/* mykernel.c: your portion of the kernel
 *
 *	Below are procedures that are called by other parts of the kernel.
 *	Your ability to modify the kernel is via these procedures.  You may
 *	modify the bodies of these procedures any way you wish (however,
 *	you cannot change the interfaces).
 */

#include "aux.h"
#include "sys.h"
#include "mykernel3.h"

#define FALSE 0
#define TRUE 1

/*	A sample semaphore table.  You may change this any way you wish.
 */

static struct {

	int valid;	/* Is this a valid entry (was sem allocated)? */
	int value;	/* value of semaphore */
	int max;
	int head;
	int tail;
	int stack[MAXPROCS];

} semtab[MAXSEMS];


/*	InitSem () is called when kernel starts up.  Initialize data
 *	structures (such as the semaphore table) and call any initialization
 *	procedures here.
 */

void InitSem ()
{
	int s;

	/* modify or add code any way you wish */

	for (s = 0; s < MAXSEMS; s++) {		/* mark all sems free */
		semtab[s].valid = FALSE;
		semtab[s].head = 0;
		semtab[s].tail = 0;
	}

}

/*	MySeminit (p, v) is called by the kernel whenever the system
 *	call Seminit (v) is called.  The kernel passes the initial
 * 	value v, along with the process ID p of the process that called
 *	Seminit.  MySeminit should allocate a semaphore (find a free entry
 *	in semtab and allocate), initialize that semaphore's value to v,
 *	and then return the ID (i.e., index of the allocated entry).
 */

int MySeminit (p, v)
	int p, v;
{
	int s;

	/* modify or add code any way you wish */

	for (s = 0; s < MAXSEMS; s++) {
		if (semtab[s].valid == FALSE) {
			break;
		}
	}
	if (s == MAXSEMS) {
		Printf ("No free semaphores\n");
		return (-1);
	}

	semtab[s].valid = TRUE;
	semtab[s].value = v;
	semtab[s].max = v;

	return (s);
}

/*	MyWait (p, s) is called by the kernel whenever the system call
 *	Wait (s) is called.
 */

void MyWait (p, s)
	int p, s;
{
	/* modify or add code any way you wish */

	// //DEBUG
	// Printf("\n%d - semtab BEFORE WAIT:\n",p);
	// int s2 = s;
	// Printf("id: %d  valid: %d  value: %d.\n",s2,semtab[s2].valid,semtab[s2].value);
	
	// Printf("semtab[%d].stack:",s2);
	// int s3;
	// for (s3 = 0; s3 < 25; s3++) {
	// 	Printf("[%d]",semtab[s2].stack[s3]);
	// 	if(semtab[s2].head == s3)
	// 		Printf("h ");
	// 	if(semtab[s2].tail == s3)
	// 		Printf("t ");
	// 	Printf(",");
	// }
	// Printf("\n");
	// //END DEBUG

	semtab[s].value = semtab[s].value - 1;
	if(semtab[s].value < 0)
	{
		// Printf("Blocking process %d\n",p);

		//add proc to tail of stack
		int tail = semtab[s].tail;
		semtab[s].stack[tail] = p;
		
		// semtab[s].tail++;
		if(semtab[s].tail+1 == MAXPROCS)
		{
			if(semtab[s].head != 0)
			{
				semtab[s].tail = 0;
			}
		}
		else
		{
			// if(semtab[s].head != semtab[s].tail)
				semtab[s].tail++;
		}

		// //DEBUG
		// Printf("%d - semtab BLOCK AFTER WAIT:\n",p);
		// s2 = s;
		// Printf("id: %d  valid: %d  value: %d.\n",s2,semtab[s2].valid,semtab[s2].value);
		
		// Printf("semtab[%d].stack:",s2);
		// for (s3 = 0; s3 < 25; s3++) {
		// 	Printf("[%d]",semtab[s2].stack[s3]);
		// 	if(semtab[s2].head == s3)
		// 		Printf("h ");
		// 	if(semtab[s2].tail == s3)
		// 		Printf("t ");
		// 	Printf(",");
		// }
		// Printf("\n");
		// Printf("\n");
		// //END DEBUG

		Block(p);
	}

	// //DEBUG
	// Printf("%d - semtab AFTER WAIT -> semtab[%d].value = %d \n\n",p,s2,semtab[s2].value);
	// //END DEBUG
}

/*	MySignal (p, s) is called by the kernel whenever the system call
 *	Signal (s) is called.
 */

void MySignal (p, s)
	int p, s;
{
	/* modify or add code any way you wish */

		// //DEBUG
		// Printf("\n%d - semtab[%d] BEFORE SIGNAL:\n",p,s);
		// int s2 = s;
		// Printf("id: %d  valid: %d  value: %d.\n",s2,semtab[s2].valid,semtab[s2].value);
		
		// Printf("semtab[%d].stack:",s2);
		// int s3;
		// for (s3 = 0; s3 < 25; s3++) {
		// 	Printf("[%d]",semtab[s2].stack[s3]);
		// 	if(semtab[s2].head == s3)
		// 		Printf("h ");
		// 	if(semtab[s2].tail == s3)
		// 		Printf("t ");
		// 	Printf(",");
		// }
		// Printf("\n");
		// //END DEBUG	

	
	//remove proc at head of stack only if value < 0
	// if(semtab[s].value + 1 <= semtab[s].max)
	semtab[s].value++;
	if(semtab[s].value <= 0)//semtab[s].max)
	{
		int head = semtab[s].head;
		int wakingUp = semtab[s].stack[head];

		if(semtab[s].head+1 == MAXPROCS)
		{
			semtab[s].head = 0;
		}
		else
		{
			// if(semtab[s].head+1 != semtab[s].tail+1)
			semtab[s].head++;
		}

			// //DEBUG
			// Printf("%d: Waking up proc %d.\n",s,wakingUp);

			// Printf("\n%d - semtab[%d] AFTER SIGNAL:\n",p,s);
			// s2 = s;
			// Printf("id: %d  valid: %d  value: %d.\n",s2,semtab[s2].valid,semtab[s2].value);
			
			// Printf("semtab[%d].stack:",s2);
			// for (s3 = 0; s3 < 25; s3++) {
			// 	Printf("[%d]",semtab[s2].stack[s3]);
			// 	if(semtab[s2].head == s3)
			// 		Printf("h ");
			// 	if(semtab[s2].tail == s3)
			// 		Printf("t ");
			// 	Printf(",");
			// }
			// Printf("\n");
			// Printf("\n");
			// //END DEBUG	

		Unblock(wakingUp);
	}

		// //DEBUG
		// Printf("Nothing to wake up.\n");

		// Printf("\n%d - semtab[%d] AFTER SIGNAL:\n",p,s);
		// s2 = s;
		// Printf("id: %d  valid: %d  value: %d.\n",s2,semtab[s2].valid,semtab[s2].value);
		
		// Printf("semtab[%d].stack:",s2);
		// for (s3 = 0; s3 < 25; s3++) {
		// 	Printf("[%d]",semtab[s2].stack[s3]);
		// 	if(semtab[s2].head == s3)
		// 		Printf("h ");
		// 	if(semtab[s2].tail == s3)
		// 		Printf("t ");
		// 	Printf(",");
		// }
		// Printf("\n");
		// Printf("\n");
		// //END DEBUG	
	
}

