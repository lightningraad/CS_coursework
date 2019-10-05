# Particle System

Perform a keytime animation.

## Requirements:
* Put this project number and your name in the title bar.
* Do a keytime animation. 
	* There need to be at least 6 keytimes that define the animation.
* The object to be animated is your choice of any 3D geometry. 
	* It must have sufficiently-good linework (or polygon-work) in it to be able to tell that it is rotating (a smooth sphere is not a good choice). 
	* Being a 3D object, each keytime position is defined by seven numbers: (t,x,y,z,thetax,thetay,thetaz), where t is the time in seconds.
* The (t,x,y,z,thetax,thetay,thetaz) values for each keytime are your choice and should be pre-defined in the program.
* Animate the object by smoothly interpolating (x,y,z,thetax,thetay,thetaz) between the keytimes. 
	* Use the smooth interpolation C++ class that we discussed.
* Display the object's keytime positions to see how well it passes through the keytime positions.