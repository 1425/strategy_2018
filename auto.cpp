#include<iostream>
#include<cassert>
#include<vector>
#include<map>
#include "util.h"

//start generic code

template<typename A,typename B>
A sorted(A a,B b){
	std::sort(std::begin(a),std::end(a),b);
	return a;
}

std::vector<double> as_doubles(std::vector<bool> in){
	return mapf(
		[](auto a)->double{ if(a) return 1; return 0; },
		in
	);
}

double mean(std::vector<bool> v){
	return mean(as_doubles(v));
}

using namespace std;

#define SHOW(A,B) o<<""#B<<":"<<a.B<<" ";
#define LIST(A) A,
#define CMP(A,B) if(a.B<b.B) return 1; if(b.B<a.B) return 0;

//start program-specific code

#define OUTCOME(X)\
	X(double,switch_cubes)\
	X(double,scale_cubes)\
	X(double,points)\
	X(double,has_cube)

struct Outcome{
	OUTCOME(INST)
};

ostream& operator<<(ostream& o,Outcome a){
	o<<"Outcome( ";
	OUTCOME(SHOW)
	return o<<")";
}

Outcome operator+(Outcome a,Outcome b){
	#define X(A,B) a.B+=b.B;
	OUTCOME(X)
	#undef X
	return a;
}

Outcome operator-(Outcome a,Outcome b){
	#define X(A,B) a.B-=b.B;
	OUTCOME(X)
	#undef X
	return a;
}

bool operator<(Outcome a,Outcome b){
	OUTCOME(CMP)
	return 0;
}

#define ACTION(X)\
	X(CN) X(CF) X(WN) X(WF) X(D)

enum class Action{
	ACTION(LIST)
};

ostream& operator<<(ostream& o,Action a){
	#define X(A) if(a==Action::A) return o<<""#A;
	ACTION(X)
	#undef X
	assert(0);
}

vector<Action> actions(){
	return {
		#define X(A) Action::A,
		ACTION(X)
		#undef X
	};
}

#define PLATE_ASSIGNMENT(X)\
	X(bool,switch_near)\
	X(bool,scale_near)

struct Plate_assignment{
	PLATE_ASSIGNMENT(INST)
};

ostream& operator<<(ostream& o,Plate_assignment a){
	o<<"Plate_assignment( ";
	PLATE_ASSIGNMENT(SHOW)
	return o<<")";
}

bool operator<(Plate_assignment a,Plate_assignment b){
	PLATE_ASSIGNMENT(CMP)
	return 0;
}

vector<Plate_assignment> plate_assignments(){
	return {
		Plate_assignment{0,0},
		Plate_assignment{0,1},
		Plate_assignment{1,0},
		Plate_assignment{1,1}
	};
}

using Strategy=map<Plate_assignment,Action>;

vector<Strategy> strategies(){
	vector<Strategy> r;
	for(auto a:actions()){
		for(auto b:actions()){
			for(auto c:actions()){
				for(auto d:actions()){
					r|=Strategy{
						{Plate_assignment{0,0},a},
						{Plate_assignment{0,1},b},
						{Plate_assignment{1,0},c},
						{Plate_assignment{1,1},d}
					};
				}
			}
		}
	}
	return r;
}

struct Priority_impl{
	virtual ~Priority_impl(){}
	virtual bool operator()(Outcome,Outcome)const=0;
	virtual void show(std::ostream&)const=0;
	virtual Priority_impl *clone()const=0;
};

struct Switch:Priority_impl{
	bool operator()(Outcome a,Outcome b)const{
		CMP1(switch_cubes)
		CMP1(points)
		CMP1(scale_cubes)
		CMP1(has_cube)
		return 0;
	}

	void show(std::ostream& o)const{
		o<<"Switch";
	}

	Switch *clone()const{
		return new Switch();
	}
};

struct Scale:Priority_impl{
	bool operator()(Outcome a,Outcome b)const{
		CMP1(scale_cubes)
		CMP1(points)
		CMP1(switch_cubes)
		CMP1(has_cube)
		return 0;
	}

	void show(std::ostream& o)const{ o<<"Scale"; }
	Scale *clone()const{ return new Scale(); }
};

struct Points:Priority_impl{
	bool operator()(Outcome a,Outcome b)const{
		CMP1(points)
		CMP1(scale_cubes)
		CMP1(switch_cubes)
		CMP1(has_cube)
		return 0;
	}

	void show(std::ostream& o)const{ o<<"Points"; }
	Points *clone()const{ return new Points(); }
};

struct Expected_points:Priority_impl{
	bool operator()(Outcome a,Outcome b)const{
		auto value=[](Outcome a)->double{
			return a.points+a.scale_cubes*5+a.switch_cubes*5+a.has_cube;
		};
		return value(a)<value(b);
	}

	void show(std::ostream& o)const{ o<<"Expected points"; }
	Expected_points *clone()const{ return new Expected_points(); }
};

class Priority{
	Priority_impl *inner;

	public:
	Priority(Priority_impl const& a):inner(a.clone()){}

	Priority(Priority const& a):inner(a.inner->clone()){}

	Priority& operator=(Priority const&);

	~Priority(){ delete inner; }

	bool operator()(Outcome a,Outcome b)const{
		return inner->operator()(a,b);
	}

	void show(std::ostream& o)const{
		inner->show(o);
	}
};

std::ostream& operator<<(std::ostream& o,Priority const& a){
	a.show(o);
	return o;
}

vector<Priority> priorities(){
	return {Switch(),Scale(),Points(),Expected_points()};
}

#define SCENARIO(X)\
	X(Priority,priority)\
	X(bool,crossover_ok)
//todo: put in stuff about what the rest of your team is already doing.
struct Scenario{
	SCENARIO(INST)
};

ostream& operator<<(ostream& o,Scenario a){
	o<<"Scenario( ";
	SCENARIO(SHOW)
	return o<<")";
}

vector<Scenario> scenarios(){
	vector<Scenario> r;
	for(auto priority:priorities()){
		for(auto b:bools()){
			r|=Scenario{priority,b};
		}
	}
	return r;
}

Outcome mean(vector<Outcome> in){
	return Outcome{
		#define X(A) mean(mapf([](auto a){ return a.A; },in))
		X(switch_cubes),
		X(scale_cubes),
		X(points),
		X(has_cube)
		#undef X
	};
}

const Outcome DRIVE{0,0,5,0};

struct Probability{
	double px;//0-1 only

	operator double()const{
		return px;
	}
};

struct Auto_task_time{
	double seconds; //0-15 only
};

double operator-(double a,Auto_task_time b){
	return a-b.seconds;
}

struct Action_capability{
	Probability px;
	Auto_task_time time_required;
};

using Robot_capabilities=map<Action,Action_capability>;

Robot_capabilities perfect_robot(){
	Robot_capabilities r;
	for(auto action:actions()){
		r[action]=Action_capability{1,0};
	}
	return r;
}

Robot_capabilities example_robot(){
	Robot_capabilities r;
	r[Action::CN]=Action_capability{.8,8.5};
	r[Action::CF]=Action_capability{.5,14};
	r[Action::WN]=Action_capability{.8,5};
	r[Action::WF]=Action_capability{.8,14};
	r[Action::D]=Action_capability{1,0};
	return r;
}

Outcome outcome(Scenario scenario,Plate_assignment plates,Robot_capabilities capabilities,Action action){
	auto f=capabilities.find(action);
	assert(f!=capabilities.end());
	auto px=f->second.px;
	auto time=f->second.time_required;
	switch(action){
		case Action::CN:{
			auto base=Outcome{0,px,px*2*(15-time),0};
			if(plates.scale_near){
				return base+DRIVE;
			}
			return DRIVE-base;
		}
		case Action::CF:{
			Outcome base{0,px,px*2*(15-time),0};
			if(!plates.scale_near && scenario.crossover_ok){
				return DRIVE+base;
			}
			//assume that being in the wrong zone is as bad as placing on the wrong plate.
			return DRIVE-base;
		}
		case Action::WN:{
			if(plates.switch_near){
				return Outcome{px,0,px*2*(15-time),0}+DRIVE;
			}
			return Outcome{-px,0,0,0}+DRIVE;
		}
		case Action::WF:{
			if(!plates.switch_near && scenario.crossover_ok){
				return Outcome{px,0,px*2*(15-time),0}+DRIVE;
			}
			return Outcome{-px,0,0,0}+DRIVE;
		}
		case Action::D:
			return DRIVE+Outcome{0,0,0,1};
		default:
			assert(0);
	}
}

Outcome outcome(Scenario scenario,Robot_capabilities robot,Strategy strategy){
	return mean(mapf(
		[=](pair<Plate_assignment,Action> p){
			return outcome(scenario,p.first,robot,p.second);
		},
		strategy
	));
}

void print_compact(int)nyi

void print_compact(Outcome a){
	cout<<"("<<a.switch_cubes<<","<<a.scale_cubes<<","<<a.points;
	cout<<","<<a.has_cube<<")";
}

void print_compact(Plate_assignment a){
	//cout<<"("<<a.switch_near<<","<<a.scale_near<<")";
	cout<<a.switch_near<<a.scale_near;
}

void print_compact(Action a){
	cout<<a;
}

template<typename A,typename B>
void print_compact(map<A,B> a){
	cout<<"{";
	for(auto p:a) print_compact(p);
	cout<<"}";
}

template<typename A,typename B>
void print_compact(pair<A,B> p){
	cout<<"(";
	print_compact(p.first);
	cout<<",";
	print_compact(p.second);
	cout<<")";
}

int main(){
	//print_lines(plate_assignments());
	//print_lines(scenarios());
	//print_lines(strategies());

	//auto robot=perfect_robot();
	auto robot=example_robot();
	for(auto scenario:scenarios()){
		PRINT(scenario);
		auto m=mapf(
			[=](auto strat){ return make_pair(outcome(scenario,robot,strat),strat); },
			strategies()
		);
		auto p=take(5,reversed(
			sorted(
				m,
				[=](auto a,auto b){
					return scenario.priority(a.first,b.first);
				}
			)
		));
		//print_lines(take(5,reversed(sorted(m))));
		for(auto a:p){
			print_compact(a);
			cout<<"\n";
		}
	}
}

