#ifndef DECODE_H
#define DECODE_H

#include "util.h"
#include "util2.h"
#include "sub.h"

double rand(const double*);
double dead(const double*);

template<typename T>
std::vector<T> dead(const std::vector<T>*){
	return mapf([](auto){ return dead((T*)0); },range(5));
}

template<typename A,typename B>
std::pair<A,B> dead(const std::pair<A,B>*){
	return make_pair(dead((A*)0),dead((B*)0));
}

template<typename A,typename B>
std::pair<A,B>& operator/=(std::pair<A,B> &p,long unsigned int i){
	p.first/=i;
	p.second/=i;
	return p;
}

using Team=unsigned int;

#define SCOUTING_ROW(X)\
	X(Team,team)\
	X(std::string,match)\
	X(unsigned int,auto_switch_cubes)\
	X(unsigned int,auto_scale_cubes)\
	X(unsigned int,teleop_switch_cubes)\
	X(unsigned int,teleop_scale_cubes)\
	X(unsigned int,teleop_vault_cubes)\
	X(unsigned int,helped_climb)\
	X(bool,piggyback)

//TODO: put in the full set of data that we're going to care about.

struct Scouting_row{
	SCOUTING_ROW(INST)
};

std::ostream& operator<<(std::ostream& o,Scouting_row a);

int decode(std::string s,const int*);

std::string decode(std::string s,const std::string*);

bool decode(std::string,const bool*);
unsigned decode(std::string,const unsigned*);

std::vector<Scouting_row> read_csv(std::string filename);

#define CLIMB_CAPABILITIES(X)\
	X(drives)\
	X(itself)\
	X(bar1)\
	X(bar2)\
	X(lift1)\
	X(lift2)\
	X(all1)\
	X(all2)

struct Climb_capabilities{
	#define X(NAME) Px NAME;
	CLIMB_CAPABILITIES(X)
	#undef X
};

std::ostream& operator<<(std::ostream&,Climb_capabilities);
bool operator<(Climb_capabilities const&,Climb_capabilities const&);
bool operator==(Climb_capabilities const&,Climb_capabilities const&);
bool operator!=(Climb_capabilities const&,Climb_capabilities const&);

Climb_capabilities& operator+=(Climb_capabilities& a,Climb_capabilities b);
Climb_capabilities operator/(Climb_capabilities a,long unsigned int i);
Climb_capabilities rand(const Climb_capabilities*);
Climb_capabilities dead(const Climb_capabilities*);

//Climb_capabilities mean(Nonempty_vector<Climb_capabilities>)nyi

using Alliance_climb=std::array<Climb_capabilities,3>;

double single_climb(std::array<Climb_capabilities,3> a);

double no_climb(Alliance_climb);
double bar1(Alliance_climb);
double bar2(Alliance_climb);
double lift1(Alliance_climb);
double lift2(Alliance_climb);
double all1(Alliance_climb);
double all2(Alliance_climb);

double expected_value(std::array<Climb_capabilities,3>);

class Cubes_scored{
	double value;

	public:
	static const unsigned MAX=50;//approx

	Cubes_scored():value(0){}

	Cubes_scored(double in):value(in){
		/*if(value>=MAX){
			PRINT(value);
		}
		assert(value<MAX);*/
	}

	operator double()const{
		return value;
	}

	double get()const{
		return value;
	}
};

Cubes_scored operator+(Cubes_scored a,Cubes_scored b);
Cubes_scored& operator+=(Cubes_scored &a,Cubes_scored b);
Cubes_scored& operator/=(Cubes_scored& a,unsigned int b);

Cubes_scored rand(const Cubes_scored*);
Cubes_scored dead(const Cubes_scored*);

double mean(Nonempty_vector<Cubes_scored>);

//for teleop period
using Scale_cubes=Cubes_scored;
using Switch_cubes=Cubes_scored;
using Vault_cubes=Cubes_scored;
//using Cube_match=tuple<Scale_cubes,Switch_cubes,Vault_cubes>;

#define CUBE_MATCH(X)\
	X(Cubes_scored,scale_cubes)\
	X(Cubes_scored,switch_cubes)\
	X(Cubes_scored,vault_cubes)

struct Cube_match{
	CUBE_MATCH(INST)
};

std::ostream& operator<<(std::ostream&,Cube_match);
Cube_match operator/(Cube_match a,long unsigned int i);
Cube_match& operator+=(Cube_match& a,Cube_match b);
bool operator<(Cube_match,Cube_match);
bool operator==(Cube_match,Cube_match);
Cube_match rand(const Cube_match*);
Cube_match dead(const Cube_match*);

/*Cube_match mean(Nonempty_vector<Cube_match> a){
	
}*/

using Cube_capabilities=std::vector<Cube_match>;

Cube_capabilities mean(Nonempty_vector<Cube_capabilities>);

std::vector<Cubes_scored> scale_cubes(std::vector<Cube_match> const&);
std::vector<Cubes_scored> switch_cubes(std::vector<Cube_match> const&);
std::vector<Cubes_scored> vault_cubes(std::vector<Cube_match> const&);

double average_total_cubes(Cube_capabilities);

double scale_expectation(Skellam_cdf const&,double cubes1,double cubes2);

int vault_value(Cubes_scored);

double vault(Cube_capabilities);

double expected_outcome(Skellam_cdf const&,Cube_capabilities,Cube_capabilities);

#define AUTO_CAPABILITIES(X)\
	X(double,scale_cubes)\
	X(double,switch_cubes)\

struct Auto_capabilities{
	AUTO_CAPABILITIES(INST)
};

std::ostream& operator<<(std::ostream&,Auto_capabilities const&);
bool operator<(Auto_capabilities const&,Auto_capabilities const&);
bool operator==(Auto_capabilities const&,Auto_capabilities const&);
bool operator!=(Auto_capabilities const&,Auto_capabilities const&);

Auto_capabilities& operator+=(Auto_capabilities&,Auto_capabilities);

Auto_capabilities rand(const Auto_capabilities*);
Auto_capabilities dead(const Auto_capabilities*);

Auto_capabilities operator/(Auto_capabilities a,long unsigned int i);

double expected_value(std::array<Auto_capabilities,3>);

#define ROBOT_CAPABILITIES(X)\
	X(Cube_capabilities,cubes)\
	X(Climb_capabilities,climb)\
	X(Auto_capabilities,auton)\

struct Robot_capabilities{
	ROBOT_CAPABILITIES(INST)
};

std::ostream& operator<<(std::ostream&,Robot_capabilities const&);
bool operator<(Robot_capabilities const&,Robot_capabilities const&);
bool operator>(Robot_capabilities const&,Robot_capabilities const&);
bool operator==(Robot_capabilities const&,Robot_capabilities const&);
bool operator!=(Robot_capabilities const&,Robot_capabilities const&);

Robot_capabilities rand(const Robot_capabilities*);
Robot_capabilities dead(const Robot_capabilities*);

Robot_capabilities mean(Nonempty_vector<Robot_capabilities> const&);

double average_total_cubes(Robot_capabilities);

std::map<Team,Robot_capabilities> interpret(std::vector<Scouting_row>);

using Alliance_capabilities=std::array<Robot_capabilities,3>;

Alliance_capabilities dead(const Alliance_capabilities*);

std::array<Climb_capabilities,3> climbs(Alliance_capabilities);

std::array<Cube_capabilities,3> cubes(Alliance_capabilities);

Cube_capabilities distill_cubes(Alliance_capabilities);

std::array<Auto_capabilities,3> auton(Alliance_capabilities const&);

double expected_outcome(Skellam_cdf const& skellam_cdf,Alliance_capabilities a,Alliance_capabilities b);

Alliance_capabilities random_alliance(std::vector<Robot_capabilities> in);

std::pair<std::array<Robot_capabilities,3>,std::vector<Robot_capabilities>> random_alliance2(std::vector<Robot_capabilities>);

std::string capabilities_table(std::vector<std::pair<Team,Robot_capabilities>> const&);
std::string capabilities_table(std::map<Team,Robot_capabilities> const&);

#endif