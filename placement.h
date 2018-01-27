#ifndef PLACEMENT_H
#define PLACEMENT_H

#include<iosfwd>
#include<vector>

class State;

#define CUBE_PLACEMENTS(X)\
	X(red_switch)\
	X(blue_switch)\
	X(scale)\
	X(vault)

//all the places that a single alliance might want to place a cube.
struct Cube_placement{
	#define X(A) unsigned A;
	CUBE_PLACEMENTS(X)
	#undef X
};

std::ostream& operator<<(std::ostream&,Cube_placement);

std::vector<Cube_placement> cube_placements(State,unsigned cubes);

#endif
