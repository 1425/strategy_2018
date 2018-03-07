#include <fstream>
#include <map>
#include<unistd.h>
#include<sys/wait.h>
#include<iomanip>
#include<functional>
#include "util.h"
#include "sub.h"
#include "util2.h"

//start generic functions

using namespace std;

template<typename T>
T index_or_last(size_t index,vector<T> v){
	assert(v.size());
	if(v.size()>index) return v[index];
	return v[v.size()-1];
}

template<typename T,size_t N>
T max(std::array<T,N> a){
	assert(N);
	T r=a[0];
	for(auto elem:a){
		r=max(elem,r);
	}
	return r;
}

auto atoi(string const& s){ return atoi(s.c_str()); }

string small(string s){ return tag("small",s); }
string title(string s){ return tag("title",s); }
string head(string s){ return tag("head",s); }
string h1(string s){ return tag("h1",s); }


template<typename K,typename V>
vector<K> firsts(map<K,V> m){
	return mapf([](auto a){ return a.first; },m);
}

template<typename T>
vector<T> take_range(size_t start,size_t length,vector<T> in){
	vector<T> r;
	for(size_t at=start;at<start+length && at<in.size();at++){
		r|=in[at];
	}
	return r;
}

template<typename T>
vector<pair<T,T>> nonduplicate_pairs(vector<T> a){
	vector<pair<T,T>> r;
	for(auto i:range(a.size())){
		for(auto j:range(a.size())){
			if(i==j) continue;
			r|=make_pair(a[i],a[j]);
		}
	}
	return r;
}

template<typename T,size_t N>
size_t index_max(array<T,N> const& a){
	assert(N);
	size_t r=0;
	T value=a[0];
	for(auto p:enumerate(a)){
		if(p.second>value){
			r=p.first;
			value=p.second;
		}
	}
	return r;
}

template<typename Func,typename T,size_t N>
size_t index_max(Func f,array<T,N> const& in){
	return index_max(mapf(f,in));
}

template<typename T,size_t N>
std::array<std::pair<size_t,T>,N> enumerate(std::array<T,N> a){
	std::array<pair<size_t,T>,N> r;
	for(auto i:range(N)){
		r[i]=make_pair(i,a[i]);
	}
	return r;
}

template<typename T,size_t N>
std::array<T,N-1> without_index(size_t i,std::array<T,N> a){
	assert(i<N);
	std::array<T,N-1> r;

	size_t in_index=0;
	size_t out_index=0;
	while(in_index<N){
		if(in_index==i) in_index++;

		r[out_index]=a[in_index];

		in_index++;
		out_index++;
	}
	return r;
}

pair<double,double> limits(vector<double> a){
	return make_pair(min(a),max(a));
}

template<typename T>
auto size(vector<T> v){ return v.size(); }

template<typename T>
bool all_equal(vector<T> a){
	if(a.empty()) return 1;
	for(auto elem:a){
		if(elem!=a[0]){
			return 0;
		}
	}
	return 1;
}

template<typename T>
vector<vector<T>> transpose(vector<vector<T>> in){
	if(in.empty()) return {};

	auto s=MAP(size,in);
	assert(all_equal(s));
	auto len=s[0];

	vector<vector<T>> r;
	for(auto i:range(len)){
		vector<T> row;
		for(auto a:in){
			row|=a[i];
		}
		r.push_back(row);
	}
	return r;
}

string as_2decimals(double a){
	stringstream ss;
	ss<<fixed<<setprecision(2)<<a;
	return ss.str();
}

/*template<typename A,typename B,typename C>
tuple<A,B,C> sum(vector<tuple<A,B,C>>)nyi*/

//start program-speicifc functions.

/*
How to figure out which teams to compare against:
the frontier along several dimensions:
1) scale cubes (incl auto)
2) switch cubes (incl auto)
3) climb points (since not really interactive)
4) auto pts (include w/ climb points)

need to know the top N+1 combinations, where N=# of robots available as partners
this would mean that would have at least 1 where the candidate partner is not in it.

how to keep track of the top N items:
(and any linear combination of them is allowed also)
to add to the set:
*/

/*struct Top_items{
	using T=int;
	vector<T> current;

	void add(T t){
		
	}
};

Team_number->[alliances]

*/

using Team=unsigned int;

#define SCOUTING_ROW(X)\
	X(Team,team)\
	X(string,match)\
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

ostream& operator<<(ostream& o,Scouting_row a){
	o<<"Scouting_row(";
	SCOUTING_ROW(SHOW)
	return o<<")";
}

int decode(string s,const int*){
	//could actually check for errors.
	return atoi(s.c_str());
}

string decode(string s,const string*){
	return s;
}

bool decode(string s,const bool*){
	if(s=="0") return 0;
	if(s=="1") return 1;
	stringstream ss;
	ss<<"Invalid bool: \""<<s<<"\"";
	throw ss.str();
}

unsigned decode(string s,const unsigned*){
	auto i=atoi(s.c_str());
	assert(i>=0);
	return i;
}

vector<Scouting_row> read_csv(string filename){
	ifstream f(filename);
	vector<Scouting_row> r;
	while(f.good()){
		string s;
		getline(f,s);
		if(s.empty()) continue;

		auto sp=split(s,',');
		Scouting_row row;
		#define X(A,B) assert(sp.size()); {\
			auto v=car(sp);\
			sp=cdr(sp);\
			row.B=decode(v,(A*)nullptr);\
		}
		SCOUTING_ROW(X)
		#undef X
		assert(sp.empty());
		r|=row;
	}
	return r;
}

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

ostream& operator<<(ostream& o,Climb_capabilities a){
	o<<"Climb_capabilities(";
	#define X(NAME) o<<""#NAME<<":"<<a.NAME<<" ";
	CLIMB_CAPABILITIES(X)
	#undef X
	return o<<")";
}

bool operator<(Climb_capabilities const& a,Climb_capabilities const& b){
	#define X(A) if(a.A<b.A) return 1; if(a.A>b.A) return 0;
	CLIMB_CAPABILITIES(X)
	#undef X
	return 0;
}

bool operator==(Climb_capabilities const& a,Climb_capabilities const& b){
	#define X(A) if(a.A!=b.A) return 0;
	CLIMB_CAPABILITIES(X)
	#undef X
	return 1;
}

bool operator!=(Climb_capabilities const& a,Climb_capabilities const& b){
	return !(a==b);
}

Climb_capabilities& operator+=(Climb_capabilities& a,Climb_capabilities b){
	#define X(A) a.A+=b.A;
	CLIMB_CAPABILITIES(X)
	#undef X
	return a;
}

Climb_capabilities operator/(Climb_capabilities a,long unsigned int i){
	#define X(A) a.A/=i;
	CLIMB_CAPABILITIES(X)
	#undef X
	return a;
}

Climb_capabilities rand(const Climb_capabilities*){
	Climb_capabilities r;
	#define X(A) r.A=rand((Px*)0);
	CLIMB_CAPABILITIES(X)
	#undef X
	return r;
}

Climb_capabilities dead(const Climb_capabilities*){
	Climb_capabilities r;
	#define X(A) r.A=dead((Px*)0);
	CLIMB_CAPABILITIES(X)
	#undef X
	return r;
}

//Climb_capabilities mean(Nonempty_vector<Climb_capabilities>)nyi

using Alliance_climb=std::array<Climb_capabilities,3>;

double single_climb(std::array<Climb_capabilities,3> a){
	return 30*max(mapf([](auto x){ return x.itself; },a))+10;
}

double no_climb(Alliance_climb){
	return 15;
}

double bar1(Alliance_climb a){
	auto i=index_max([](auto x){ return x.bar1; },a);
	auto bar1_rx=a[i].bar1;
	auto x=without_index(i,mapf([](auto x){ return x.itself; },a));
	return bar1_rx*max(x)*60+5;
}

double bar2(Alliance_climb a){
	auto i=index_max([](auto x){ return x.bar2; },a);
	auto bar2_rx=a[i].bar2;
	auto x=without_index(i,mapf([](auto x){ return x.itself; },a));
	return bar2_rx*sum(x)*45;
}

double lift1(Alliance_climb a){
	auto i=index_max([](auto x){ return x.lift1; },a);
	auto rx=a[i].lift1;
	auto x=without_index(i,mapf([](auto x){ return x.drives; },a));
	return rx*max(x)*30+5;
}

double lift2(Alliance_climb a){
	auto i=index_max([](auto x){ return x.lift2; },a);
	auto rx=a[i].lift2;
	auto x=without_index(i,mapf([](auto x){ return x.drives; },a));
	return rx*sum(x)*30;
}

double all1(Alliance_climb a){
	auto i=index_max([](auto x){ return x.all1; },a);
	auto rx=a[i].all1;
	auto x=without_index(i,mapf([](auto x){ return x.drives; },a));
	return (rx+rx*max(x))*30+5;
}

double all2(Alliance_climb a){
	auto i=index_max([](auto x){ return x.all2; },a);
	auto rx=a[i].all2;
	auto x=without_index(i,mapf([](auto x){ return x.drives; },a));
	return rx*(1+sum(x))*30;
}

double expected_value(std::array<Climb_capabilities,3> a){
	//TODO: Once there is data in the Climb_capabilities struct, use it.

	//strategies:
	//1) just let the best climber go
	//2) bar1/2/etc.
	//3) lift1/2 etc.
	//4) best all1/all2
	//5) everybody just park.

	return max({
		no_climb(a),
		single_climb(a),
		bar1(a),
		bar2(a),
		lift1(a),
		lift2(a),
		all1(a),
		all2(a)
	});
}

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

Cubes_scored operator+(Cubes_scored a,Cubes_scored b){
	//saturating addition.
	//return min(double(Cubes_scored::MAX-1),a.get()+b.get());
	return a.get()+b.get();
}

Cubes_scored& operator+=(Cubes_scored &a,Cubes_scored b){
	a=(a+b);
	return a;
}

Cubes_scored& operator/=(Cubes_scored& a,unsigned int b){
	//note that this will be doing some rounding.
	a=Cubes_scored{a.get()/b};
	return a;
}

Cubes_scored rand(const Cubes_scored*){
	return rand()%Cubes_scored::MAX;
}

Cubes_scored dead(const Cubes_scored*){
	return 0;
}

double mean(Nonempty_vector<Cubes_scored> a){
	return mean(to_doubles(a));
}

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

ostream& operator<<(ostream&,Cube_match)nyi

Cube_match rand(const Cube_match*){
	Cube_match r;
	#define X(A,B) r.B=rand((A*)0);
	CUBE_MATCH(X)
	#undef X
	return r;
}

Cube_match dead(const Cube_match*){
	Cube_match r;
	#define X(A,B) r.B=dead((A*)0);
	CUBE_MATCH(X)
	#undef X
	return r;
}

Cube_match operator/(Cube_match a,long unsigned int i){
	#define X(A,B) a.B/=i;
	CUBE_MATCH(X)
	#undef X
	return a;
}

Cube_match& operator+=(Cube_match& a,Cube_match b){
	#define X(A,B) a.B+=b.B;
	CUBE_MATCH(X)
	#undef X
	return a;
}

bool operator<(Cube_match,Cube_match)nyi

bool operator==(Cube_match,Cube_match)nyi

//ostream& opoera

/*Cube_match mean(Nonempty_vector<Cube_match> a){
	
}*/

using Cube_capabilities=vector<Cube_match>;

Cube_capabilities mean(Nonempty_vector<Cube_capabilities> in){
	return flatten(in);
}

vector<Cubes_scored> scale_cubes(vector<Cube_match> const& a){
	return mapf([](auto x){ return x.scale_cubes; },a);
}

vector<Cubes_scored> switch_cubes(vector<Cube_match> const& a){
	return mapf([](auto x){ return x.switch_cubes; },a);
}

vector<Cubes_scored> vault_cubes(vector<Cube_match> const& a){
	return mapf([](auto x){ return x.vault_cubes; },a);
}

double average_total_cubes(Cube_capabilities a){
	return mean_or_0(scale_cubes(a))+mean_or_0(switch_cubes(a))+mean_or_0(vault_cubes(a));
}

template<typename T>
vector<T> dead(const vector<T>*){
	return mapf([](auto){ return dead((T*)0); },range(5));
}

template<typename A,typename B>
pair<A,B> dead(const pair<A,B>*){
	return make_pair(dead((A*)0),dead((B*)0));
}

double scale_expectation(Skellam_cdf const& skellam_cdf,double cubes1,double cubes2){
	assert(cubes1>=0);
	assert(cubes2>=0);

	//returns points in favor of the first team

	//this is the stupidest impl that will sort of do the right thing.
	/*if(cubes1>cubes2) return 135;
	if(cubes1<cubes2) return -135;
	return 0;*/

	//PRINT(cubes1);
	//PRINT(cubes2);

	//next step: make it instead be based on a Skellam distribution & some summations.
	static const int INTERVALS=67;
	return 2*sum(mapf(
		[&](auto t){
			auto c1=cubes1*t/INTERVALS;
			auto c2=cubes2*t/INTERVALS;
			/*PRINT(c1);
			PRINT(c2);
			auto a=skellam_cdf(c1,c2,-1);
			auto b=skellam_cdf(c2,c1,-1);
			PRINT(a);
			PRINT(b);*/
			return skellam_cdf(c2,c1,-1)-skellam_cdf(c1,c2,-1);
		},
		range_st<int,INTERVALS>()
	));
}

int vault_value(Cubes_scored cubes){
	/*individual cube values:
	 * 15/15/15
	 * 10/10/10
	 * 5/5/5
	 * */
	if(cubes<1) return 0;
	if(cubes<2) return 15;
	if(cubes<3) return 30;
	if(cubes<4) return 45;
	if(cubes<5) return 55;
	if(cubes<6) return 65;
	if(cubes<7) return 75;
	if(cubes<8) return 80;
	if(cubes<9) return 85;
	return 95;
}

double vault(Cube_capabilities a){
	return mean_or_0(vault_cubes(a));
}

double expected_outcome(Skellam_cdf const& skellam_cdf,Cube_capabilities a,Cube_capabilities b){
	//TODO: Make it so that each of the teams can try different strategies.
	/*auto mean_or_zero=[](auto a){
		if(a.empty())nyi
		return mean(make_nonempty(a));
	};*/
	return scale_expectation(skellam_cdf,mean_or_0(scale_cubes(a)),mean_or_0(scale_cubes(b)))+
		scale_expectation(skellam_cdf,mean_or_0(switch_cubes(a)),mean_or_0(switch_cubes(b)))+
		vault_value(vault(a))-vault_value(vault(b));
}

#define AUTO_CAPABILITIES(X)\
	X(double,scale_cubes)\
	X(double,switch_cubes)\

struct Auto_capabilities{
	AUTO_CAPABILITIES(INST)
};

ostream& operator<<(std::ostream& o,Auto_capabilities const& a){
	o<<"Auto_capabilities(";
	AUTO_CAPABILITIES(SHOW)
	return o<<")";
}

bool operator<(Auto_capabilities const& a,Auto_capabilities const& b){
	#define X(A,B) CMP1(B)
	AUTO_CAPABILITIES(X)
	#undef X
	return 0;
}

bool operator==(Auto_capabilities const& a,Auto_capabilities const& b){
	#define X(A,B) if(a.B!=b.B) return 0;
	AUTO_CAPABILITIES(X)
	#undef X
	return 1;
}

bool operator!=(Auto_capabilities const& a,Auto_capabilities const& b){
	return !(a==b);
}

Auto_capabilities& operator+=(Auto_capabilities& a,Auto_capabilities b){
	#define X(A,B) a.B+=b.B;
	AUTO_CAPABILITIES(X)
	#undef X
	return a;
}

double rand(const double*){
	return (rand()%100)/50.0;
}

Auto_capabilities rand(const Auto_capabilities*){
	Auto_capabilities r;
	#define X(A,B) r.B=rand((A*)0);
	AUTO_CAPABILITIES(X)
	#undef X
	return r;
}

double dead(const double*){
	return 0;
}

Auto_capabilities dead(const Auto_capabilities*){
	Auto_capabilities r;
	#define X(A,B) r.B=dead((A*)0);
	AUTO_CAPABILITIES(X)
	#undef X
	return r;
}

template<typename A,typename B>
pair<A,B>& operator/=(pair<A,B> &p,long unsigned int i){
	p.first/=i;
	p.second/=i;
	return p;
}

Auto_capabilities operator/(Auto_capabilities a,long unsigned int i){
	#define X(A,B) a.B/=i;
	AUTO_CAPABILITIES(X)
	#undef X
	return a;
}

double expected_value(std::array<Auto_capabilities,3> a){
	auto s=sum(a);
	auto switch_value=[&](){
		auto switch_cubes=s.switch_cubes;
		if(switch_cubes<1){
			return 10*switch_cubes;
		}
		return 10+(switch_cubes-1)*3;
	}();
	//auto vault value=0
	return switch_value+s.scale_cubes*10;
}

#define ROBOT_CAPABILITIES(X)\
	X(Cube_capabilities,cubes)\
	X(Climb_capabilities,climb)\
	X(Auto_capabilities,auton)\

struct Robot_capabilities{
	ROBOT_CAPABILITIES(INST)
};

ostream& operator<<(ostream& o,Robot_capabilities const& a){
	o<<"Robot_capabilities(";
	ROBOT_CAPABILITIES(SHOW)
	return o<<")";
}

bool operator<(Robot_capabilities const& a,Robot_capabilities const& b){
	#define X(A,B) CMP1(B)
	ROBOT_CAPABILITIES(X)
	#undef X
	return 0;
}

bool operator>(Robot_capabilities const& a,Robot_capabilities const& b){
	return b<a;
}

bool operator==(Robot_capabilities const& a,Robot_capabilities const& b){
	#define X(A,B) if(a.B!=b.B) return 0;
	ROBOT_CAPABILITIES(X)
	#undef X
	return 1;
}

bool operator!=(Robot_capabilities const& a,Robot_capabilities const& b){
	return !(a==b);
}

Robot_capabilities rand(const Robot_capabilities*){
	return Robot_capabilities{
		rand((Cube_capabilities*)0),
		rand((Climb_capabilities*)0),
		rand((Auto_capabilities*)0)
	};
}

Robot_capabilities dead(const Robot_capabilities*){
	return Robot_capabilities{
		dead((Cube_capabilities*)0),
		dead((Climb_capabilities*)0),
		dead((Auto_capabilities*)0)
	};
}

Robot_capabilities mean(Nonempty_vector<Robot_capabilities> const& a){
	return Robot_capabilities{
		#define X(NAME) mean(mapf([](auto x){ return x.NAME; },a))
		X(cubes), X(climb), X(auton)
		#undef X
	};
}

double average_total_cubes(Robot_capabilities a){
	return average_total_cubes(a.cubes);
}

map<Team,Robot_capabilities> interpret(vector<Scouting_row> a){
	auto g=group([](auto x){ return x.team; },a);
	return map_map(
		[](auto a)->Robot_capabilities{
			return Robot_capabilities{
				mapf(
					[](auto x){
						return Cube_match{
							Cubes_scored(x.teleop_scale_cubes),
							Cubes_scored(x.teleop_switch_cubes),
							Cubes_scored(x.teleop_vault_cubes)
						};
					},
					a
				),
				Climb_capabilities(),
				Auto_capabilities()
			};
			print_lines(a);
			nyi
		},
		g
	);
}

using Alliance_capabilities=array<Robot_capabilities,3>;

Alliance_capabilities dead(const Alliance_capabilities*){
	auto d=dead((Robot_capabilities*)nullptr);
	return Alliance_capabilities{d,d,d};
}

array<Climb_capabilities,3> climbs(Alliance_capabilities a){
	return mapf([](auto x){ return x.climb; },a);
}

array<Cube_capabilities,3> cubes(Alliance_capabilities a){
	return mapf([](auto x){ return x.cubes; },a);
}

Cube_capabilities distill_cubes(Alliance_capabilities a){
	auto c=cubes(a);
	Cube_capabilities r;
	/*for(auto v0:c[0]){
		for(auto v1:c[1]){
			for(auto v2:c[2]){
				r|=v0+v1+v2;
			}
		}
	}*/
	//r|=sum(MAP(mean,c));
	r|=sum(mapf(
		[](vector<Cube_match> in){
			if(in.size()) return mean(make_nonempty(in));
			return dead((Cube_match*)0);
		},
		c
	));
	return r;
};

array<Auto_capabilities,3> auton(Alliance_capabilities const& a){
	return mapf([](auto x){ return x.auton; },a);
}

double expected_outcome(Skellam_cdf const& skellam_cdf,Alliance_capabilities a,Alliance_capabilities b){
	//returns # of points expect alliance 'a' to win by.
	double auto_pts=expected_value(auton(a))-expected_value(auton(b));
	double climb=expected_value(climbs(a))-expected_value(climbs(b));

	auto from_cubes=expected_outcome(
		skellam_cdf,
		distill_cubes(a),
		distill_cubes(b)
	);
	//ignoring the vault right now.
	return auto_pts+climb+from_cubes;
}

Alliance_capabilities random_alliance(vector<Robot_capabilities> in){
	auto t=take(3,in);
	if(t.size()!=3) throw "Not enough teams for an alliance";
	return as_array<Robot_capabilities,3>(t);
}

auto random_alliance2(vector<Robot_capabilities> in){
	auto t=take_rest(3,in);
	if(t.first.size()!=3) throw "Not enough teams for an alliance";
	return make_pair(as_array<Robot_capabilities,3>(t.first),t.second);
}

template<typename It,typename Out,typename Func>
void transform1(It in_begin,It in_end,Out /*out*/,Func f){
	vector<pid_t> pids;
	for(auto at=in_begin;at!=in_end;++at){
		auto pid=fork();
		if(pid==0){
			//child
			auto r=f(*at);
			PRINT(r);
			nyi
		}
		assert(pid!=-1);
		pids|=pid;
	}
	for(auto pid:pids){
		int status;
		waitpid(pid,&status,0);
		//TODO: check status
	}
	nyi
}

vector<pair<double,unsigned>> make_picklist_inner_par(unsigned picker,vector<Robot_capabilities> const& robot_capabilities,optional<array<unsigned,3>> known_opponents){
	assert(picker<robot_capabilities.size());
	Skellam_cdf skellam_cdf;

	auto const& own_capabilities=robot_capabilities[picker];
	auto robots=robot_capabilities.size();
	vector<pair<double,unsigned>> r;

	auto potential_partners=filter([picker](auto x){ return x!=picker; },range(robots));

	//TODO: Try to switch this to the average of the 20th-26th best cube robots.
	//(might matter more w/ climb stuff)
	auto third_robot=[&](){
		auto s=reversed(sorted(robot_capabilities,[](auto a){ return average_total_cubes(a); }));
		auto t=take_range(22,26,s);
		if(t.empty()){
			return dead((Robot_capabilities*)0);
		}
		return mean(make_nonempty(t));
	}();

	std::transform(
		begin(potential_partners),
		end(potential_partners),
		std::back_inserter(r),
		[&](auto partner){
			//return make_pair(4.0,4);
			Alliance_capabilities alliance{own_capabilities,robot_capabilities[partner],third_robot};
			cout<<".";
			cout.flush();

			auto opponents=seconds(filter([picker,partner](auto p){ return p.first!=picker && p.first!=partner; },enumerate(robot_capabilities)));
			auto interesting_opponents=take(
				3,
				reversed(sorted(
					opponents,
					[](auto a){ return mean_or_0(scale_cubes(a.cubes))+mean_or_0(switch_cubes(a.cubes))+mean_or_0(vault_cubes(a.cubes))/3.0; }
				))
			);

			vector<double> a;
			if(known_opponents){
				Alliance_capabilities opponents{
					robot_capabilities[(*known_opponents)[0]],
					robot_capabilities[(*known_opponents)[1]],
					robot_capabilities[(*known_opponents)[2]]
				};
				a|=expected_outcome(skellam_cdf,alliance,opponents);
			}else{
				for(auto p:nonduplicate_pairs(interesting_opponents)){
					Alliance_capabilities opponents{p.first,p.second,third_robot};
					a|=expected_outcome(skellam_cdf,alliance,opponents);
				}
			}
			return make_pair(min(a),unsigned(partner));
		}
	);

	return reversed(sorted(r));
}

auto make_picklist(const Team picker,map<Team,Robot_capabilities> robot_capabilities,optional<array<Team,3>> known_opponents){
	assert(robot_capabilities.count(picker));
	if(known_opponents){
		for(auto team:*known_opponents){
			assert(robot_capabilities.count(team));
		}
	}

	//PRINT(picker);
	vector<Team> teams;
	vector<Robot_capabilities> capabilities;
	for(auto p:robot_capabilities){
		teams|=p.first;
		capabilities|=p.second;
	}

	auto get_index=[&](Team team)->unsigned{
		for(auto p:enumerate(teams)){
			if(p.second==team) return p.first;
		}
		assert(0);
	};

	auto d=make_picklist_inner_par(
		//picker,
		[&](){
			for(auto p:enumerate(teams)){
				if(p.second==picker){
					return p.first;
				}
			}
			assert(0);
		}(),
		capabilities,
		[&](){
			if(known_opponents){
				return optional<array<unsigned,3>>{mapf(get_index,*known_opponents)};
			}
			return optional<array<unsigned,3>>();
		}()
	);
	return mapf(
		[=](pair<double,int> p){
			return make_pair(p.first,teams[p.second]);
		},
		d
	);
}

using Cube_average=tuple<double,double,double>;

#define ROBOT_SIMPLE(X)\
	X(Auto_capabilities,auton)\
	X(Cube_average,cubes)\
	X(Climb_capabilities,climb)

struct Robot_simple{
	ROBOT_SIMPLE(INST)

	Robot_simple(Robot_capabilities a):
		auton(a.auton),
		cubes(
			mean_or_0(scale_cubes(a.cubes)),
			mean_or_0(switch_cubes(a.cubes)),
			mean_or_0(vault_cubes(a.cubes))
		),
		climb(a.climb)
	{}
};

ostream& operator<<(ostream& o,Robot_simple const& a){
	o<<"Robot_simple(";
	ROBOT_SIMPLE(SHOW)
	return o<<")";
}

double solo_points(Robot_capabilities const& a){
	Skellam_cdf skellam_cdf;
	auto d=dead((Robot_capabilities*)nullptr);
	Alliance_capabilities a1{a,d,d};
	Alliance_capabilities a2{dead((Alliance_capabilities*)nullptr)};

	/*auto d1=distill_cubes(a1);
	auto d2=distill_cubes(a2);

	auto v=expected_outcome(skellam_cdf,d1,d2);
	PRINT(v);*/

	return expected_outcome(skellam_cdf,a1,a2);
}

vector<pair<pair<double,Team>,vector<pair<double,Team>>>> make_second_picks(
	Team picker,
	vector<pair<double,Team>> pick_list_d,
	map<Team,Robot_capabilities> robot_capabilities,
	optional<array<Team,3>> known_opponents
){
	auto pick_list=seconds(pick_list_d);
	assert(robot_capabilities.count(picker));
	assert(filter([picker](auto x){ return x==picker; },pick_list).empty());
	for(auto team:pick_list){
		assert(robot_capabilities.count(team));
	}

	//PRINT(picker);
	//PRINT(pick_list);
	Skellam_cdf skellam_cdf;

	vector<pair<pair<double,Team>,vector<pair<double,Team>>>> out;
	for(auto p:take(15,pick_list_d)){
		auto partner=p.second;
		auto other_robots=filter(
			[&](auto a){ return a!=picker && a!=partner; },
			firsts(robot_capabilities)
		);
		vector<pair<double,Team>> values;
		for(auto candidate:other_robots){
			Alliance_capabilities alliance{robot_capabilities[picker],robot_capabilities[partner],robot_capabilities[candidate]};

			auto opponents=[&]()->Alliance_capabilities{
				if(known_opponents){
					return Alliance_capabilities{
						robot_capabilities[(*known_opponents)[0]],
						robot_capabilities[(*known_opponents)[1]],
						robot_capabilities[(*known_opponents)[2]]
					};
				}
				auto opponents1=filter([partner,candidate](auto x){ return x!=partner && x!=candidate; },pick_list);
				auto top_opponents=take(2,opponents1);
				assert(top_opponents.size()==2);
				return Alliance_capabilities{
					robot_capabilities[top_opponents[0]],
					robot_capabilities[top_opponents[1]],
					robot_capabilities[index_or_last(22,opponents1)]
				};
			}();
			auto exp=expected_outcome(skellam_cdf,alliance,opponents);
			values|=make_pair(exp,candidate);
		}
		//PRINT(partner);
		auto second_picks=take(22,reversed(sorted(values)));
		//print_lines(second_picks);
		out|=make_pair(p,second_picks);

		cout<<".";
		cout.flush();
	}
	return out;
}


string as_html(vector<pair<pair<double,Team>,vector<pair<double,Team>>>> in){
	auto title1="Team 1425 Picklist";
	return html(
		head(title(title1))+
		body(
			h1(title1)+
			tag("table","border",
				tr(
					tag("th","rowspan=2 colspan=2","First pick")+
					tag("th","colspan=22","Second pick")
				)+
				tr(join(mapf(
					[](auto i){ return th(as_string(i)); },
					range(1,23)
				)))+
				join(mapf(
					[](pair<size_t,pair<pair<double,Team>,vector<pair<double,Team>>>> p1){
						auto p=p1.second;
						return tr(
							th(as_string(p1.first))+
							tag("td","align=right",as_string(p.first.second)+"<br>"+small(as_string(int(p.first.first))))+
							join(mapf(
								[](auto x){
									//return td(as_string(x));
									return tag("td","align=right",as_string(x.second)+"<br>"+small(as_string(int(x.first))));
								},
								p.second
							))
						);
					},
					enumerate_from(1,in)
				))
			)
		)
	);
}

string colorize(double f,string s){
	//red->green?
	//white->green?
	auto color=[&](){
		auto x=int(f*255);
		stringstream ss;
		ss<<"#"<<setfill('0')<<setw(2)<<hex<<(255-x)<<x<<"00";
		return ss.str();
	}();
	return tag("font","color=\""+color+"\"",s);
}

string show_robots(map<Team,Robot_capabilities> const& in){
	auto title1="Team 1425: Estimated robot skills";
	vector<pair<Team,vector<double>>> data=mapf(
		[](auto p){
			return make_pair(
				p.first,
				vector<double>{
					p.second.auton.scale_cubes,
					p.second.auton.switch_cubes,
					mean_or_0(p.second.cubes).scale_cubes,
					mean_or_0(p.second.cubes).switch_cubes,
					p.second.climb.drives,
					p.second.climb.itself,
					p.second.climb.bar1,
					p.second.climb.bar2,
					p.second.climb.lift1,
					p.second.climb.lift2,
					p.second.climb.all1,
					p.second.climb.all2
				}
			);
		},
		in
	);

	auto data_limits=mapf(limits,transpose(seconds(data)));

	return html(
		head(title(title1))+
		body(
			h1(title1)+
			tag("table","border",
				tr(join(mapf(th,vector<string>{
					"Team",
					"Auto scale cubes",
					"Auto switch cubes",
					"Scale cubes",
					"Switch cubes",
					"Climb_drive",
					"Climb_itself",
					"Climb_bar1",
					"Climb_bar2",
					"Climb_lift1",
					"Climb_lift2",
					"Climb_all1",
					"Climb_all2"
				})))+
				join(mapf(
					[&](auto p){
						return tr(
							td(as_string(p.first))+
							join(mapf(
								[](auto p1){
									auto [min,max]=p1.first;
									auto f=p1.second;
									auto portion=(f-min)/(max-min);
							       		return td(as_string(f)+" "+colorize(portion,small(as_2decimals(portion))));
								},
								zip(data_limits,p.second)
							)
						));
					},
					data
				))
			)
		)
	);
}

vector<string> args(int argc,char **argv){
	vector<string> r;
	for(auto i:range(1,argc)){
		r|=string(argv[i]);
	}
	return r;
}

int main1(int argc,char **argv){
	auto x=rand((map<Team,Robot_capabilities>*)nullptr);
	//x=take(5,x);

	write_file("robots.html",show_robots(x));
	
	/*cout<<"Example robots:\n";
	//print_lines(x);
	for(auto a:take(5,x)){
		//PRINT(a);
		PRINT(Robot_simple(a.second));
		PRINT(solo_points(a.second));
	}*/
	
	auto a=args(argc,argv);

	assert(x.size());
	auto picker=begin(x)->first;

	optional<array<Team,3>> known_opponents;

	using It=vector<string>::iterator;
	map<string,pair<string,std::function<int(It&,It)>>> flags;
	flags.insert(make_pair(
		"--picker",
		make_pair(
			"Team number that is doing the picking.  Defaults to the lowest numbered team.",
			[&](It& at,It end){
				if(at==end){
					cout<<"Error: Expected team number.\n";
					return 1;
				}
	
				picker=atoi(*at);
				++at;

				if(x.count(picker)==0){
					cout<<"Error: Team not found in data.  Cannot be used as picker.\n";
					return 1;
				}
				return 0;
			}
		)
	));

	flags.insert(make_pair(
		"--known_opponents",
		make_pair(
			"Assume that the opposing alliance is the set of three given team numbers",
			[&known_opponents,&x](It &at,It end){
				if(known_opponents){
					cout<<"Error: Opponents already specified.\n";
					return 1;
				}
				vector<Team> teams;
				for(auto _:range(3)){
					(void)_;
					if(at==end){
						cout<<"Error: Expected 3 team numbers of opponents.\n";
						return 1;
					}
					auto team=atoi(*at);
					if(x.count(team)==0){
						cout<<"Error: Could not find data about opponent: "<<team<<".\n";
						cout<<"Known teams:"<<firsts(x)<<"\n";
						return 1;
					}
					teams.push_back(atoi(*at));
					at++;
				}
				known_opponents=as_array<Team,3>(teams);
				return 0;
			}
		)
	));

	auto show_help=[&](){
		for(auto p:flags){
			cout<<p.first<<"\n";
			cout<<"\t"<<p.second.first<<"\n";
		}
	};

	flags.insert(make_pair(
		"--help",
		make_pair(
			"Display this message",
			[&show_help](It &/*at*/,It /*end*/){
				show_help();
				return 1;
			}
		)
	));

	for(auto at=begin(a);at!=end(a);){
		auto f=flags.find(*at);
		if(f==flags.end()){
			cout<<"Error: Unrecognized argument:"<<*at<<"\n";
			show_help();
			return 1;
		}
		at++;
		auto r=f->second.second(at,end(a));
		if(r) return r;
	}

	auto picks=make_picklist(picker,x,known_opponents);

	cout<<"\n";
	print_lines(enumerate_from(1,picks));

	auto m=make_second_picks(picker,picks,x,known_opponents);

	cout<<"\n";
	print_lines(m);
	write_file("out.html",as_html(m));

	return 0;

	/*auto scouting_data=read_csv("demo.csv");
	auto useful=interpret(scouting_data);
	//auto a1=take(3,seconds(useful));
	auto [a1,rest]=random_alliance2(seconds(useful));
	auto a2=random_alliance(rest);
	PRINT(a1);
	PRINT(a2);
	PRINT(expected_outcome(Skellam_cdf(),a1,a2));
	return 0;*/
}

int main(int argc,char **argv){
	/*const int threads_wanted = 20;
	omp_set_dynamic(false);
	omp_set_num_threads(threads_wanted);*/

	try{
		return main1(argc,argv);
	}catch(const char *s){
		cout<<s<<"\n";
		return 1;
	}catch(string const& s){
		cout<<s<<"\n";
		return 1;
	}
}
