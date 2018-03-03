#include<iostream>
#include<array>
#include<sstream>
#include "util.h"

/*This is to work out what the strategic options are for an alliance, in isolation
this is just in terms of robot actions, and does not include use of powerups.
*/

//start generic routines

std::string title(std::string s){ return tag("title",s); }
std::string head(std::string s){ return tag("head",s); }
std::string h1(std::string s){ return tag("h1",s); }

template<typename T,size_t LEN>
std::ostream& operator<<(std::ostream& o,std::array<T,LEN> const& a){
	o<<"[ ";
	for(auto const& elem:a){
		o<<elem<<" ";
	}
	return o<<"]";
}

template<typename T>
bool any(std::vector<T> a){
	for(auto elem:a){
		if(elem){
			return 1;
		}
	}
	return 0;
}

template<typename A,typename B,typename C>
std::vector<std::tuple<A,B,C>> cross3(std::vector<A> as,std::vector<B> bs,std::vector<C> cs){
	std::vector<std::tuple<A,B,C>> r;
	for(auto a:as){
		for(auto b:bs){
			for(auto c:cs){
				r|=std::make_tuple(a,b,c);
			}
		}
	}
	return r;
}

template<typename T>
std::vector<std::tuple<T,T,T>> cross3(std::array<std::vector<T>,3> a){
	return cross3(a[0],a[1],a[2]);
}

template<typename A,typename B,typename C>
std::vector<std::tuple<A,B,C>> cross(std::vector<A> as,std::vector<B> bs,std::vector<C> cs){
	std::vector<std::tuple<A,B,C>> r;
	for(auto a:as) for(auto b:bs) for(auto c:cs){
		r|=std::make_tuple(a,b,c);
	}
	return r;
}

/*template<typename T>
T sum(vector<T> const& a){
	nyi
}*/

template<typename T>
T sum(std::tuple<T,T,T> in){
	return std::get<0>(in)+std::get<1>(in)+std::get<2>(in);
}

template<typename T,size_t SIZE>
size_t count(std::array<T,SIZE> a){
	size_t r=0;
	for(auto elem:a){
		if(elem){
			r++;
		}
	}
	return r;
}

template<typename T>
std::array<T,3> as_array(std::tuple<T,T,T> a){
	return {std::get<0>(a),std::get<1>(a),std::get<2>(a)};
}

template<typename T>
std::vector<std::array<T,3>> permutations(std::array<T,3> a){
	return {
		{a[0],a[1],a[2]},
		{a[0],a[2],a[1]},
		{a[1],a[0],a[2]},
		{a[1],a[2],a[0]},
		{a[2],a[0],a[1]},
		{a[2],a[1],a[0]}
	};
}

template<typename T>
std::vector<T> frontier(std::vector<T> a){
	//remove from the set if there is another option that is always better.
	return filter(
		[a](auto elem){
			return !any(mapf(
				[elem](auto x){ return always_better(x,elem); },
				a
			));
		},
		a
	);
}

template<typename T>
auto frontier(std::set<T> a){
	return frontier(to_vector(a));
}

using namespace std;

//start program-specific stuff

#define ROBOT_CAPABILITIES(X)\
	X(float,scale_time)\
	X(float,low_time)\

//TODO: Put in climbing times?

struct Robot_capabilities{
	#define X(A,B) A B;
	ROBOT_CAPABILITIES(X)
	#undef X
};

ostream& operator<<(ostream& o,Robot_capabilities a){
	o<<"(";
	#define X(A,B) o<<""#B<<":"<<a.B<<" ";
	ROBOT_CAPABILITIES(X)
	#undef X
	return o<<")";
}

Robot_capabilities rand(const Robot_capabilities*){
	return {
		float(rand()%30)+2,
		float(rand()%30)+2
	};
}

using Alliance_capabilities=array<Robot_capabilities,3>;

Alliance_capabilities rand(const Alliance_capabilities*){
	return {
		rand((Robot_capabilities*)nullptr),
		rand((Robot_capabilities*)nullptr),
		rand((Robot_capabilities*)nullptr)
	};
}

#define ACTION_SET(X)\
	X(unsigned,scale_cubes)\
	X(unsigned,low_cubes)

struct Action_set{
	#define X(A,B) A B;
	ACTION_SET(X)
	#undef X
};

ostream& operator<<(ostream& o,Action_set a){
	o<<"Action_set( ";
	#define X(A,B) o<<""#B<<":"<<a.B<<" ";
	ACTION_SET(X)
	#undef X
	return o<<")";
}

bool operator<(Action_set a,Action_set b){
	#define X(A,B) if(a.B<b.B) return 1; if(b.B<a.B) return 0;
	ACTION_SET(X)
	#undef X
	return 0;
}

bool operator==(Action_set /*a*/,Action_set /*b*/){
	nyi
}

Action_set operator+(Action_set a,Action_set b){
	return Action_set{
		#define X(A,B) a.B+b.B,
		ACTION_SET(X)
		#undef X
	};
}

static const float TELEOP_TIME=120;//assume using all of it for cube placement

unsigned max_scale_cubes(Robot_capabilities a){
	return TELEOP_TIME/a.scale_time;
}

vector<Action_set> actions_available(Robot_capabilities const& robot){
	vector<Action_set> r;
	for(auto scale_cubes:range(1+max_scale_cubes(robot))){
		const float placing_scale_cubes=robot.scale_time*scale_cubes;
		const float remaining=TELEOP_TIME-placing_scale_cubes;
		const unsigned low_cubes=unsigned(remaining/robot.low_time);
		r|=Action_set{scale_cubes,low_cubes};
	}
	return r;
}

bool always_better(Action_set a,Action_set b){
	return (a.scale_cubes>b.scale_cubes && a.low_cubes>=b.low_cubes) ||
		(a.scale_cubes>=b.scale_cubes && a.low_cubes>b.low_cubes);
}

vector<Action_set> actions_available(Alliance_capabilities const& alliance){
	auto m=MAP(actions_available,alliance);
	auto x=to_set(MAP(sum,cross3(m)));
	return frontier(x);
}

//start what is essentially a duplicate of part of the python climbing info

#define CLIMB_TYPE(X)\
	X(DEAD)\
	X(DRIVES)\
	X(ITSELF)\
	X(BAR1)\
	X(BAR2)\
	X(LIFT1)\
	X(LIFT2)\
	X(ALL1)\
	X(ALL2)

enum class Climb_type{
	#define X(A) A,
	CLIMB_TYPE(X)
	#undef X
};

ostream& operator<<(ostream& o,Climb_type a){
	#define X(A) if(a==Climb_type::A) return o<<""#A;
	CLIMB_TYPE(X)
	#undef X
	assert(0);
}

vector<Climb_type> climb_types(){
	return {
		#define X(A) Climb_type::A,
		CLIMB_TYPE(X)
		#undef X
	};
}

bool can_climb_bar(Climb_type a){
	switch(a){
		case Climb_type::DEAD:
		case Climb_type::DRIVES:
			return 0;
		case Climb_type::ITSELF:
		case Climb_type::BAR1:
		case Climb_type::BAR2:
			return 1;
		case Climb_type::LIFT1:
		case Climb_type::LIFT2:
			return 0;
		case Climb_type::ALL1:
		case Climb_type::ALL2:
			return 1;
		default:
			assert(0);
	}
}

bool can_drive(Climb_type a){
	return a!=Climb_type::DEAD;
}

using Alliance_climb_type=array<Climb_type,3>;

vector<Alliance_climb_type> alliance_climb_types(){
	return MAP(as_array,cross(climb_types(),climb_types(),climb_types()));
}

unsigned climbs_inner(Alliance_climb_type a){
	//returns 0-3
	array<Climb_type,2> rest{a[1],a[2]};
	switch(a[0]){
		case Climb_type::DEAD:
			return 0;
		case Climb_type::DRIVES:
			return 0;
		case Climb_type::ITSELF:
			return 1;
		case Climb_type::BAR1:
			return 1+max(can_climb_bar(a[1]),can_climb_bar(a[2]));
		case Climb_type::BAR2:
			return 1+can_climb_bar(a[1])+can_climb_bar(a[2]);
		case Climb_type::LIFT1:
			return max(can_drive(a[1]),can_drive(a[2]));
		case Climb_type::LIFT2:
			return count(mapf(can_drive,rest));
		case Climb_type::ALL1:
			return 1+max(can_drive(a[1]),can_drive(a[2]));
		case Climb_type::ALL2:
			return 1+count(mapf(can_drive,rest));
		default:
			assert(0);
	}
}

unsigned climbs(Alliance_climb_type a){
	//returns 0-3
	//the maximum number of climbs for that type of alliance.
	return max(mapf(climbs_inner,permutations(a)));
}

#define ROBOT_ENDGAME(X)\
	X(FREE)\
	X(CLIMBED)\
	X(OCCUPIED)

enum class Robot_endgame{
	#define X(NAME) NAME,
	ROBOT_ENDGAME(X)
	#undef X
};

ostream& operator<<(ostream& o,Robot_endgame a){
	#define X(A) if(a==Robot_endgame::A) return o<<""#A;
	ROBOT_ENDGAME(X)
	#undef X
	assert(0);
}

#define SINGLE_ARG(A,B) A,B

using Occupied=array<bool,3>;

vector<Occupied> occupied_options(){
	vector<Occupied> r;
	for(auto a:bools()){
		for(auto b:bools()){
			for(auto c:bools()){
				r|={a,b,c};
			}
		}
	}
	return r;
}

#define ALLIANCE_ENDGAME(X)\
	X(unsigned,climbed)\
	X(Occupied,occupied)

struct Alliance_endgame{
	#define X(A,B) A B;
	ALLIANCE_ENDGAME(X)
	#undef X
};

ostream& operator<<(ostream& o,Alliance_endgame const& a){
	o<<"Alliance_endgame( ";
	#define X(A,B) o<<""#B<<":"<<a.B<<" ";
	ALLIANCE_ENDGAME(X)
	#undef X
	return o<<")";
}

bool subset(Occupied a,Occupied b){
	//is 'a' a subset of 'b'?
	for(auto p:zip(a,b)){
		if(p.first && !p.second){
			return 0;
		}
	}
	return 1;
}

bool strict_subset(Occupied a,Occupied b){
	//is 'a' a struct subset of 'b'?
	return subset(a,b) && !subset(b,a);
}

bool always_better(Alliance_endgame a,Alliance_endgame b){
	//if at least as many climbs and have robots that don't need.
	return (a.climbed>b.climbed && subset(a.occupied,b.occupied)) ||
		(a.climbed>=b.climbed && strict_subset(a.occupied,b.occupied));
}

vector<Alliance_endgame> climb_options(Alliance_climb_type alliance){
	return frontier(mapf(
		[=](auto to_climb){
			Alliance_climb_type effective={
				to_climb[0]?alliance[0]:Climb_type::DEAD,
				to_climb[1]?alliance[1]:Climb_type::DEAD,
				to_climb[2]?alliance[2]:Climb_type::DEAD
			};
			return Alliance_endgame{climbs(effective),to_climb};
		},
		occupied_options()
	));
}

void endgame_demo(){
	PRINT(climb_types());
	PRINT(alliance_climb_types());
	print_lines(mapf([](auto a){ return make_pair(a,climbs(a)); },alliance_climb_types()));
	for(auto a:alliance_climb_types()){
		cout<<a<<"\n";
		for(auto b:climb_options(a)){
			cout<<"\t"<<b<<"\n";
		}
	}
}

auto consolidated_climb_options(){
	return reversed(to_vector(to_set(MAP(
		[](auto a){
			return reversed(sorted(a));
		},
		alliance_climb_types()
	))));
}

string climb_table(Alliance_climb_type a){
	/*			climbed
				occupied*/
	/*return table(join(mapf(
		[](auto a)->string{
			return tr(
				join(mapf(
					[](auto a){ return td(as_string(a)); },
					a.occupied
				))+
				td(as_string(a.climbed))
			);
		},
		climb_options(a)
	)));*/
	return join(mapf(
		[](auto a)->string{
			return "("+
				join(mapf(
					[](auto a){ return as_string(a); },
					a.occupied
				))+
				"-"+as_string(a.climbed)
				+")";
		},
		climb_options(a)
	));
	return as_string(
		climb_options(a)
	);
}

void endgame_table(){
	stringstream ss;
	ss<<html(
		head(
			title("Endgame possibilities")
		)+
		body(
			h1("Endgame possibilities")+
			tag("table","border",
				tr(
					th("Robot 1 climb type")+
					th("Robot 2 climb type")+
					th("Robot 3 climb type")+
					th("Options")
				)+
				join(mapf(
					[](auto a){
						return tr(
							td(as_string(a[0]))+
							td(as_string(a[1]))+
							td(as_string(a[2]))+
							td(climb_table(a))
						);
					},
					consolidated_climb_options()
				))
			)
		)
	);
	write_file("endgame.html",ss.str());
}

void teleop_demo(){
	auto a=rand((Alliance_capabilities*)nullptr);
	//a=rand((Alliance_capabilities*)nullptr);
	PRINT(a);
	print_lines(actions_available(a));
}

//Auto_assignment
	//scale/switch
//Teleop_assignment
	//switch/scale/vault/defense
//Endgame_assignment:
	//per robot: climb/switch/scale/vault/defense

//Assignment: Auto_assignment x Teleop_assignment x Endgame_assignment

/*
TODO: Make endgame lookup brochure
9*9*9=
*/

int main(){
	//endgame_demo();
	endgame_table();
}

