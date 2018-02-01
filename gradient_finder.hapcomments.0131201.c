#define _USE_MATH_DEFINES     	//**The define statement is the best way to handle fixed numbers

#include <stdio.h>
#include <math.h> //required to use 'M_PI' hap add: M_PI is a preprocessor definiton from line 1
#include <stdlib.h> //required to use 'rand()'
#include <time.h> //required to use 'srand(time(NULL))'

/* ************************* */
/* 
Routine: gradient_finder
Purpose: find the velcoity gradient via opposite square weight at any given location within the defined sphere
Coordinates: Spherical (radius {0, R}, theta {0, 2*pi}, phi {0, pi})
Unit: SI 
To-do: -  
*/
/* ************************* */
int main () {

  //Initializing rand()
  srand(time(NULL));		//**Seeding srnad from syatem clock. Just as an aside, using a fixed seed number gives an
				//identical sequence of pseudorandom numbers. May come in handy for troubleshooting...
  int rad_rand_max = 6;		//**I think these nest three values are fixed, ie the code does not change them. As such it
  int theta_rand_max = 3;	//is better to use #define (the preprocessor directive) rather than assign them as a variable
  int phi_rand_max = 2;		//That way, the code is faster, since it doesn't need to do a memory fetch each time it 
				//encounters them

  //Declaring mass and velocity arrays
  int particle_num = 10;	//** likewise, these numbers can just be #defined
  float radius_max = 10;	//** I hope this isn't 10 meters, which it seems to be if the usits are SI...
  float theta_max = 2*M_PI;	//** Defined in _USE_MATH_DEFINES as M_2_PI
  float phi_max = M_PI;
  
  float masses[particle_num];	//** I think it may be better to define a struct for the particle construct. Two reasons: it
				//makes the code a little easier to follow, and more important it is easier to add new masses
				// on the fly, since the masses on the fly using malloc and such. Also using a struct can be a 
				//faster, since each struct is in contigious memory, so are pretty much guaranteed to be in the
				//cache, which minimized multiclock memory fetches.
  
  float pos_radius[particle_num];
  float pos_theta[particle_num];
  float pos_phi[particle_num];
  
  float vel_radius[particle_num];
  float vel_theta[particle_num];
  float vel_phi[particle_num];

  for(int i=0; i < particle_num; i++)
  { 
    //Set initial radial positions for all particles
    pos_radius[i] = rand()%rad_rand_max;		
	  					
    //Set initial angular positions for all particles
    //Normalizing random numbers to theta and phi maxes
    float temp = rand()%theta_rand_max; //temp is between 0 and thera_rand_max ==3
    temp = temp/theta_rand_max;		//temp is between 0 and 1
    pos_theta[i] = 2*M_PI*temp; 	//pos_theta[i] is between 0 and 2_M_PI 
	  				//** OK, I see what you did here. It works, but it takes two division and two
	  				// multiplication operations per particle. 
	  				//I recommend this instead: out of the loop use #define and make constants equal to 
	  				//M_2_PI/RAND_MAX, so you will only need one multipication per assignment in the loop.
	  				//However, the preprocessor doesn't do arithmatic per se, so it may be necessary to use
	  				// a variable to hold the result of this calculation. Te advantage is the code will then
	  				//be wordsize independent, however, eadh assignment will still need to do a memory fetch, 
	  				//which costs clocks.  The way around this is to declare a constant variable. I think,
	  				//but am not sure, that the compiler will then just code the numner in-line rather than 
	  				//perforn the memory fetch operation each time it occurs.
    temp = rand()%phi_rand_max;
    temp = temp/phi_rand_max;
    pos_phi[i] = M_PI*temp;

    //Set initial radial and angular velocities for all particles
    //Keeps the velocities small enough that one time step won't take the particle out of the sphere
    temp = rand()%11;
    vel_radius[i] = 0.001*temp;
    temp = rand()%11;
    vel_theta[i] = 0.001*temp;
    temp = rand()%11;
    vel_phi[i] = 0.001*temp;      
  }

  //Temporal evolution
  float t_max = 10;			//0-10 sec
  float t_step;

  for(int j=0; j < 10*t_max; j++)	//J 0-100
  {
    t_step = 0.1*j;			//t_step is time in sec			
    int i = j/t_max;			//i is 0 for j = 0-9, 1 for j = 10-19, etc
	  				//Pollack's note to self: I think this does 10 moves for each particle, succesively
  
    pos_radius[i] = pos_radius[i] + vel_radius[i]*t_step;
    if (fabsf(pos_radius[i]) >= radius_max)
    {
	pos_radius[i] = pos_radius[i] - radius_max;
	vel_radius[i] = -1*vel_radius[i];
    }
      
    pos_theta[i] = pos_theta[i] + vel_theta[i]*t_step; 
    //Maintain periodicity 
    if (pos_theta[i] > theta_max)
    {
	pos_theta[i] = pos_theta[i] - theta_max;
    }
      
    pos_phi[i] = pos_phi[i] + vel_phi[i]*t_step;
    //Maintain periodicity 
    if (pos_phi[i] > phi_max)
    {
	pos_phi[i] = pos_phi[i] - phi_max;		//**Isn't this a bad bounce? Let 0 <= theta <=2*PI
	    						// and consider the plane where theta = 0. North will be "up"
	    						// and south "down", this axis determining a "polar" line
	    						// Then 0<= phi <= PI; as phi increases to be greater than PI
	    						// we cross the "polar" line, then phi decreases, but the point
	    						// is now in the theta = PI direction.
	    						// In otherwords, when phi increases beyond PI, three things happen:
	    						// 1) theta changes to theta + PI;
	    						// 2) phi becomes 2*PI - old_phi_greater_than_PI; and
	    						// 3) V_phi now changes sign, i.e. decreases if it was increasing
	    						// and VV.
    }

    for(int j=0; j < i; j++)				//I think this j is inside the other j loop. This may and may not
	    						//work depending on the complier. In general, a dangerous practice
	    						//better to use another index, like k.
	    						// in this setting it may work because every time this second loop
	    						//exits J is set to i, but I suspect this is serendipity rather
	    						//than by design.
    {
      if((pos_radius[i] == pos_radius[j]) && (pos_theta[i] == pos_theta[j]) && (pos_phi[i] == pos_phi[j]))
      {
	//Case where each particle is moving in a different direction
	if(((vel_radius[i] < 0) && (vel_radius[j] > 0)) || ((vel_radius[i] > 0) && (vel_radius[j] < 0)))
	{
	  vel_radius[i] = -1*vel_radius[i];
	  vel_radius[j] = -1*vel_radius[j];
	  
	  vel_theta[i] = -1*vel_theta[i];
	  vel_theta[j] = -1*vel_theta[j];
	  
	  vel_phi[i] = -1*vel_phi[i];
	  vel_phi[j] = -1*vel_phi[j];
	}
	//Case where both particles are moving in the same direction, but one has a higher velocity
      }
    }
  }
  
  return(0);
}
