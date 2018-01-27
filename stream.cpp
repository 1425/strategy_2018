#include<iostream>
#include<array>
#include "util.h"

using namespace std;

/*This is to work out what the strategic options are for an alliance, in isolation
this is just in terms of robot actions, and does not include use of powerups.
*/

//start generic routines

#define MAP(F,IN) mapf([&](auto elem){ return F(elem); },IN)

template<typename T>
vector<T> to_vector(set<T> a){
	return vector<T>(begin(a),end(a));
}

template<typename T,size_t LEN>
std::ostream& operator<<(std::ostream& o,std::array<T,LEN> const& a){
	o<<"[ ";
	for(auto const& elem:a){
		o<<elem<<" ";
	}
	return o<<"]";
}

template<typename T>
bool any(vector<T> a){
	for(auto elem:a){
		if(elem){
			return 1;
		}
	}
	return 0;
}

template<typename A,typename B,typename C>
vector<tuple<A,B,C>> cross3(vector<A> as,vector<B> bs,vector<C> cs){
	vector<tuple<A,B,C>> r;
	for(auto a:as){
		for(auto b:bs){
			for(auto c:cs){
				r|=make_tuple(a,b,c);
			}
		}
	}
	return r;
}

template<typename T>
vector<tuple<T,T,T>> cross3(array<vector<T>,3> a){
	return cross3(a[0],a[1],a[2]);
}

template<typename A,typename B,typename C>
vector<tuple<A,B,C>> cross(vector<A> as,vector<B> bs,vector<C> cs){
	vector<tuple<A,B,C>> r;
	for(auto a:as) for(auto b:bs) for(auto c:cs){
		r|=make_tuple(a,b,c);
	}
	return r;
}

template<typename T>
T sum(vector<T> const& a){
	nyi
}

template<typename T>
T sum(tuple<T,T,T> in){
	return get<0>(in)+get<1>(in)+get<2>(in);
}

template<typename T,size_t LEN>
T sum(array<T,LEN> const& a){
	T r{};
	for(auto elem:a){
		r+=elem;
	}
	return r;
}

template<typename T>
set<T> to_set(vector<T> v){
	return set<T>(begin(v),end(v));
}

template<typename T,size_t SIZE>
size_t count(array<T,SIZE> a){
	size_t r=0;
	for(auto elem:a){
		if(elem){
			r++;
		}
	}
	return r;
}

template<typename T>
array<T,3> as_array(tuple<T,T,T> a){
	return {get<0>(a),get<1>(a),get<2>(a)};
}

template<typename T>
vector<array<T,3>> permutations(array<T,3> a){
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
vector<T> frontier(vector<T> a){
	//remove from the set if there is another option that is always better.
	return filter(
		[&](auto elem){
			return !any(mapf(
				[&](auto x){ return always_better(elem,x); },
				a
			));
		},
		a
	);
}

template<typename T>
auto frontier(set<T> a){
	return frontier(to_vector(a));
}

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

bool operator==(Action_set a,Action_set b){
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

#define ALLIANCE_ENDGAME(X)\
	X(unsigned,climbed)\
	X(array<SINGLE_ARG(bool,3)>,occupied)

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

vector<Alliance_endgame> climb_options(Alliance_climb_type a){
	vector<Alliance_endgame> r;

	r|={0,{0,0,0}};

	if(can_climb_bar(a[0])){
		r|={1,0,0};
	}
	if(can_climb_bar(a[1])){
		r|={0,1,0};
	}
	if(can_climb_bar(a[2])){
		r|={0,0,1};
	}

	if(climbs({a[0],a[1],Climb_type::DEAD})==2){
		r|={1,1,0};
	}
	if(climbs({a[0],a[2],Climb_type::DEAD})==2){
		r|={1,0,1};
	}
	if(climbs({a[1],a[2],Climb_type::DEAD})==2){
		r|={0,1,1};
	}

	if(climbs(a)==3){
		r|={1,1,1};
	}

	return r;
}

int main(){
	/*PRINT(always_better(Action_set{7,8},Action_set{7,7}));
	auto a=rand((Alliance_capabilities*)nullptr);
	//a=rand((Alliance_capabilities*)nullptr);
	PRINT(a);
	print_lines(actions_available(a));*/

	//PRINT(climb_types());
	//PRINT(alliance_climb_types());
	//print_lines(mapf([](auto a){ return make_pair(a,climbs(a)); },alliance_climb_types()));
	for(auto a:alliance_climb_types()){
		cout<<a<<"\n";
		for(auto b:climb_options(a)){
			cout<<"\t"<<b<<"\n";
		}
	}
}

