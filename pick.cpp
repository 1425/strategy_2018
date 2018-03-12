#include <fstream>
#include <map>
#include<unistd.h>
#include<sys/wait.h>
#include<iomanip>
#include<functional>
#include "util.h"
#include "sub.h"
#include "util2.h"
#include "decode.h"

//start generic functions

template<typename T>
std::multiset<T> to_multiset(std::vector<T> a){
	return std::multiset<T>{a.begin(),a.end()};
}

template<typename T>
std::set<T> to_set(std::multiset<T> a){
	return std::set<T>{std::begin(a),std::end(a)};
}

template<typename T>
std::ostream& operator<<(std::ostream& o,std::multiset<T> const& a){
	o<<"{";
	for(auto v:to_set(a)){
		o<<"("<<a.count(v)<<","<<v<<")";
	}
	return o<<"}";
}

template<typename T>
T index_or_last(size_t index,std::vector<T> v){
	assert(v.size());
	if(v.size()>index) return v[index];
	return v[v.size()-1];
}

/*template<typename K,typename V>
std::vector<K> firsts(std::map<K,V> m){
	return mapf([](auto a){ return a.first; },m);
}*/

template<typename T>
std::vector<T> take_range(size_t start,size_t length,std::vector<T> in){
	std::vector<T> r;
	for(size_t at=start;at<start+length && at<in.size();at++){
		r|=in[at];
	}
	return r;
}

template<typename T>
std::vector<std::pair<T,T>> nonduplicate_pairs(std::vector<T> a){
	std::vector<std::pair<T,T>> r;
	for(auto i:range(a.size())){
		for(auto j:range(a.size())){
			if(i==j) continue;
			r|=std::make_pair(a[i],a[j]);
		}
	}
	return r;
}

template<typename T>
auto size(std::vector<T> v){ return v.size(); }

using namespace std;

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
		//auto second_picks=take(22,reversed(sorted(values)));
		auto second_picks=reversed(sorted(values));
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

string show_robots(map<Team,Robot_capabilities> const& in){
	auto title1="Team 1425: Estimated robot skills";
	return html(
		head(title(title1))+
		body(
			h1(title1)+
			capabilities_table(in)
		)
	);
}

void check_scouting_data(vector<Scouting_row> const& a){
	#define X(A,B) cout<<""#B<<"("<<""#A<<"): "<<to_multiset(mapf([](auto x){ return x.B; },a))<<"\n";
	SCOUTING_ROW(X)
	#undef X
}

int main1(int argc,char **argv){
	auto scouting_data=read_csv("TMAR.csv");
	//PRINT(scouting_data);
	//nyi
	check_scouting_data(scouting_data);

	auto useful=interpret(scouting_data);
	auto x=useful;
	//auto x=rand((map<Team,Robot_capabilities>*)nullptr);
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
	auto picker=1425;//begin(x)->first;

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
