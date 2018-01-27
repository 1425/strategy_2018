#include "placement.h"
#include "state.h"

using namespace std;

ostream& operator<<(ostream& o,Cube_placement a){
	o<<"Cube_placements( ";
	#define X(A) o<<""#A<<":"<<a.A<<" ";
	CUBE_PLACEMENTS(X)
	#undef X
	return o<<")";
}

vector<Cube_placement> cube_placements(State s,unsigned cubes){
	//state is in here so that could decide to not end up with extras in the fault beyond the 9.
	vector<Cube_placement> r;
	for(auto a:range(cubes+1)){
		auto left=cubes-a;
		for(auto b:range(left+1)){
			auto left2=left-b;
			for(auto c:range(left2+1)){
				//assume that going to place all of the cubes since no reason not to (not going to run out)
				//TODO: Make it so that not overloading the vault (will have to pass in alliance in to do that though)
				r|=Cube_placement{a,b,c,left2-c};
			}
		}
	}
	return r;
}


