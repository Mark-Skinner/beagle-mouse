//
//  star.cpp
//  micromouse2014
//
//

#include "star.h"
#include "cell.h"
#include "packet.h"
#include "wall.h"
#include <math.h>
#include <iostream>

#define cellsize 16
bool deadend = false;




/********** Star ********/
star::star()
{
	
	maze.direction = "north";
	maze.compass = 90; // starting position is 90 degrees, which will be "north". 0 degrees is to the left to match up with the lidar
	maze.shift = 0.0; // default position, so shift is 0 degrees
    
	maze.startingRow = 0;
	maze.startingColumn = 0;
	atJunction = false;


	// The goal cell's attributes, assuming the goal is always the center
	maze.goalX = (maze.mazeSize / 2) * cellsize;	// halfway through the maze, multiplied by the centimeters in each cell
	maze.goalY = maze.goalX;

	rightTurns = 0;
	leftTurns = 0;

	// create the maze
	maze.createMaze();

	/********* Give the cells their heuristicCost, the estimated cost to the goal ******/


	// scan and update the maze
	while (true)
	{
		scan();	headOnDistance = -1; // set headOnDistance to the average of the packets that represent what is directly in front. compensate with compass
		maze.updateMaze();
		goForwardOne();
		scan();	headOnDistance2 = -1;
		maze.updateMaze();
		rob.PositionChange();

		decide(); // decide will check current spot, choose to go forward, left, right, blah blah
	}

	

}

bool closeEnough(double angle1, double angle2)
{
	if (abs(angle2 - angle1) <= 1.0)
		return true;
	else
		return false;
}


void polarToCartesian(double radius, double angle, double &x, double &y)
{
	x = radius * cos(angle);
	y = radius * sin(angle);
}

void star::scan()
{
    using namespace star_config;
	vision.empty(); // empty the previous vision. new scan
	vect nums; // used to separate the measurements into degrees, 4 numbers per degree
	for (int i = 0; i < 1440; i++) // 1440 measurements total
	{
		// read in 
		double fake;
		double radius;

		if ((i > 180) && (i < 540)) // between angle 45 and 135. might need to expand in order to scan more at once & create the maze faster
		{
			nums.push_back(fake);
			if (nums.size() == 4)
			{
				packet *pack = 
				nums.empty();
				vision.push_back(pack);
			}
		}
	}
}





/*********** movement **********/




// decision-making at junctions. updates the map with new scans from the middle of the junction.
// does a breadth search to guess which path is faster, and gives movement costs based on this search.
// Then a breadth search from the goal to the current position is performed, updating the heuristic costs
int star::decide()
{
	double moveDistance;
	cell *currentCell = maze.findCell(maze.xDistance, maze.yDistance);

	/******************* Find the next upcoming junction *************************/
	char sourceDirection;
	cell *cellPoint = maze.getPointerToJunction(sourceDirection);

		int rowDifference, columnDifference, difference;
		// difference is either in columns or rows
			rowDifference = abs(currentCell->row - cellPoint->row);		columnDifference = abs(currentCell->column - cellPoint->column);
		if (rowDifference > 0)
			difference = rowDifference;
		else if (columnDifference > 0)
			difference = columnDifference;

	// An upcoming junction has been found; move to it
	if (cellPoint->declareSidesOpen(sourceDirection))
	{
		// get to the middle of the junction, as in inside the junction itself
		moveDistance = cellsize * difference;

		/***** MOVE THIS MUCH *****/
		for (int i = 0; i < moveDistance; i++)
			goForwardOne();

		// declare that we are at a junction. inside the junction. Don't scan. Turn first
		atJunction = true;
	}

	
	if (atJunction == true)
	{
		// no need to turn, just check latest scan to see and update the grid even MORE
		// READ SCAN
		scan();
		maze.updateMaze();
    }
	return 0;
}

void star::pushChildCellsToDeque(std::deque<cell*> &childCells)
{

}

void star::breadthSearch()
{
	int countCost = 1; // the movement cost increments every time you get to a new set of children
	cell *currentCell = maze.findCell(maze.xDistance, maze.yDistance);


	std::deque<cell*> childCells; // used for the breadth search, contains the current child cells. queue is not supported for some reason, so use in the same way. push_back, pop front



	/********** CLEAR CELL VALUES ***********/







	/****************************************/



	// look at child cells
	cell *cellP = currentCell;
	int north, south, east, west;
	cellP->returnSides(north, south, east, west);

	int i = 1;

	// add any cell as long as there's no wall between it and the current cell pointer
	// add their movement costs
	do{
		if (north < 1)
		{
			if (!cellP->north->goalCell)
			{
				cellP->north->movementCost = countCost;
				cellP->north->sourceDirection = 'n';
				childCells.push_back(cellP->north);
			}
		}
		if (west < 1)
		{
			if (!cellP->west->goalCell)
			{
				cellP->west->movementCost = countCost;
				cellP->west->sourceDirection = 'w';
				childCells.push_back(cellP->west);
			}
		}
		if (south < 1)
		{
			if (!cellP->south->goalCell)
			{
				cellP->south->movementCost = countCost;
				cellP->south->sourceDirection = 's';
				childCells.push_back(cellP->south);
			}
		}
		if (east < 1)
		{
			if (!cellP->east->goalCell)
			{
				cellP->east->movementCost = countCost;
				cellP->east->sourceDirection = 'e';
				childCells.push_back(cellP->east);
			}
		}

		// move the pointer to one of the next child cells in the 'queue'
		cellP = childCells.front();
		childCells.pop_front(); // remove the child cell from the queue

		// return the sides of the child cell excluding the source side
		cellP->returnSides(north, south, east, west, cellP->sourceDirection);

		i++;
		countCost++;
	} while ((i < ((maze.mazeSize*maze.mazeSize) - 4)));	// do every cell except the goal cell


	//give the cells their new heuristic cost *************************
	// WORKING from the goal to the current position

	// look at child cells
	cellP = maze.findClosestGoalCell(cellP->x_center, cellP->y_center);
	int north, south, east, west;
	cellP->returnSides(north, south, east, west);

	countCost = 1;
	i = 1;

	// add any cell as long as there's no wall between it and the current cell pointer
	// add their movement costs
	do{
		if (north < 1)
		{
			if (!cellP->north->goalCell)
			{
				cellP->north->heuristicCost = countCost;
				cellP->north->sourceDirection = 'n';
				childCells.push_back(cellP->north);
			}
		}
		if (west < 1)
		{
			if (!cellP->west->goalCell)
			{
				cellP->west->heuristicCost = countCost;
				cellP->west->sourceDirection = 'w';
				childCells.push_back(cellP->west);
			}
		}
		if (south < 1)
		{
			if (!cellP->south->goalCell)
			{
				cellP->south->heuristicCost = countCost;
				cellP->south->sourceDirection = 's';
				childCells.push_back(cellP->south);
			}
		}
		if (east < 1)
		{
			if (!cellP->east->goalCell)
			{
				cellP->east->heuristicCost = countCost;
				cellP->east->sourceDirection = 'e';
				childCells.push_back(cellP->east);
			}
		}

		// move the pointer to one of the next child cells in the 'queue'
		cellP = childCells.front();
		childCells.pop_front(); // remove the child cell from the queue

		// return the sides of the child cell excluding the source side
		cellP->returnSides(north, south, east, west, cellP->sourceDirection);

		i++;
		countCost++;
	} while ((i < ((maze.mazeSize*maze.mazeSize) - 4)));	// do every cell except the current cell







	/********* depth search ***********/

	// start at current cell once again
	cell *cellP = maze.findCell(maze.xDistance, maze.yDistance);
	int unknownSides = 0;



}

// mode 1 = go with the "known" path
// mode 2 = go with the "unknown" path
int star::depthSearch(cell &sender, cell &current, std::stack<cell*> &tempStack, std::deque<cell*> &pathKnown, std::deque<cell*> &pathUnknown, int &unknownSides, int mode)
{
	// prepare to check walls
	int north, south, east, west;

	// check for dead end. if it's a dead end, return
	current.returnSides(north, south, east, west);
	int walls = north + south + east + west;
	if (walls >= 3)
		return -1;	// dead end

	


	/*if (north == 0)
		unknownSides++;
	if (south == 0)
		unknownSides++;
	if (east == 0)
		unknownSides++;
	if (west == 0)
		unknownSides++;

		*/

	std::stack<cell*> tempStack;
	int result;

	cell *closestGoalCell = maze.findClosestGoalCell(current.x_center, current.y_center);
	double x = current.x_center - closestGoalCell->x_center;
	double y = current.y_center - closestGoalCell->y_center;
	double distance = sqrt((x*x) + (y*y));
	//if (distance < cellsize) // if they're the same, or close to it...
	//{
	//	// this path will be c
	//}

	double lowestSum;
	cell *lowestNeighbor; // add the neighbor with the lowest sum to the path

	switch (mode)
	{
	case 1:{	// go with the known path
			   while (true)
			   {

			   }

			   int confirmedSides = 0;
			   if (north == -1)
			   {
				   if (!current.north->goalCell)
				   {
					   lowestSum = current.north->returnSum();
					   lowestNeighbor = current.north;
					   confirmedSides++;
				   }
			   }
			   if (south == -1)
			   {
				   if (!current.south->goalCell)
				   {
					   if (current.south->returnSum() <= lowestSum)
						   lowestNeighbor = current.south;
					   confirmedSides++;
				   }
			   }
			   if (east == -1)
			   {
				   if (!current.east->goalCell)
				   {
					   if (current.east->returnSum() <= lowestSum)
						   lowestNeighbor = current.east;
					   confirmedSides++;
				   }
			   }
			   if (west == -1)
			   {
				   if (!current.west->goalCell)
				   {
					   if (current.west->returnSum() <= lowestSum)
						   lowestNeighbor = current.west;
					   confirmedSides++;
				   }
			   }
			   if (mode == 1) // "known" mode needs an ending condition
			   {
				   if (confirmedSides == 0)
			   }
			   
			   pathKnown.push_back(lowestNeighbor);

			   // search through this child element
			   result = depthSearch(current, *current.north, tempStack, pathKnown, pathUnknown, unknownSides, mode);

	}	break;
	case 2:{	// go with the unknown path
			   if (north < 1)
			   {
				   if (!current.north->goalCell)
				   {
					   tempStack.push(current.north);

					   // search through this child element
					   result = depthSearch(current, *current.north, tempStack, pathKnown, pathUnknown, unknownSides, mode);
				   }
			   }
			   if (south < 1)
			   {
				   if (!current.south->goalCell)
				   {
					   tempStack.push(current.south);

					   // search through this child element
					   result = depthSearch(current, *current.south, tempStack, pathKnown, pathUnknown, unknownSides, mode);
				   }
			   }
			   if (east < 1)
			   {
				   if (!current.east->goalCell)
				   {
					   tempStack.push(current.east);

					   // search through this child element
					   result = depthSearch(current, *current.east, tempStack, pathKnown, pathUnknown, unknownSides, mode);
				   }
			   }
			   if (west < 1)
			   {
				   if (!current.west->goalCell)
				   {
					   tempStack.push(current.west);

					   // search through this child element
					   result = depthSearch(current, *current.west, tempStack, pathKnown, pathUnknown, unknownSides, mode);
				   }
			   }
	}
	default:
		break;
	}

}

void star::determineMovementCost(cell &ce)
{
	cell * cellIt;
	cellIt = maze.findCell(0, 0);


	// this isn't used, i don't think
}

// find the initial heuristic cost for all of the cells
void star::determineheuristicCost()
{
	// it's okay to measure from the exact center rather than the range of the cell in the center
	maze.goalX = maze.mazeSize * cellsize * 0.5;
	maze.goalY = maze.goalX;


}



