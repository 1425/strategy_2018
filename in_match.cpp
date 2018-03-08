#include "decode.h"
#include<functional>

using namespace std;

//start generic functions

auto h2(string s){ return tag("h2",s); }

template<typename T,size_t N>
std::vector<T> to_vector(std::array<T,N> a){
	std::vector<T> r;
	for(auto elem:a) r|=elem;
	return r;
}

template<typename T,size_t N>
std::vector<T> take(size_t x,std::array<T,N> in){
	vector<T> r;
	for(auto i:range(x)){
		r|=in[i];
	}
	return r;
}

template<typename T,size_t N,size_t N2>
std::array<T,N-N2> skip(std::array<T,N> in){
	std::array<T,N-N2> r;
	for(auto [out_i,in_i]:enumerate(range(N2,N))){
		r[out_i]=in[in_i];
	}
	return r;
}

template<typename A,typename B,size_t N>
std::array<A,N> firsts(std::array<pair<A,B>,N> in){
	std::array<A,N> r;
	for(auto p:enumerate(in)){
		r[p.first]=p.second.first;
	}
	return r;
}

//start program-speicifc functions

string climb_display(std::array<std::pair<Team,Robot_capabilities>,3> in){
	const auto teams=firsts(in);

	map<string,std::function<double(Alliance_climb)>> options{
		{"no_climb",no_climb},
		{"single_climb",single_climb},
		{"bar1",bar1},
		{"bar2",bar2},
		{"lift1",lift1},
		{"lift2",lift2},
		{"all1",all1},
		{"all2",all2}
	};

	using Climb_option=string;
	using Climb_strategy=pair<Climb_option,array<Team,3>>;
	vector<Climb_strategy> possible_strategies;
	for(auto opt:options){
		for(auto assignments:permutations(teams)){
			possible_strategies|=make_pair(opt.first,assignments);
		}
	}

	auto get_abilities=[&](Team team){
		for(auto p:in){
			if(p.first==team){
				return p.second;
			}
		}
		assert(0);
	};

	auto value=[&](Climb_strategy p)->double{
		auto climb_abilities=mapf([&](auto team){ return get_abilities(team).climb; },p.second);
		if(p.first=="no_climb") return 15;
		if(p.first=="single_climb") return 30*climb_abilities[0].itself+10;
		if(p.first=="bar1") return 30*climb_abilities[0].bar1*(1+climb_abilities[1].itself)+5;
		if(p.first=="bar2") return 30*climb_abilities[0].bar2*(1+climb_abilities[1].itself+climb_abilities[2].itself);
		if(p.first=="lift1") return 30*climb_abilities[0].lift1*climb_abilities[1].drives;
		if(p.first=="lift2") return 30*climb_abilities[0].lift2*(climb_abilities[1].drives+climb_abilities[2].drives);
		if(p.first=="all1") return 30*climb_abilities[0].all1*(1+climb_abilities[1].drives)+5;
		if(p.first=="all2") return 30*climb_abilities[0].all2*(1+climb_abilities[1].drives+climb_abilities[2].drives);
		nyi
	};

	auto t=take(5,reversed(sorted(mapf(
		[&](auto a){
			return make_pair(value(a),a);
		},
		possible_strategies
	))));

	return tag("table","border",
		tr(join(mapf(th,vector<string>{"Expected Value","Action","Assignment 1","Assignment 2","Assignment 3"})))+
		join(mapf(
			[](auto a){
				return tr(
					td(as_string(a.first))+
					td(as_string(a.second.first))+
					td(as_string(a.second.second[0]))+
					td(as_string(a.second.second[1]))+
					td(as_string(a.second.second[2]))
				);
			},
			t
		))
	);
}

using Item=std::pair<Team,Robot_capabilities>;
using Match_robots=std::array<std::pair<Team,Robot_capabilities>,6>;

auto alliances(Match_robots in){
	return make_pair(
		as_array<Item,3>(take(3,in)),
		skip<Item,6,3>(in)
	);
}

string climb_display(std::array<std::pair<Team,Robot_capabilities>,6> in){
	auto a=climb_display(
		as_array<Item,3>(take(3,in))
	);
	auto b=climb_display(skip<Item,6,3>(in));
	return table(tr(td(a)+td(b)));
}

double total_switch(std::array<pair<Team,Robot_capabilities>,3> in){
	return sum(mapf([](auto a){ return mean_or_0(a.second.cubes); },in)).switch_cubes;
}

double total_scale(std::array<pair<Team,Robot_capabilities>,3> in){
	return sum(mapf([](auto a){ return mean_or_0(a.second.cubes); },in)).scale_cubes;
}

string teleop_display(std::array<std::pair<Team,Robot_capabilities>,6> in){
	auto a=alliances(in);
	auto scale=total_scale(a.first)-total_scale(a.second);
	auto switches=total_switch(a.first)-total_switch(a.second);

	auto advice=[&](){
		if(scale>0){
			if(switches>0){
				if(scale>switches){
					return "1) Scale 2) Switches 3) Vault";
				}
				return "1) Switches 2) Scale 3) Vault";
			}
			return "1) Scale 2) Vault";
		}
		if(switches>0){
			return "1) Switches 2) Vault";
		}
		if(scale>switches){
			return "1) Scale 2) Vault";
		}
		return "1) Switches 2) Vault";
	}();
	stringstream ss;
	ss<<"scale differential:"<<scale<<"<br>";
	ss<<"switch differential:"<<switches<<"<br>";
	ss<<"Suggested strategy:"<<advice;
	return ss.str();
}

string match_strategy(std::array<std::pair<Team,Robot_capabilities>,6> in){
	auto title1="Team 1425 Strategy Suggestions";
	return html(
		head(title(title1))+
		body(
			h1(title1)+
			capabilities_table(to_vector(in))+
			h2("Teleop")+teleop_display(in)+
			h2("Endgame")+climb_display(in)
		)
	);
}

int main(int argc,char **argv){
	auto a=args(argc,argv);
	auto m=mapf(
		[](auto elem){
			auto i=atoi(elem);
			assert(i>0);
			return Team{unsigned(i)};
		},
		a
	);

	array<Team,6> teams;
	if(m.size()==5){
		//assume 1425 for first arg if only 5 args
		teams[0]=1425;
		for(unsigned i:range(1,6)){
			teams[i]=m[i-1];
		}
	}else if(m.size()==6){
		teams=as_array<Team,6>(m);
	}else{
		cout<<"usage:\n";
		cout<<"\tmatch_strat ALLIE_1 ALLIE_2 ALLIE_3 OPPONENT_1 OPPONENT_2 OPPONENT_3\n";
		cout<<"\tmatch_strat ALLIE_1 ALLIE_2 OPPONENT_1 OPPONENT_2 OPPONENT_3\n";
		return 1;
	}

	auto x=rand((map<Team,Robot_capabilities>*)nullptr);
	auto s=match_strategy(mapf(
		[&](auto team){
			auto f=x.find(team);
			if(f==x.end()){
				cout<<"Error: No data about team: "<<team<<"\n";
				cout<<"Known teams:"<<firsts(x)<<"\n";
				exit(1);
			}
			return make_pair(f->first,f->second);
		},
		teams
	));
	write_file("match_strategy.html",s);
	return 0;
}
