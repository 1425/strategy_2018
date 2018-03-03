#include <fstream>
#include <map>
//#include <execution>
#include "util.h"
#include "sub.h"

using namespace std;

//start generic functions

template<typename T>
using Pair2=pair<T,T>;

template<typename K,typename V>
set<K> keys(map<K,V> const& m){
	set<K> r;
	for(auto elem:m) r|=elem.first;
	return r;
}

template<typename Func,typename T>
set<T> filter(Func f,set<T> a){
	set<T> r;
	for(auto elem:a){
		if(f(elem)){
			r|=elem;
		}
	}
	return r;
}

template<typename A,typename B>
pair<A,B> mean(Nonempty_vector<pair<A,B>> a){
	return make_pair(mean(firsts(a)),mean(seconds(a)));
}

template<typename T>
T mean(Nonempty_vector<T> const& a){
	assert(a.size());
	return sum(a)/a.size();
}

template<typename T>
std::ostream& operator<<(std::ostream& o,std::set<T> const& a){
	o<<"{ ";
	for(auto const& elem:a){
		o<<elem<<" ";
	}
	return o<<"}";
}

template<typename T>
std::set<T>& operator|=(std::set<T>& s,T t){
	s.insert(t);
	return s;
}

template<typename T>
std::pair<T,T> sorted(std::pair<T,T> a){
	if(a.first>a.second) std::swap(a.first,a.second);
	return a;
}

template<typename K,typename V>
std::vector<V> values(std::map<K,V> const& a){
	std::vector<V> r;
	for(auto p:a){
		r|=p.second;
	}
	return r;
}

template<typename T>
std::set<std::pair<T,T>> unique_pairs(std::set<T> const& a){
	std::set<std::pair<T,T>> r;
	for(auto const& elem1:a){
		for(auto const& elem2:a){
			if(elem1!=elem2){
				r|=sorted(std::make_pair(elem1,elem2));
			}
		}
	}
	return r;
}

template<typename T>
auto unique_pairs(std::vector<T> a){
	//return unique_pairs(to_set(move(a)));
	std::vector<std::pair<T,T>> r;
	for(auto i:range(a.size())){
		for(auto j:range(i+1,a.size())){
			r|=std::make_pair(a[i],a[j]);
		}
	}
	return r;
}

template<typename K,typename V>
std::map<K,V>& operator|=(std::map<K,V> &a,std::pair<K,V> p){
	a.insert(move(p));
	return a;
}

template<typename K,typename V>
std::map<K,V>& operator|=(std::map<K,V> &a,std::pair<const K,V> p){
	a.insert(move(p));
	return a;
}

template<typename Func,typename K,typename V>
std::map<K,V> filter(Func f,std::map<K,V> const& in){
	std::map<K,V> r;
	for(auto p:in){
		if(f(p)){
			r|=p;
		}
	}
	return r;
}

unsigned rand(const unsigned*){
	return rand()%100;
}

template<typename A,typename B>
std::pair<A,B> rand(const std::pair<A,B>*){
	return std::make_pair(rand((A*)0),rand((B*)0));
}

template<typename T>
std::vector<T> rand(const std::vector<T>*){
	return mapf([](auto){ return rand((T*)0); },range(rand()%40));
}

int rand(const int*){
	return (rand()%100)-50;
}

template<typename K,typename V>
std::map<K,V> rand(const std::map<K,V>*){
	std::map<K,V> r;
	for(auto _:range(10)){
		(void)_;
		r[rand((K*)nullptr)]=rand((V*)nullptr);
	}
	return r;
}

template<typename T>
std::pair<std::vector<T>,std::vector<T>> take_rest(size_t len,std::vector<T> in){
	std::vector<T> a;
	auto actual_len=std::min(len,in.size());
	for(auto i:range(actual_len)){
		a|=std::move(in[i]);
	}

	std::vector<T> b;
	for(auto i:range(actual_len,in.size())){
		b|=std::move(in[i]);
	}
	return make_pair(a,b);
}

template<typename A,typename B>
std::pair<A,B> operator+(std::pair<A,B> a,std::pair<A,B> b){
	return std::make_pair(a.first+b.first,a.second+b.second);
}

template<typename A,typename B>
std::pair<A,B>& operator+=(std::pair<A,B> &a,std::pair<A,B> const& b){
	a.first+=b.first;
	a.second+=b.second;
	return a;
}

template<typename T>
vector<double> to_doubles(vector<T> a){
	return mapf([](auto x){ return double(x); },a);
}

template<typename T>
Nonempty_vector<double> to_doubles(Nonempty_vector<T> const& a){
	return mapf([](auto x){ return double(x); },a);
}

double mean(Nonempty_vector<unsigned> const& a){
	return mean(to_doubles(a));
}

template<typename T>
T car(std::vector<T> const& a){
	assert(a.size());
	return a[0];
}

template<typename T>
std::vector<T> cdr(std::vector<T> const& a){
	std::vector<T> r;
	for(auto i:range(size_t(1),a.size())){
		r|=a[i];
	}
	return r;
}

template<typename T,size_t N>
std::ostream& operator<<(std::ostream& o,std::array<T,N> const& a){
	o<<"[ ";
	for(auto elem:a){
		o<<elem<<" ";
	}
	return o<<"]";
}

template<typename T,size_t N>
std::array<T,N> as_array(std::vector<T> in){
	assert(in.size()==N);
	std::array<T,N> r;
	for(auto i:range(N)){
		r[i]=std::move(in[i]);
	}
	return r;
}

template<typename A,typename B>
std::vector<B> seconds(std::vector<std::pair<A,B>> const& a){
	return mapf([](auto const& x){ return x.second; },a);
}

template<typename A,typename B>
auto seconds(Nonempty_vector<std::pair<A,B>> const& a){
	return mapf([](auto const& x){ return x.second; },a);
}

template<typename K,typename V>
std::vector<V> seconds(std::map<K,V> const& a){
	return mapf([](auto const& x){ return x.second; },a);
}

template<typename Func,typename T>
auto group(Func f,std::vector<T> const& in)->std::map<decltype(f(in[0])),std::vector<T>>{
	std::map<decltype(f(in[0])),std::vector<T>> r;
	for(auto const& elem:in){
		r[f(elem)]|=elem;
	}
	return r;
}

template<typename Func,typename K,typename V>
auto map_map(Func f,std::map<K,V> in)->std::map<K,decltype(f(in.begin()->second))>{
	std::map<K,decltype(f(in.begin()->second))> r;
	for(auto p:in){
		r.insert(std::make_pair(p.first,f(p.second)));
	}
	return r;
}

std::vector<std::string> split(std::string haystack,char needle){
	std::vector<std::string> r;
	std::stringstream ss;
	for(auto c:haystack){
		if(c==needle){
			r|=ss.str();
			ss.str("");
		}else{
			ss<<c;
		}
	}
	if(ss.str().size()){
		r|=ss.str();
	}
	return r;
}

#define RM_CONST(X) typename std::remove_const<X>::type
#define RM_REF(X) typename std::remove_reference<X>::type
#define ELEMENT_TYPE(X) RM_CONST(RM_REF(decltype(*std::begin(X))))

template<typename A,typename B>
auto zip(A a,B b){
	using AE=ELEMENT_TYPE(a);
	using BE=ELEMENT_TYPE(b);
	std::vector<std::pair<AE,BE>> r;
	for(auto ai=std::begin(a),bi=std::begin(b);ai!=std::end(a) && bi!=std::end(b);++ai,++bi){
		r|=std::make_pair(*ai,*bi);
	}
	return r;
}

template<typename T>
class Sorted_pair{
	std::pair<T,T> data;

	public:
	Sorted_pair(T a,T b):
		data(std::make_pair(std::min(a,b),std::max(a,b)))
	{}

	std::pair<T,T> const& get()const{ return data; }
};

template<typename T>
bool operator<(Sorted_pair<T> const& a,Sorted_pair<T> const& b){
	return a.get()<b.get();
}

template<typename T>
bool operator==(Sorted_pair<T> a,Sorted_pair<T> b){
	return a.get()==b.get();
}

template<typename T>
std::ostream& operator<<(std::ostream& o,Sorted_pair<T> const& a){
	return o<<a.get();
}

template<typename T,size_t N>
class Set_limited{
	//the point of this class is to have a set that has a max size.
	//theoretically, this could be implemented in a much more efficient way, but to start, just make it so that it does checks on insert.
	std::set<T> inner;

	public:
	Set_limited();

	Set_limited(std::array<T,N> a){
		for(auto elem:a) inner|=elem;
	}

	auto size()const{ return inner.size(); }

	auto begin()const{ return inner.begin(); }
	auto end()const{ return inner.end(); }

	std::set<T> const& get()const{ return inner; }
};

template<typename T,size_t N>
bool operator<(Set_limited<T,N> const& a,Set_limited<T,N> const& b){
	return a.get()<b.get();
}

template<typename T,size_t N>
bool operator==(Set_limited<T,N> const& a,Set_limited<T,N> const& b){
	return a.get()==b.get();
}

template<typename T,size_t N>
std::ostream& operator<<(std::ostream& o,Set_limited<T,N> const& a){
	return o<<a.get();
}

template<typename T,size_t N>
class Sorted_array{
	std::array<T,N> data;

	public:
	Sorted_array(std::array<T,N> a):data(sorted(a)){}

	std::array<T,N> const& get()const{ return data; }
};

template<typename T,size_t N>
bool operator<(Sorted_array<T,N> const& a,Sorted_array<T,N> const& b){
	return a.get()<b.get();
}

//start program-speicifc functions.

using Team=unsigned int;

#define SCOUTING_ROW(X)\
	X(Team,team)\
	X(string,match)\
	X(unsigned int,auto_switch_cubes)\
	X(unsigned int,auto_scale_cubes)\
	X(unsigned int,teleop_switch_cubes)\
	X(unsigned int,teleop_scale_cubes)\
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

struct Climb_capabilities{
	//TODO: fixme
};

ostream& operator<<(ostream& o,Climb_capabilities ){
	o<<"Climb_capabilities(";

	return o<<")";
}

bool operator<(Climb_capabilities const&,Climb_capabilities const&){
	return 0;
}

bool operator!=(Climb_capabilities const&,Climb_capabilities const&){
	return 0;
}

Climb_capabilities rand(const Climb_capabilities*){
	return {};
}

Climb_capabilities dead(const Climb_capabilities*){
	return {};
}

double expected_value(array<Climb_capabilities,3>){
	//TODO: Once there is data in the Climb_capabilities struct, use it.
	return 0;
}

class Cubes_scored{
	unsigned short value;

	public:
	static const unsigned MAX=50;//approx

	Cubes_scored(unsigned in):value(in){
		if(value>=MAX){
			PRINT(value);
		}
		assert(value<MAX);
	}

	operator unsigned()const{
		return value;
	}

	unsigned get()const{
		return value;
	}
};

Cubes_scored operator+(Cubes_scored a,Cubes_scored b){
	//saturating addition.
	return min(Cubes_scored::MAX-1,a.get()+b.get());
}

Cubes_scored& operator+=(Cubes_scored &a,Cubes_scored b){
	a=(a+b);
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
using Cube_capabilities=vector<pair<Scale_cubes,Switch_cubes>>;

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

	//next step: make it instead be based on a Skellam distribution & some summations.
	return 2*sum(mapf(
		[&](auto t){
			auto c1=cubes1*t/77;
			auto c2=cubes2*t/77;
	       		return skellam_cdf(c1,c2,-1)-skellam_cdf(c2,c1,-1);
		},
		range_st<int,77>()
	));
}

double expected_outcome(Skellam_cdf const& skellam_cdf,Cube_capabilities a,Cube_capabilities b){
	//TODO: Make it so that each of the teams can try different strategies.
	auto mean_or_zero=[](auto a){
		if(a.empty())nyi
		return mean(make_nonempty(a));
	};
	return scale_expectation(skellam_cdf,mean_or_zero(firsts(a)),mean_or_zero(firsts(b)))+
		scale_expectation(skellam_cdf,mean_or_zero(seconds(a)),mean_or_zero(seconds(b)));
}

#define ROBOT_CAPABILITIES(X)\
	X(Cube_capabilities,cubes)\
	X(Climb_capabilities,climb)

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
	return Robot_capabilities{rand((Cube_capabilities*)0),rand((Climb_capabilities*)0)};
}

Robot_capabilities dead(const Robot_capabilities*){
	return Robot_capabilities{dead((Cube_capabilities*)0),dead((Climb_capabilities*)0)};
}

map<Team,Robot_capabilities> interpret(vector<Scouting_row> a){
	auto g=group([](auto x){ return x.team; },a);
	return map_map(
		[](auto a)->Robot_capabilities{
			return Robot_capabilities{
				mapf([](auto x){ return make_pair(Cubes_scored(x.teleop_scale_cubes),Cubes_scored(x.teleop_switch_cubes)); },a),
				Climb_capabilities()
			};
			print_lines(a);
			nyi
		},
		g
	);
}

using Alliance_capabilities=array<Robot_capabilities,3>;

array<Climb_capabilities,3> climbs(Alliance_capabilities a){
	return mapf([](auto x){ return x.climb; },a);
}

array<Cube_capabilities,3> cubes(Alliance_capabilities a){
	return mapf([](auto x){ return x.cubes; },a);
}

double expected_outcome(Skellam_cdf const& skellam_cdf,Alliance_capabilities a,Alliance_capabilities b){
	//returns # of points expect alliance 'a' to win by.
	double climb=expected_value(climbs(a))-expected_value(climbs(b));

	auto distill_cubes=[](Alliance_capabilities a)->Cube_capabilities{
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
			[](vector<pair<Cubes_scored,Cubes_scored>> in){
				if(in.size()) return mean(make_nonempty(in));
				return make_pair(Cubes_scored(0),Cubes_scored(0));
			},
			c
		));
		return r;
	};

	auto from_cubes=expected_outcome(
		skellam_cdf,
		distill_cubes(a),
		distill_cubes(b)
	);
	//ignoring the vault right now.
	return climb+from_cubes;
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

vector<pair<double,unsigned>> make_picklist_inner(unsigned picker,vector<Robot_capabilities> const& robot_capabilities){
	assert(picker<robot_capabilities.size());

	Skellam_cdf skellam_cdf;
	auto const& own_capabilities=robot_capabilities[picker];
	auto robots=robot_capabilities.size();
	vector<pair<double,unsigned>> r;
	for(const auto partner:range(robots)){
		if(partner==picker) continue;
		Alliance_capabilities alliance{own_capabilities,robot_capabilities[partner],dead((Robot_capabilities*)nullptr)};

		vector<double> a;
		for(const auto opponent1:range(robots)){
			if(opponent1==picker || opponent1==partner) continue;
			for(const auto opponent2:range(opponent1+1,robots)){
				if(opponent2==picker || opponent2==partner) continue;
				Alliance_capabilities opponents{robot_capabilities[opponent1],robot_capabilities[opponent2],dead((Robot_capabilities*)nullptr)};
				a|=expected_outcome(skellam_cdf,alliance,opponents);
			}
		}
		r|=make_pair(min(a),unsigned(partner));
	}
	return sorted(r);
}

vector<pair<double,unsigned>> make_picklist_inner_par(unsigned picker,vector<Robot_capabilities> const& robot_capabilities){
	assert(picker<robot_capabilities.size());

	Skellam_cdf skellam_cdf;
	auto const& own_capabilities=robot_capabilities[picker];
	auto robots=robot_capabilities.size();
	vector<pair<double,unsigned>> r;

	auto potential_partners=filter([picker](auto x){ return x!=picker; },range(robots));
	transform(
		//std::parallel_unsequenced_policy{},
		begin(potential_partners),
		end(potential_partners),
		std::back_inserter(r),
		[&](auto partner){
			//return make_pair(4.0,4);
			Alliance_capabilities alliance{own_capabilities,robot_capabilities[partner],dead((Robot_capabilities*)nullptr)};

			vector<double> a;
			for(const auto opponent1:range(robots)){
				if(opponent1==picker || opponent1==partner) continue;
				for(const auto opponent2:range(opponent1+1,robots)){
					if(opponent2==picker || opponent2==partner) continue;
					Alliance_capabilities opponents{robot_capabilities[opponent1],robot_capabilities[opponent2],dead((Robot_capabilities*)nullptr)};
					a|=expected_outcome(skellam_cdf,alliance,opponents);
				}
			}
			return make_pair(min(a),unsigned(partner));
		}
	);

	/*for(const auto partner:range(robots)){
		if(partner==picker) continue;
		Alliance_capabilities alliance{own_capabilities,robot_capabilities[partner],dead((Robot_capabilities*)nullptr)};

		vector<double> a;
		for(const auto opponent1:range(robots)){
			if(opponent1==picker || opponent1==partner) continue;
			for(const auto opponent2:range(opponent1+1,robots)){
				if(opponent2==picker || opponent2==partner) continue;
				Alliance_capabilities opponents{robot_capabilities[opponent1],robot_capabilities[opponent2],dead((Robot_capabilities*)nullptr)};
				a|=expected_outcome(skellam_cdf,alliance,opponents);
			}
		}
		r|=make_pair(min(a),unsigned(partner));
	}*/
	return sorted(r);
}

auto make_picklist(const Team picker,map<Team,Robot_capabilities> robot_capabilities){
	PRINT(picker);
	vector<Team> teams;
	vector<Robot_capabilities> capabilities;
	for(auto p:robot_capabilities){
		teams|=p.first;
		capabilities|=p.second;
	}
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
		capabilities
	);
	return mapf(
		[=](pair<double,int> p){
			return make_pair(p.first,teams[p.second]);
		},
		d
	);

	Skellam_cdf skellam_cdf;
	//first, just assume two-team alliances
	//then come back and figure out what the best compliments would be.
	//PRINT(picker)
	//PRINT(robot_capabilities)

	auto get_expected=[&](Alliance_capabilities a,Alliance_capabilities b)->double{
		return expected_outcome(skellam_cdf,a,b);
	};

	auto own_capabilities=robot_capabilities[picker];
	//auto other_robots=filter([=](auto p){ return p!=picker; },to_vector(keys(robot_capabilities)));
	auto other_robots=filter([=](auto p){ return p.first!=picker; },robot_capabilities);

	auto m=sorted(mapf(
		[&](auto partner){
			Alliance_capabilities alliance{own_capabilities,partner.second,dead((Robot_capabilities*)nullptr)};
			auto remaining=filter([=](auto const& p){ return p!=partner; },other_robots);
			auto p=unique_pairs(values(remaining));
			auto v=min(mapf(
				[&](auto opponent_pair){
					Alliance_capabilities opponents{
						opponent_pair.first,
						opponent_pair.second,
						dead((Robot_capabilities*)0)
					};
					//return expected_outcome(skellam_cdf,alliance,opponents);
					return get_expected(alliance,opponents);
				},
				p
			));
			//PRINT(v);
			return make_pair(v,partner.first);
		},
		other_robots
	));
	//print_lines(m);
	return reversed(m);
}

int main1(){
	auto x=rand((map<Team,Robot_capabilities>*)nullptr);

	cout<<"Example robots:\n";
	print_lines(x);

	assert(x.size());
	auto picks=make_picklist(begin(x)->first,x);
	print_lines(picks);
	return 0;

	auto scouting_data=read_csv("demo.csv");
	auto useful=interpret(scouting_data);
	//auto a1=take(3,seconds(useful));
	auto [a1,rest]=random_alliance2(seconds(useful));
	auto a2=random_alliance(rest);
	PRINT(a1);
	PRINT(a2);
	PRINT(expected_outcome(Skellam_cdf(),a1,a2));
	return 0;
}

int main(){
	try{
		return main1();
	}catch(const char *s){
		cout<<s<<"\n";
		return 1;
	}catch(string const& s){
		cout<<s<<"\n";
		return 1;
	}
}
