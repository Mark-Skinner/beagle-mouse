//
//  star.h
//  micromouse2014
//
//
// Created by Carlton Semple, Lyle Moffitt, & Nick Beni
// Created at Syracuse University for the 2014 micromouse competition

#ifndef __micromouse2014__star__
#define __micromouse2014__star__


#include <iostream>
#include <vector>
#include <stack>
#include <string>


#include "config.h"
#include "grid.h"


typedef
std::vector<cell*> path;

typedef
std::string string;

// contains 
class item{

};

/* This will simplify data input into 19 regions. 
The angle of the data from the lidar will determine the position in viewfinder, 
	and the distance/radius will be used to determine when an open space is coming up on the left/right, or a wall ahead.
The number of items in the viewfinder may be increased for better accuracy and increased response time*/
class local_grid{
	std::vector<double> viewfinder;
	std::vector<double> previous;
public:
	local_grid();

	// takes the grid from the lidar scan and simplifies the data
	void updateView(grid &fetched_scan, int &returnedLeft, int &returnedRight, double &returnedFront); 
};


class star
{
	// size of cells
	double lengthwidth;
	double threshold;	// distance at which it's determined the side walls aren't there
	double front_threshold; // about the length of a cell. 
							// when it's determined that an open side is on the left/right, this says if there is a wall in front or not

	grid maze;
	
	/*path to_orig;
	path guess;*/
	string direction; // direction that mouse is facing
	int rightTurns;
	int leftTurns;
	
	std::vector<cell *> traversed;

	/*********** Local Grid ************/
	int leftFromViewfinder; // will get the region that the open space (to the left) is currently at in relation to the lidar
	int rightFromViewfinder; // will get the region that the open space (to the right) is currently at in relation to the lidar
	double frontFromViewfinder; // gets the distance of the region directly in front
public:
	star();

	

	grid *get_scan(); // retrieves scan from lidar 
	cell * motion(grid &fetched_scan, cell &currentcell, path &junctions, string &direction); // local motion. calls the turn functions
	void changeDirection(int turn); // left turn = 1. right turn = 2

	void choose(cell &junction);

	int search(path &traversed, grid &fetched_scan);

	local_grid viewFinder;
};

#endif /* defined(__micromouse2014__star__) */
