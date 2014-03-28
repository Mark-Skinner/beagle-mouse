//
//  nav.h
//
// Created by Carlton Semple, Lyle Moffitt, & Nick Beni
// Created at Syracuse University for the 2014 micromouse competition
//

#ifndef _micromouse__nav_
#define _micromouse__nav_


#include "motor.h"
#include "encoder.h"

#include "lidar.h"
#include "packet.h"

#include <iostream>
#include <tuple>

#define DIV(a,b) ( b==0 ? a : double(a)/double(b) )
#define AVG(a,b) ( 0.5 * double(a) + 0.5 * double(b) )

namespace nav_config
{
	constexpr double max_speed = 1; 
	constexpr double min_speed = 0;
	constexpr double min_negspeed = 0;
	constexpr double max_negspeed = 1;
	constexpr double max_invariance = 0.0001;
	constexpr double min_invariance = 0.00000001;
	constexpr size_t duration = 10000000;
    
    constexpr uint wheel_diameter = 42;//mm
    
    constexpr uint unit_per_rot = 45;
    
    static inline
    bool eqish(double a, double b)
    {
        return (abs(DIV(a,b))-1<=(0+min_invariance) &&
                abs(DIV(a,b))-1<=(0-min_invariance) ? true:false); 
    }
    
    

}
//change

class nav
{
private:
	motor left; 
	motor right;
    
    encoder enc;
    
    lidar& view;

public:
    /* ____Primary methods:____ require specific arguments */
    
    // Class constructor for navigation
    nav(lidar & lidar_ptr);

    // moves a distance in terms of mm 
	void movedistancevariable(size_t mm);
    
	// makes sure wheels are moving in step at same speed 
    bool synchronize(double speed);
	 
	// assuming the front middle is 0 degrees; positive == clockwise 
	void turn(double angle);

    // move at constant velocity
    void move(double speed);
    
    /** veers the rover to the left based on ratio of speeds
     Positive:  right is increased relative to left
     Negative:  left is decreased relative to right
     */
	void veerleft(double ratio);
	
	/** veers the rover to the right based on ratio of speeds
     Positive:  left is increased relative to right
     Negative:  right is decreased relative to left
     */
	void veerright(double ratio);
    
    
    /* ____Secondary methods:____ call primaries with constant values */
    
	// go forward one unit cell and stop
	void goForwardOne();
	
	// turns rover 90 deg to the left (static)
    inline
	void turnleft(){    turn(-90);  }
	
	// turns rover 90 deg to the right  (static)
    inline
	void turnright(){   turn(90);   }
	
	//spins the rover around 180 deg
    inline
	void turnaround(){  turn(180);  }
    
	//stops motor 
	void stop();


	// keep the robot facing straight - in parallel with the walls along the sides of it
	void stayStraightLoop();
	
	/** the following each return true if they find that the robot isn't going straight**/
	bool straightAhead(int &leftORright);
	bool oneSidedApproach();
	bool twoSidedApproach();
		
};





#endif //_micromouse__nav_

