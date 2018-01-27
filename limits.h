#ifndef LIMITS_H
#define LIMITS_H

#include "state.h"

using Points=int;
//using Result=std::pair<Points,State>;//points is in terms of net benefit to red.

#define RESULT(X)\
	X(Points,points)\
	X(State,state)

struct Result{
	#define X(A,B) A B;
	RESULT(X)
	#undef X
};

std::ostream& operator<<(std::ostream&,Result);
Result operator-(Result,Result);
bool operator<(Result,Result);
bool operator!=(Result,Result);

Points best_case(State);
Points worst_case(State);

#define POWERUP_TYPES(X) \
	X(BOOST1) \
	X(BOOST2) \
	X(BOOST3) \
	X(FORCE1) \
	X(FORCE2) \
	X(FORCE3) \
	X(LEVITATE)

enum Powerup_type{
	#define X(NAME) NAME,
	POWERUP_TYPES(X)
	#undef X
};

std::ostream& operator<<(std::ostream&,Powerup_type);

std::vector<Powerup_type> powerup_types();

#endif
