/* Programming Assignment 3: Exercise A
 *
 * In this assignment, you will implement semaphores, and use them in a
 * program that simulates cars sharing a single-lane road and that require
 * synchronization.
 *
 * In this first exercise, we begin by studying a simulation program
 * that creates multiple processes, where each process simulates a car
 * entering, proceeding over, and exiting, a single-lane road.  The road
 * is 10 miles long, and consists of 10 positions (each of length 1 mile).
 *
 * Cars entering    ------------------------------------------    Cars entering
 * from the WEST -> | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 10 | <- from the EAST
 *                  ------------------------------------------
 *
 * Cars may enter the road from the West, at position 1, or they may enter
 * from the East, at position 10.  Cars enter and proceed at a certain speed
 * (expressed in MPH, miles per hour) and maintain that speed until they exit.
 *
 * The primary rule of the road is that each position can be occupied by
 * at most 1 car at any time.  If a car tries to proceed to a position
 * that is already occupied, a CRASH occurs.  When a crash occurs, both
 * cars get automatically removed from the road (allowing other cars to
 * proceed).
 *
 * In the program below, two cars drive over the road in the same direction,
 * both entering from the West (pos 1).  Car 1 drives at 40 MPH, and Car 2
 * at 60 MPH.  The procedure driveRoad simulates this activity: it takes
 * as parameters entrance point (EAST or WEST), and speed (in MPH).  To
 * make sure that both cars do not collide by entering the road at the
 * same time, Car 2 waits some time before entering.  This is implemented
 * by the procedure Delay (t) which takes a time (in seconds) as a parameter.
 * In the program, Car 2 delays by 900 seconds.  Since Car 1 drives at 40 MPH,
 * it will travel the 10-mile road in 15 minutes, or 900 seconds.  Hence,
 * Car 2 will enter at just about the time that Car 1 exits.
 *
 * Exercises
 *
 * 1. Run the program and note the output.  A line is printed each time a
 * car makes a move (either to enter, proceed, or exit, the road) or if a
 * collision occurs.
 *
 * 2. Since Car 2 is delayed by 900 seconds, it will have waited until Car 1
 * exits (note that times are NOT exact in the simulation, so Car 1 may or
 * may not exit right at the time Car 2 enters).  Modify the program so that
 * Car 2 delays by only 450 seconds.  What happens, and why?
 *
 * 3. Now modify the program so Car 2 delays by only 300 seconds.  What
 * happens, and why?  Run the program a few times to see if the same events
 * occur in the same way each time.
 *
 * 4. Modify the program so Car 2 delays by 0 seconds, just to see where
 * the crash occurs.
 *
 * 5. Now change the direction of Car 2 so that it enters from the East
 * (thus, starting at position 10).  Leave the delay at 0.  Where does the
 * crash occur, and why?
 *
 * 6. Still having both cars go in opposite directions, modify the delay so
 * that a crash does not occur.  How large does the delay have to be?
 *
 * 7. Study the implementation of driveRoad.  In addition to the Delay
 * procedure, it makes use of two other procedures, EnterRoad, and ProceedRoad.
 * EnterRoad causes a car to enter the road, and takes a parameter indicating
 * the car's point of entrance.  ProceedRoad simply causes the car to
 * move one position (one mile), in the direction it entered.  Notice how
 * movement is implemented by having the cars delay by 3600/mph, the number
 * of seconds in an hour divided by the speed in miles per hour.
 */

#include <stdio.h>
#include "aux.h"
#include "umix.h"

static struct {		
	int roadStat[11];

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
	

	int changePos;
} shm;

int driveRoad (int from, int mph);

void Main ()
{
	Regshm ((char *) &shm, sizeof (shm));	/* register as shared */

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

	if (Fork () == 0) {			/* Car 2 */
		Delay (200);
		driveRoad (WEST, 50);
		Exit ();
	}

	if (Fork () == 0) {			/* Car 3 */
		Delay (350);
		driveRoad (EAST, 60);
		Exit ();
	}

	if (Fork () == 0) {			/* Car 4 */
		Delay (500);
		driveRoad (EAST, 70);
		Exit ();
	}

	if (Fork () == 0) {			/* Car 5 */
		Delay (650);
		driveRoad (EAST, 80);
		Exit ();
	}

	if (Fork () == 0) {			/* Car 6 */
		Delay (800);
		driveRoad (EAST, 90);
		Exit ();
	}

	if (Fork () == 0) {			/* Car 7 */
		Delay (950);
		driveRoad (EAST, 100);
		Exit ();
	}

	if (Fork () == 0) {			/* Car 8 */
		Delay (1100);
		driveRoad (EAST, 110);
		Exit ();
	}

	driveRoad (EAST, 30);			/* Car 1 */

	Exit ();
}

#define IPOS(FROM)	(((FROM) == WEST) ? 1 : NUMPOS)

int driveRoad (from, mph)
	int from, mph;
{
	int c;					/* car id c = process id */
	int p, np, i;				/* positions */

	c = Getpid ();				/* learn this car's id */

	EnterRoad (from);

	PrintRoad ();
	
	Printf ("Car %d enters at %d at %d mph\n", c, IPOS(from), mph);

	for (i = 1; i < NUMPOS; i++) {

		Wait(shm.changePos);
		if (from == WEST) {
			p = i;
			np = i + 1;
		} else {
			p = NUMPOS + 1 - i;
			np = p - 1;
		}
		Signal(shm.changePos);

		// Printf("**Car %d wants %d -> %d - Wait(%d)\n\n", c, p, np,p);

		Delay (3600/mph);
		Wait(shm.roadStat[np]);
		// Printf("**Car %d is moving %d -> %d.\n",c,p,np);
		ProceedRoad();
		PrintRoad();
		Printf ("Car %d moves from %d to %d - Signal(shm.roadStat[%d])\n\n", c, p, np,p);
		Signal(shm.roadStat[p]);
	}

	Delay (3600/mph);
	ProceedRoad ();
	Signal(shm.roadStat[np]);
	PrintRoad ();
	Printf ("Car %d exits road\n", c);
}

int printRoadStatus()
{
	Printf("W");
	for(int x = 1; x<=10; x++)
	{
		if(shm.roadStat[x] == 1)
			Printf("1");
		else if(shm.roadStat[x] == 0)
			Printf("-");
	}
	Printf("E*\n");

}
