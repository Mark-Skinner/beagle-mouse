
//
//  wall.h
//  micromouse2014
//
//
// Created by Carlton Semple, Lyle Moffitt, & Nick Beni
// Created at Syracuse University for the 2014 micromouse competition
//

#ifndef micromouse_wall_h
#define micromouse_wall_h


#include <cstddef>

namespace wall_config
{
    // standard thickness of wall
    constexpr size_t thickness = 12;//mm
    
    // maximum variance of measurements
    constexpr float variance = 0.05;// percent (%)
}


struct wall
{

	double length;
	double viewAngle; // angle between the two vectors
	double leftAngle;
	double rightAngle;

    // the measurements that helped create the wall
    double radiusLeft;
    double radiusRight;
    double a1;
    double a2;
    
    double xBegin, xEnd; // when the wall is computed, this is the final step in capturing where it is in relation to cells
    double yBegin, yEnd;
    
    double topTriangleAngle; // used when computing the parallel/perpendicular distances. it's the viewAngle 
    double perpendicularDistance;
    double parallelDistance;
    
	wall() { length = 0; }
	wall(double difference) { length = difference; }
	wall(double radius1, double radius2, double angle1, double angle2);
	
	
};

#endif// micromouse_wall_h

