#include<iostream>
#include<array>
#include "util.h"

using namespace std;

/*This is to work out what the strategic options are for an alliance, in isolation
this is just in terms of robot actions, and does not include use of powerups.
*/

//start generic routines

#define MAP(F,IN) mapf([&](auto elem){ return F(elem); },IN)

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

template<typename T>
T sum(vector<T> const& a){
	nyi
}

template<typename T>
T sum(tuple<T,T,T> in){
	return get<0>(in)+get<1>(in)+get<2>(in);
}

template<typename T,size_t LEN>
T sum(array<T,LEN>)nyi

template<typename T>
set<T> to_set(vector<T> v){
	return set<T>(begin(v),end(v));
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

template<typename T>
vector<T> frontier(vector<T> a){
	//remove from the set if there is another option that is always better.
	return filter(
		[&](auto elem){
			return any(mapf(
				[&](auto x){ return always_better(x,elem); },
				a
			));
		},
		a
	);
}

template<typename T>
vector<T> to_vector(set<T> a){
	return vector<T>(begin(a),end(a));
}

template<typename T>
auto frontier(set<T> a){
	return frontier(to_vector(a));
}

vector<Action_set> actions_available(Alliance_capabilities const& alliance){
	auto m=MAP(actions_available,alliance);
	auto x=to_set(MAP(sum,cross3(m)));
	print_lines(x);
	auto f=frontier(x);
	cout<<"after:\n";
	//PRINT(f);
	print_lines(f);
	nyi
}

int main(){
	auto a=rand((Alliance_capabilities*)nullptr);
	PRINT(a);
	PRINT(actions_available(a));
}

