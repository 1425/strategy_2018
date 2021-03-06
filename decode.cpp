#include "decode.h"
#include<fstream>
#include<iomanip>
#include "skellam_local.h"

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

#define SIMPLE_CUBES(X)\
	X(auto_scale)\
	X(auto_switch)\
	X(tele_scale)\
	X(tele_switch)\
	X(vault)

struct Simple_cubes{
	#define X(A) double A;
	SIMPLE_CUBES(X)
	#undef X
};

ostream& operator<<(ostream& o,Simple_cubes const& a){
	o<<"Simple_cubes( ";
	#define X(A) o<<""#A<<":"<<a.A<<" ";
	SIMPLE_CUBES(X)
	#undef X
	return o<<")";
}

Alliance decode(std::string const& s,const Alliance*){
	if(s=="blue") return Alliance::BLUE;
	if(s=="red") return Alliance::RED;
	stringstream ss;
	ss<<"Invalid alliance:"<<s;
	throw ss.str();
}

Team::Team():data(0){
	//invalid!
}

Team::Team(unsigned i):data(i){}

Team::operator unsigned()const{
	return data;
}

std::ostream& operator<<(std::ostream& o,Team a){
	return o<<unsigned(a);
}

Team decode(std::string const& s,const Team*){
	return decode(s,(unsigned*)nullptr);
}

Team rand(const Team*){
	return Team{rand((unsigned*)nullptr)};
}

ostream& operator<<(std::ostream& o,Always_null const&){
	return o<<"(NULL)";
}

bool operator<(Always_null,Always_null){
	return 0;
}

Always_null decode(std::string const& s,const Always_null*){
	if(s!="NULL"){
		std::stringstream ss;
		ss<<"Error: Expected \"NULL\" but got: "<<s;
		throw ss.str();
	}
	return Always_null{};
}

ostream& operator<<(ostream& o,Scouting_row1 const& a){
	o<<"Scouting_row1(";
	SCOUTING_ROW1(SHOW)
	return o<<")";
}

ostream& operator<<(ostream& o,Scouting_row2 const& a){
	o<<"Scouting_row2(";
	SCOUTING_ROW2(SHOW)
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
	if(s=="Yes") return 1;
	if(s=="No") return 0;
	stringstream ss;
	ss<<"Invalid bool: \""<<s<<"\"";
	throw ss.str();
}

unsigned decode(string s,const unsigned*){
	auto i=atoi(s.c_str());
	assert(i>=0);
	return i;
}

template<typename T>
std::optional<T> decode(string s,const std::optional<T>*){
	if(s=="NULL"){
		return std::nullopt;
	}
	return decode(s,(T*)nullptr);
}

#define SCOUTING_ROW SCOUTING_ROW2

vector<Scouting_row> read_csv(string filename){
	ifstream f(filename);
	vector<Scouting_row> r;
	bool first=1;
	while(f.good()){
		string s;
		getline(f,s);
		if(s.empty()) continue;

		auto sp=split(s,',');
		if(first){
			first=0;
			#define X(A,B) assert(sp.size()); {\
				auto v=car(sp);\
				sp=cdr(sp);\
				if(""#B!=v){\
					cout<<"B:"#B<<" "<<v<<"\n";\
				}\
				assert(""#B==v);\
			}
			SCOUTING_ROW(X)
			#undef X
		}else{
			Scouting_row row;
			#define X(A,B) assert(sp.size()); {\
				auto v=car(sp);\
				sp=cdr(sp);\
				try{\
					row.B=decode(v,(A*)nullptr);\
				}catch(string const& s){\
					throw string()+""#B+": "+s;\
				}\
			}
			SCOUTING_ROW(X)
			#undef X
			//PRINT(sp);
			assert(sp.empty());
			r|=row;
		}
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

double share_bar_climb(std::array<Climb_capabilities,3> a){
	auto s=sorted(mapf([](auto x){ return x.itself; },a));
	return 30*(s[0]+s[1])+5;
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
		share_bar_climb(a),
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

ostream& operator<<(ostream& o,Cube_match a){
	o<<"Cube_match( ";
	#define X(A,B) o<<""#B<<":"<<a.B<<" ";
	CUBE_MATCH(X)
	#undef X
	return o<<")";
}

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

double scale_expectation(int initial_condition,double cubes1,double cubes2){
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
		[&](auto t)->double{
			auto c1=cubes1*t/INTERVALS;
			auto c2=cubes2*t/INTERVALS;
			/*PRINT(c1);
			PRINT(c2);
			auto a=skellam_cdf(c1,c2,-1);
			auto b=skellam_cdf(c2,c1,-1);
			PRINT(a);
			PRINT(b);*/
			//(void)c1; (void)c2; nyi//return skellam_cdf(c2,c1,-1)-skellam_cdf(c1,c2,-1);
			auto p=skellam_totals(initial_condition,c1,c2);
			return p.second-p.first;
		},
		range_st<int,INTERVALS>()
	));
}

double vault_value(Cubes_scored cubes){
	/*individual cube values:
	 * 15/15 (12.5?)/15
	 * 10/10/10
	 * 5/5/5
	 * 
	 * TODO: Figure out if there is a more linear function to do this so that don't end up with weird thresholds.
	 * */
	if(cubes<1) return 0;
	if(cubes<2) return 15*(cubes-1);
	if(cubes<3) return 15+15*(cubes-2);
	if(cubes<4) return 30+15*(cubes-3);
	if(cubes<5) return 45+10*(cubes-4);
	if(cubes<6) return 55+10*(cubes-5);
	if(cubes<7) return 65+10*(cubes-6);
	if(cubes<8) return 75+5*(cubes-7);
	if(cubes<9) return 80+5*(cubes-7);
	return 95;
}

double vault(Cube_capabilities a){
	return mean_or_0(vault_cubes(a));
}

//double expected_outcome(Simple_cubes a,Simple_cubes b){
double expected_outcome(Cube_capabilities a,Cube_capabilities b){
	//TODO: Make it so that each of the teams can try different strategies.

	/*
	Version used at 2018waamv/2018pncmp
 	return scale_expectation(mean_or_0(scale_cubes(a)),mean_or_0(scale_cubes(b)))+
		scale_expectation(
			mean_or_0(scale_cubes(a)+switch_cubes(a)),
			mean_or_0(scale_cubes(b)+switch_cubes(b))
		)+
		vault_value(vault(a))-vault_value(vault(b));*/

	const auto a_scale=mean_or_0(scale_cubes(a));
	const auto b_scale=mean_or_0(scale_cubes(b));
	const auto a_switch=mean_or_0(switch_cubes(a));
	const auto b_switch=mean_or_0(switch_cubes(b));
	auto a_vault=vault(a);
	auto b_vault=vault(b);

	static const unsigned PYRAMID=10;
	static const unsigned SWITCH=6;
	static const unsigned PORTAL=7*2;
	static const unsigned MAX_CUBES=2*(PYRAMID+SWITCH+PORTAL);

	//if the match is not running out of cubes, but you are then assume that you're able to grab 2 more cubes from the center of the field than your opponents.
	static const unsigned FROM_OPPONENT_SWITCH=2;

	static const unsigned TEAM_MAX=PYRAMID+SWITCH+PORTAL+FROM_OPPONENT_SWITCH;

	{
		auto team_a=a_scale+a_switch+a_vault;
		if(team_a>TEAM_MAX){
			a_vault=max(0.0,TEAM_MAX-a_scale-a_switch);
		}
	}

	{
		auto team_b=b_scale+b_switch+b_vault;
		if(team_b>TEAM_MAX){
			b_vault=max(0.0,TEAM_MAX-b_scale-b_switch);
		}
	}

	auto total_cubes=a_scale+b_scale+a_switch+b_switch+a_vault+b_vault;
	if(total_cubes>MAX_CUBES){
		//cout<<"match total hit.\n";
		auto excess_cubes=total_cubes-MAX_CUBES;
		auto vault_cubes=a_vault+b_vault;
		auto to_remove=min(excess_cubes,vault_cubes);
		auto frac_to_retain=(1-to_remove/vault_cubes);
		a_vault*=frac_to_retain;
		b_vault*=frac_to_retain;
	}

	return scale_expectation(0,a_scale,b_scale)+
		scale_expectation(
			0,
			mean_or_0(scale_cubes(a)+switch_cubes(a)),
			mean_or_0(scale_cubes(b)+switch_cubes(b))
		)+
		vault_value(a_vault)-vault_value(b_vault);
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
	auto g=group([](auto x){ return x.team_number; },a);
	auto r=map_map(
		[](auto a)->Robot_capabilities{
			Robot_capabilities r{
				mapf(
					[](auto x){
						return Cube_match{
							Cubes_scored(x.auto_scale_cube+x.scale_cube),
							Cubes_scored(x.auto_switch_cube+x.home_switch_cube+x.opp_switch_cube),
							Cubes_scored(x.auto_exchange_cube+x.exchange_cube)
						};
					},
					a
				),
				[&]()->Climb_capabilities{
					/*auto m=mapf(
						[](auto x){
							PRINT(x.no_climb);
							PRINT(x.piggyback_climb);
							PRINT(x.climb);
							PRINT(x.assist);
							return Climb_capabilities();
						},
						a
					);
					return mean_or_0(m);*/
					//if(a.
					Climb_capabilities cc{};
					cc.drives=Px{.5};
					//PRINT(a);
					cc.itself=Px{mean_or_0(
						mapf(
							[](auto a){ return double(*a); },
							filter(
								[](auto a){ return a.has_value(); },
								mapf([](auto x){ return x.climb; },a)
							)
						)
					)}; //Px{.81};
					cc.bar1=Px{0};
					cc.bar2=Px{0};
					cc.lift1=Px{0};
					cc.lift2=Px{0};
					cc.all1=Px{0};
					cc.all2=Px{0};
					return cc;
				}(),
				Auto_capabilities{
					mean_or_0(mapf([](auto x){ return double(x.auto_scale_cube); },a)),
					mean_or_0(mapf([](auto x){ return double(x.auto_switch_cube); },a))
				}
			};
			//r.cubes.switch_cubes=max(r.cubes.switch_cubes,r.cubes.scale_cubes);
			return r;
			print_lines(a);
			nyi
		},
		g
	);

	//START SPECIAL MUNGING
	r[1425].climb.all1=Px{.8};
	r[1425].climb.all2=Px{.7};
	/*r[2471].climb.all1=Px{.8};
	r[2471].climb.all2=Px{.7};*/

	vector<Team> mecanums{
		//753,847,4132,4309
		492,847,948,949,2605,2557,2910,4579,3786,
		1421,2556,6832,1622,580,6838
	};
	for(auto t:mecanums){
		auto f=r.find(t);
		if(f!=r.end()){
			f->second.climb.drives=Px{0};
		}
	}
	vector<Team> climb_on_top{
		6445,2733,5450,2046,2522,4512,5920,2928,3663,4061,4469,360,4131,6465,6845
	};
	for(auto t:climb_on_top){
		auto f=r.find(t);
		if(f!=r.end()){
			f->second.climb.drives=Px{.75};
		}
	}

	//r[997].climb.drives=Px{.5};
	//#endif
	//END SPECIAL MUNGING
	//
	return r;
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

#if 0
Simple_cubes distill_cubes(Alliance_capabilities a){
	mapf(
		[](Robot_capabilities robot)->Simple_cubes{
			return {
				mean_or_0(mapf(
				robot.auton.scale/switch
				robot.cubes.scale/switch/vault
			};
		},
		a
	);
	nyi
}
#endif

#if 1
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
#endif

array<Auto_capabilities,3> auton(Alliance_capabilities const& a){
	return mapf([](auto x){ return x.auton; },a);
}

double expected_outcome(Alliance_capabilities a,Alliance_capabilities b){
	//returns # of points expect alliance 'a' to win by.
	double auto_pts=expected_value(auton(a))-expected_value(auton(b));
	double climb=expected_value(climbs(a))-expected_value(climbs(b));

	auto from_cubes=expected_outcome(
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
					mean_or_0(p.second.cubes).vault_cubes,
					p.second.climb.drives,
					p.second.climb.itself,
					//p.second.climb.bar1,
					//p.second.climb.bar2,
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
			"Exchange cubes",
			"Climb_drive",
			"Climb_itself",
			//"Climb_bar1",
			//"Climb_bar2",
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

