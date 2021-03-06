/* Programming Assignment 3: Exercise D
 *
 * Now that you have a working implementation of semaphores, you can
 * implement a more sophisticated synchronization scheme for the car
 * simulation.
 *
 * Study the program below.  Car 1 begins driving over the road, entering
 * from the East at 40 mph.  After 900 seconds, both Car 3 and Car 4 try to
 * enter the road.  Car 1 may or may not have exited by this time (it should
 * exit after 900 seconds, but recall that the times in the simulation are
 * approximate).  If Car 1 has not exited and Car 4 enters, they will crash
 * (why?).  If Car 1 has exited, Car 3 and Car 4 will be able to enter the
 * road, but since they enter from opposite directions, they will eventually
 * crash.  Finally, after 1200 seconds, Car 2 enters the road from the West,
 * and traveling twice as fast as Car 4.  If Car 3 was not coming from the
 * opposite direction, Car 2 would eventually reach Car 4 from the back and
 * crash.  (You may wish to experiment with reducing the initial delay of
 * Car 2, or increase the initial delay of Car 3, to cause Car 2 to crash
 * with Car 4 before Car 3 crashes with Car 4.)
 *
 *
 * Exercises
 *
 * 1. Modify the procedure driveRoad such that the following rules are obeyed:
 *
 *	A. Avoid all collisions.
 *
 *	B. Multiple cars should be allowed on the road, as long as they are
 *	traveling in the same direction.
 *
 *	C. If a car arrives and there are already other cars traveling in the
 *	SAME DIRECTION, the arriving car should be allowed enter as soon as it
 *	can. Two situations might prevent this car from entering immediately:
 *	(1) there is a car immediately in front of it (going in the same
 *	direction), and if it enters it will crash (which would break rule A);
 *	(2) one or more cars have arrived at the other end to travel in the
 *	opposite direction and are waiting for the current cars on the road
 *	to exit, which is covered by the next rule.
 *
 *	D. If a car arrives and there are already other cars traveling in the
 *	OPPOSITE DIRECTION, the arriving car must wait until all these other
 *	cars complete their course over the road and exit.  It should only wait
 *	for the cars already on the road to exit; no new cars traveling in the
 *	same direction as the existing ones should be allowed to enter.
 *
 *	E. This last rule implies that if there are multiple cars at each end
 *	waiting to enter the road, each side will take turns in allowing one
 *	car to enter and exit.  (However, if there are no cars waiting at one
 *	end, then as cars arrive at the other end, they should be allowed to
 *	enter the road immediately.)
 *	
 *	F. If the road is free (no cars), then any car attempting to enter
 *	should not be prevented from doing so.
 *
 *	G. All starvation must be avoided.  For example, any car that is
 *	waiting must eventually be allowed to proceed.
 *
 * This must be achieved ONLY by adding synchronization and making use of
 * shared memory (as described in Exercise C).  You should NOT modify the
 * delays or speeds to solve the problem.  In other words, the delays and
 * speeds are givens, and your goal is to enforce the above rules by making
 * use of only semaphores and shared memory.
 *
 * 2. Devise different tests (using different numbers of cars, speeds,
 * directions) to see whether your improved implementation of driveRoad
 * obeys the rules above.
 *
 * IMPLEMENTATION GUIDELINES
 * 
 * 1. Avoid busy waiting. In class one of the reasons given for using
 * semaphores was to avoid busy waiting in user code and limit it to
 * minimal use in certain parts of the kernel. This is because busy
 * waiting uses up CPU time, but a blocked process does not. You have
 * semaphores available to implement the driveRoad function, so you
 * should not use busy waiting anywhere.
 *
 * 2. Prevent race conditions. One reason for using semaphores is to
 * enforce mutual exclusion on critical sections to avoid race conditions.
 * You will be using shared memory in your driveRoad implementation.
 * Identify the places in your code where there may be race conditions
 * (the result of a computation on shared memory depends on the order
 * that processes execute).  Prevent these race conditions from occurring
 * by using semaphores.
 *
 * 3. Implement semaphores fully and robustly.  It is possible for your
 * driveRoad function to work with an incorrect implementation of
 * semaphores, because controlling cars does not exercise every use of
 * semaphores.  You will be penalized if your semaphores are not correctly
 * implemented, even if your driveRoad works.
 *
 * 4. Avoid starvation.  This is especially relevant when implementing the
 * Signal function.  If there are multiple processes that blocked on the
 * same semaphore, then a good policy is to unblock them in FIFO order.
 *
 * 5. Control cars with semaphores: Semaphores should be the basic
 * mechanism for enforcing the rules on driving cars. You should not
 * force cars to delay in other ways inside driveRoad such as by calling
 * the Delay function or changing the speed of a car. (You can leave in
 * the delay that is already there that represents the car's speed, just
 * don't add any additional delaying).  Also, you should not be making
 * decisions on what cars do using calculations based on car speed (since
 * there are a number of unpredictable factors that can affect the
 * actual cars' progress).
 *
 * GRADING INFORMATION
 *
 * 1. Semaphores: We will run a number of programs that test your
 * semaphores directly (without using cars at all). For example:
 * enforcing mututal exclusion, testing robustness of your list of
 * waiting processes, calling signal and wait many times to make sure
 * the semaphore state is consistent, etc.
 *
 * 2. Cars: We will run some tests of cars arriving in different ways,
 * to make sure that you correctly enforce all the rules for cars given
 * in the assignment.  We will use a correct semaphore implementation for
 * these tests so that even if your semaphores are not correct you could
 * still get full credit on the driving part of the grade.  Think about
 * how your driveRoad might handle different situations and write your
 * own tests with cars arriving in different ways to make sure you handle
 * all cases correctly.
 *
 *
 * WHAT TO TURN IN
 *
 * You must turn in two files: mykernel3.c and p3d.c.  mykernel3.c should
 * contain you implementation of semaphores, and p3d.c should contain
 * your modified version of InitRoad and driveRoad (Main will be ignored).
 * Note that you may set up your static shared memory struct and other
 * functions as you wish. They should be accessed via InitRoad and driveRoad,
 * as those are the functions that we will call to test your code.
 *
 * Your programs will be tested with various Main programs that will exercise
 * your semaphore implementation, AND different numbers of cars, directions,
 * and speeds, to exercise your driveRoad function.  Our Main programs will
 * first call InitRoad before calling driveRoad.  Make sure you do as much
 * rigorous testing yourself to be sure your implementations are robust.
 */

#include <stdio.h>
#include "aux.h"
#include "umix.h"

void InitRoad ();
void driveRoad (int from, int mph);

void Main ()
{
  InitRoad ();

  if (Fork () == 0) {
    Delay (0);
    driveRoad (EAST, 10);
    Exit ();
  }

  if (Fork () == 0) {
    Delay (0);
    driveRoad (WEST, 80);
    Exit ();
  }

  driveRoad (WEST, 5);

  Exit ();
}


/* Our tests will call your versions of InitRoad and driveRoad, so your
 * solution to the car simulation should be limited to modifying the code
 * below.  This is in addition to your implementation of semaphores
 * contained in mykernel3.c.
 */

static struct {		
	//road related semaphores
	int roadStat[11]; //array for holding road0->9 sem
	int road0;
	int road1;
	int road2;
	int road3;
	int road4;
	int road5;
	int road6;
	int road7;
	int road8;
	int road9;
	int changePos; //sem for p and np calculation critical section
	int cars_on_road; //counter for how many cars on the road
	int cars_on_road_sem; //sem for protecting cars_on_road counter

	//1st pos sem
	int west_queue; //id=12?
	int east_queue; //id=13?

	//ticket related sem
	int ticket; //id=14
	int west_gate; //id=15
	int east_gate; //id=16

} shm; 

void InitRoad ()
{
	Regshm ((char *) &shm, sizeof (shm));	/* register as shared */

	//initialize road related semaphores
	shm.changePos = Seminit(1);

	shm.road0 = Seminit (1);
	shm.road1 = Seminit (1);
	shm.road2 = Seminit (1);
	shm.road3 = Seminit (1);
	shm.road4 = Seminit (1);
	shm.road5 = Seminit (1);
	shm.road6 = Seminit (1);
	shm.road7 = Seminit (1);
	shm.road8 = Seminit (1);
	shm.road9 = Seminit (1);

	shm.roadStat[1] = shm.road0;
	shm.roadStat[2] = shm.road1;
	shm.roadStat[3] = shm.road2;
	shm.roadStat[4] = shm.road3;
	shm.roadStat[5] = shm.road4;
	shm.roadStat[6] = shm.road5;
	shm.roadStat[7] = shm.road6;
	shm.roadStat[8] = shm.road7;
	shm.roadStat[9] = shm.road8;
	shm.roadStat[10] = shm.road9;

	shm.cars_on_road = 0;
	//shm.cars_on_road_sem is initialized to 1 becuase only 1 process should be able to modify it at once
	shm.cars_on_road_sem = Seminit (1); 

	//initialize 1st position related semaphores
	shm.west_queue = Seminit(1);
	shm.east_queue = Seminit(1);

	//initialize ticket sem
	shm.ticket = Seminit(1);
	shm.west_gate = Seminit(1);
	shm.east_gate = Seminit(1);
}

#define IPOS(FROM)	(((FROM) == WEST) ? 1 : NUMPOS)



void driveRoad (from, mph)
	int from, mph;
{
	int c;					/* car id c = process id */
	int p, np, i;				/* positions */

	c = Getpid ();				/* learn this car's id */

	//check if first position is free
	//signal for the first position after moving from the first position
	if(from == WEST) //if from west
	{
		// Printf("%d - Wait(west_queue)\n",c);
		Wait(shm.west_queue);
	}
	else // if from east
	{
		// Printf("%d - Wait(east_queue)\n",c);
		Wait(shm.east_queue);
	}

	//ticket system -> check if it's your turn to go next
	Wait(shm.ticket); //queue car for the ticket
	//Note: at this point, the car has the ticket so...

	// Printf("%d - Direction of traffic is...",c);
	//check if the direction of traffic is right
	if(from == WEST) //if from west
	{
		// Printf("%d - ...Wait(west_gate)...\n",c);
		Wait(shm.west_gate);
		Signal(shm.west_gate);
	}
	else // if from east
	{
		// Printf("%d - ...Wait(east_gate)...\n",c);
		Wait(shm.east_gate);
		Signal(shm.east_gate);
	}
	// Printf("%d - ..direction is correct!\n",c);

	//Note: at this point, the car has the ticket and gate is open for us to go. so..
	//close the other side's gate only if first car on road

	Wait(shm.cars_on_road_sem);
	//increment counter for number of cars on road
	shm.cars_on_road++;
	// Printf("**Cars on road increased to %d\n",shm.cars_on_road);
	Signal(shm.cars_on_road_sem);

	if(shm.cars_on_road == 1)
	{
		//close the other side's gate ONLY if first car
		if(from == WEST) //if from west
		{
			// Printf("%d - Wait called on opposite/east gate.\n",c);
			Wait(shm.east_gate);
			// Printf("%d - Signal called on own/west gate.\n",c);
			// Signal(shm.west_gate);
		}
		else // if from east
		{
			// Printf("%d - Wait called on opposite/west gate.\n",c);
			Wait(shm.west_gate);
			// Printf("%d - Signal called on own/east gate.\n",c);
			// Signal(shm.east_gate);
		}
	}


	//------ from here, the car is already on the road ---->
	EnterRoad (from);

	PrintRoad ();
	Printf ("Car %d enters at %d at %d mph\n", c, IPOS(from), mph);

	//signal ticket available
	Signal(shm.ticket);
	// Printf("%d - Ticket is available.\n",c);

	for (i = 1; i < NUMPOS; i++) {

		int moving_to_2nd_position = 0;

		Wait(shm.changePos);
		if (from == WEST) {
			p = i;
			np = i + 1;
			//determine if car is moving away from first position
			if(np==2)
				moving_to_2nd_position = 1;

		} else {
			p = NUMPOS + 1 - i;
			np = p - 1;
			//determine if car is moving away from first position
			if(np==(NUMPOS-1))
				moving_to_2nd_position = 1;
		}
		Signal(shm.changePos);

		// Printf("**Car %d wants %d -> %d - Wait(%d)\n\n", c, p, np,p);

		Delay (3600/mph);
		Wait(shm.roadStat[np]);
		// Printf("**Car %d is moving %d -> %d.\n",c,p,np);
		ProceedRoad();

		//Signal that first position is open for respective direction
		if(moving_to_2nd_position == 1)
		{
			if(from == WEST)
			{
				// Printf("%d - Signal called on west queue.\n",c);
				Signal(shm.west_queue);
			}
			else
			{
				// Printf("%d - Signal called on east queue.\n",c);
				Signal(shm.east_queue);
			}
		}

		PrintRoad();
		Printf ("Car %d moves from %d to %d\n", c, p, np,p);
		if(moving_to_2nd_position == 0)
		{
			Signal(shm.roadStat[p]);
		}
			
	}

	Delay (3600/mph);
	ProceedRoad (); //after this, the car should have left the road

	//decrease the number of cars on the road
	Wait(shm.cars_on_road_sem);
	shm.cars_on_road--;
	// Printf("**Cars on road decreased to %d\n",shm.cars_on_road);
	Signal(shm.cars_on_road_sem);

	Signal(shm.roadStat[np]);
	PrintRoad ();
	Printf ("Car %d exits road\n", c);

	//if last car out, must signal for other side's gate to allow cars through
	if(shm.cars_on_road == 0)
	{
		//open the other side's gate ONLY if last car HAS LEFT
		if(from == WEST) //if from west
		{
			// Printf("%d - Signal east gate.\n",c);
			Signal(shm.east_gate);
			// Printf("%d - Signal opposite/east gate\n",c);
		}
		else // if from east
		{
			// Printf("%d - Signal west gate.\n",c);
			Signal(shm.west_gate);
			// Printf("%d - Signal opposite/west gate\n",c);
		}
	}
}
