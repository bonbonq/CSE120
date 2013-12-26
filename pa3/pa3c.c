/* Programming Assignment 3: Exercise C
 *
 * How to Use Shared Memory
 *
 * Processes in Umix are able to share memory.  An explicit mechanism is
 * needed because the default for processes is that, as in Unix, their
 * memories are independent of each other, with no portion being shared.
 * When a Fork occurs, a new process is created with its own memory that
 * is initialized with the state of the memory of the creating process.
 * But from that point on, when one of the processes modifies its private
 * memory, the private memory of the other process is unaffected.  Hence,
 * the need for a shared memory area.
 *
 * Two processes can share memory by identifying and registering variables
 * to be shared.  The variables in each process should be the same size
 * and type.  To share a set of variables, one can simply define a struct
 * containing the variables as members and then share a variable declared
 * as the struct.  The following example will clarify this.
 *
 * For two processes to share, say, three integers, a struct should be
 * defined in both programs as follows:
 *
 * struct {
 *	int x, y, z;
 * } shm;
 *
 * This structure defines three integers x, y, and z, as its members, and
 * the variable shm is declared of this structure's type.
 *
 * Within the program, shm must be registered with the operating system
 * as a shared variable using the Regshm system call:
 *
 *	Regshm ((char *) &shm, sizeof (shm))
 *
 * The first parameter is the address of the shared (struct) variable, 
 * and the second is its size.  The latter is needed because there is a
 * maximum size (MAXSHM) that the operating system supports for shared
 * memory.
 *
 * From then on, you can reference any of the shared variables via the
 * expression shm.x, where shm is the name of the addressing variable
 * and x is a member of the shared variable structure.
 *
 * Variables that become shared memory should NOT be used prior to the call
 * to Regshm (or if they are used, their values will be wiped away after
 * the Regshm returns).  Consequently, all initialization of shared memory
 * variables should happen AFTER the call the Regshm, and not before.
 *
 * If a process has a registered shared memory area, then any processes
 * it creates will inherit the shared memory, i.e., the child processes
 * do not have to call Regshm.  Otherwise, processes need to each call
 * Regshm to share memory.
 *
 * If Regshm is called more than once, the latest registration supercedes
 * the previous ones (i.e., only the last one matters, the others are
 * ignored).  Typically, all the variables that should be shared are
 * placed in a struct as shown above, and then registered once, with
 * no more registrations.
 *
 * A sample program is given below.  Note that the output of the program
 * is actually indeterminate because without any synchronization (e.g.,
 * semaphores), Process 2 may print before Process 1 sets the variables.
 */

#include <stdio.h>
#include "aux.h"
#include "umix.h"

struct {		/* structure of variables to be shared */
	int x;		/* example of an integer variable */
	char y[10];	/* example of an array of character variables */
} shm;

void Main ()
{
	Regshm ((char *) &shm, sizeof (shm));	/* register as shared */

	if (Fork () == 0) {			/* Process 2 */

		/* Proc 2 inherits the shared memory registered by Proc 1 */

		Printf ("P2: x = %d, y[3] = %c\n", shm.x, shm.y[3]);
		Exit ();
	}

	shm.x = 1062;		/* Process 1 */
	shm.y[3] = 'a';
	Printf ("P1: x = %d, y[3] = %c\n", shm.x, shm.y[3]);
}




/* Mutual Exclusion: The Dining Philosophers
 *
 * One synchronization issue presented during lecture was The Dining
 * Philospher problem. The problem goes as follows (from Wikipedia):
 *
 * Five silent philosophers sit at a table around a bowl of spaghetti.
 * A fork is placed between each pair of adjacent philosophers.
 *
 * Each philosopher must alternately think and eat. However, a philosopher
 * can only eat spaghetti when he has both left and right forks.
 *
 * Each fork can be held by only one philosopher and so a philosopher
 * can use the fork only if it's not being used by another philosopher.
 *
 * After he finishes eating, he needs to put down both the forks so they
 * become available to others.
 *
 * A philosopher can grab the fork on his right or the one on his left
 * as they become available, but can't start eating before getting both
 * of them.
 *
 * The problem occurs when each philosopher picks up a fork at once. They
 * will notice that their second fork is being held by another philosopher.
 * As such none of them can eat, and so they all put down their fork.
 *
 * WARNING: BE PROACTIVE IN TESTING YOUR CODE!
 *          It is your responsibility to proactively come up with
 *          whatever tests you think are necessary to convince
 *          yourself that your kernel is doing what is asked for
 *          in the specification.
 *
 *          Just because your code "works" with this test case is by
 *          no means proof that your kernel is completely correct!
 */
// #include <stdio.h>
// #include "aux.h"
// #include "umix.h"

// #define WAIT_MUTEX (5)
// #define WAIT_EAT (5)

// typedef int sem; // differentiate ints from semaphores

// static struct {
//   int fork[5];  // Who owns each fork at the moment?
//   int eaten[5];  // How many times has philosopher i - 1 eaten?

//   sem mutex;  // Semaphore for picking up and putting down forks
//   sem eat;  // Semaphore for eating... just to check > 1 semaphore
//   //sem think;  // Semaphore for thinking (not needed but can test > 1 sem)
// } shm;

// void Think ();
// void PickupFork ();
// void PutdownFork ();
// void Eat ();

// void DiningPhilosophers()
// {
//   int i = 0;

//   // Attempt to think and eat 10 times
//   while (i++ < 10) {
//     //Wait (shm.eat);
//     Think ();
//     //Signal (shm.eat);

//     /* Critical Section 1 */
//     Wait (shm.mutex);
//     PickupFork (Getpid () - 1);
//     PickupFork (Getpid () % 5);
//     Signal (shm.mutex);

//     /* "Critical Section" 2 (TECHNICALLY it isn't, but let's say it is) */
//     Wait (shm.eat);
//     Eat ();
//     Signal(shm.eat);

//     /* Critical Section 3 */
//     Wait (shm.mutex);
//     PutdownFork (Getpid () % 5);
//     PutdownFork (Getpid () - 1);
//     Signal (shm.mutex);

//     // After 3 times of successfully eating, the philosopher is full!
//     if (shm.eaten[Getpid () - 1] == 3) {
//       return;
//     }
//   }
// }

// void Main ()
// {
//   int i, j;

//   // initialize variables
//   for (i = 0; i < 5; i++) {
//     shm.fork[i] = 0;
//     shm.eaten[i] = 0;
//   }

//   //shm.think = Seminit (1);

//   // initialize the semaphores
//   shm.mutex = Seminit (1);
//   shm.eat = Seminit (1);

//   Regshm ((char *) &shm, sizeof (shm));	/* register as shared */

//   if (Fork () == 0) { // Philosopher 2
//     DiningPhilosophers ();
//     Exit ();
//   }

//   if (Fork () == 0) { // Philosopher 3
//     DiningPhilosophers ();
//     Exit ();
//   }

//   if (Fork () == 0) { // Philosopher 4
//     DiningPhilosophers ();
//     Exit ();
//   }

//   if (Fork () == 0) { // Philosopher 5
//     DiningPhilosophers ();
//     Exit ();
//   }

//   // Philosopher 1
//   DiningPhilosophers ();
//   Exit ();
// }

// void Think ()
// {
//   Printf ("PHILOSOPHER %d IS THINKING\n", Getpid ());
// }

// void Eat ()
// {
//   int fork1, fork2;
//   fork1 = shm.fork[Getpid () % 5];
//   fork2 = shm.fork[Getpid () - 1];

//   // Check who owns the fork. If both are by current philosopher, eat!
//   if (fork1 == fork2 && fork1 == Getpid ()) {
//     SlowPrintf (WAIT_EAT, "***PHILOSOPHER %d HAS EATEN %2d TIMES!***\n",
// 		Getpid (), ++shm.eaten[Getpid () - 1]);

//     if ( shm.eaten[Getpid () - 1] == 3) {
//       Printf ("***PHILOSOPHER %d IS NOW FULL!***\n", Getpid ());
//     }
//   }

//   SlowPrintf (WAIT_EAT, "PHILOSOPHER %d COULD NOT EAT!\n", Getpid ());
// }

// void PickupFork (p)
//      int p;
// {
//   // if fork is not owned by any philosopher, pick it up
//   if (shm.fork[p] == 0) {
//     SlowPrintf (WAIT_MUTEX, "PHILOSOPHER %d PICKED UP FORK %d!\n",
// 		Getpid (), p + 1);
//     shm.fork[p] = Getpid ();
//   } else { // otherwise complain that his fork is being used
//     SlowPrintf (WAIT_MUTEX, "PHILOSOPHER %d HAS PHILOSOPHER %d'S FORK!\n",
// 		shm.fork[p], Getpid ());
//   }
// }


// void PutdownFork (p)
//      int p;
// {
//   // Put down the fork if its owned by current philosopher
//   if (shm.fork[p] == Getpid ()) {
//     SlowPrintf (WAIT_MUTEX, "PHILOSOPHER %d PUT DOWN FORK %d!\n", Getpid (),
// 		p + 1);
//     shm.fork[p] = 0;
//   }
// }