//
//  nav.cpp
//
// Created by Carlton Semple, Lyle Moffitt, & Nick Beni
// Created at Syracuse University for the 2014 micromouse competition
//


#include "nav.h"


using namespace std;

// Defualt constructor
nav::nav(lidar & lidar_ptr):
view(lidar_ptr),
left(motor_config::m_select::LEFT),
right(motor_config::m_select::RIGHT)
{
    
}


void 
nav::movedistancevariable(size_t mm)
{
    
}


bool 
nav::synchronize(double speed) // should go in go forward
{
    using namespace encoder_config;
    using namespace nav_config;
    snapshot * state = enc.hist.front();
    auto v_L = abs(get<1>(*state)->_vel);
    auto v_R = abs(get<2>(*state)->_vel);
    auto v_avg = AVG(v_L , v_R);
    
    auto s_L = left.get_speed();
    auto s_R = right.get_speed();
    auto s_avg = AVG(s_L , s_R);
    
    uint8_t sw_t = 0;
    sw_t |=  (eqish(v_avg,speed)    ? 0x01:0);
    sw_t |=  (speed < 0             ? 0x02:0);
    
    if ( eqish(v_avg,speed) )
    {  
        left.set_speed (s_avg);
        right.set_speed(s_avg);
        return true;
    }
    if (v_L<v_R && v_avg < speed){
        left.set_speed (s_L+max_invariance);
        right.set_speed(s_R-max_invariance);
        return true;
    }else{
        left.set_speed (s_L-max_invariance);
        right.set_speed(s_R+max_invariance);
    }
}


// turn by so many degrees, determined by time
// assuming the front middle is 0 degrees
void 
nav::turn(double angle)
{
	double middle; // just here temporarily
	double degrees = middle - angle;
	double compass;
	if (degrees > 0) // turn left
	{
		if (ceil(degrees) == 0) //  go straight
		{
            
		}
		else
		{
			// turn left, degrees per second
            
			compass += degrees;
		}			
	}
	else if (degrees < 0) // turn right
	{
		if (floor(degrees) == 0) // go straight
		{
            
		}
		else
		{
			// turn right, degrees per second
            
			compass += degrees;
		}
	}
}

void
nav::move(double speed)
{
    
}

void 
nav::veerleft(double ratio)
{
    moveforward();
	left.disable();
	XLR8();
	left.enable();
}


void 
nav::veerright(double ratio)
{
    moveforward();
	right.disable();
	XLR8();
	right.enable();
}


// go forward in a small enough chunk (one cell approx.) so that the distance to the opposing wall can be scanned/updated
void 
nav::goForwardOne()
{
    double val; // give a specifc amount 
                //left motor 
                // right motor both need to advance so both are going in the same direction
	left.enable();
	right.enable();
    
    left.set_speed();
    right.set_speed();
    
    
    left.get_speed();
    right.get_speed();
    
    synchronize();
}


void 
nav::stop()
{
	while(left.chk_en() || right.chk_en() == 1)
	{
	left.disable();
	right.disable();
	}
}






