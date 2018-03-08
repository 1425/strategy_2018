#include "decode.h"
#include<fstream>
#include<iomanip>

using namespace std;

//start generic functions

template<typename K,typename V>
vector<pair<K,V>> to_vector(map<K,V> in){
	vector<pair<K,V>> r;
	for(auto p:in) r|=make_pair(p.first,p.second);
	return r;
}

std::pair<double,double> limits(std::vector<double> a){
	return std::make_pair(min(a),max(a));
}

template<typename T>
bool all_equal(std::vector<T> a){
	if(a.empty()) return 1;
	for(auto elem:a){
		if(elem!=a[0]){
			return 0;
		}
	}
	return 1;
}

template<typename T>
std::vector<std::vector<T>> transpose(std::vector<std::vector<T>> in){
	if(in.empty()) return {};

	auto s=MAP(size,in);
	assert(all_equal(s));
	auto len=s[0];

	std::vector<std::vector<T>> r;
	for(auto i:range(len)){
		std::vector<T> row;
		for(auto a:in){
			row|=a[i];
		}
		r.push_back(row);
	}
	return r;
}

std::string as_2decimals(double a){
	std::stringstream ss;
	ss<<std::fixed<<std::setprecision(2)<<a;
	return ss.str();
}


//start program-specific functions

using Team=unsigned int;

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

#if 0
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
#endif

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

std::pair<std::array<Robot_capabilities,3>,std::vector<Robot_capabilities>> random_alliance2(vector<Robot_capabilities> in){
	auto t=take_rest(3,in);
	if(t.first.size()!=3) throw "Not enough teams for an alliance";
	return make_pair(as_array<Robot_capabilities,3>(t.first),t.second);
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

string capabilities_table(vector<pair<Team,Robot_capabilities>> const& in){
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

	return tag("table","border",
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
	);
}

string capabilities_table(map<Team,Robot_capabilities> const& in){
	return capabilities_table(to_vector(in));
}

