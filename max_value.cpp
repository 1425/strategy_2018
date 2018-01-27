#include<iostream>
#include "state.h"
#include "util.h"
#include "limits.h"
#include "placement.h"

using namespace std;

using Points=int;

State place_cubes(State state,Cube_placement pl){
	//assuming that the red team is doing the placing.
	state.sw_red(state.sw_red()+pl.red_switch);
	state.sw_blue(state.sw_blue()+pl.blue_switch);
	state.scale(state.scale()+pl.scale);
	state.vault_red(state.vault_red()+pl.vault);
	return state;
}

Points max_value(State state,unsigned cubes){
	//the maximum number of points that could be forced by having the given number of cubes
	//assume that the opposing alliance does nothing for the rest of the match.  

	auto b1=best_case(state);
	//auto w1=worst_case(state);

	//vector<pair<Result,Result>> all;
	vector<Points> all;

	//iterate through all of the possible ways to place them and then see what that does to the max and min outcomes
	for(auto p:cube_placements(state,cubes)){
		auto after=place_cubes(state,p);
		//PRINT(after);
		//PRINT(best_case(after));
		//PRINT(worst_case(after));
		auto b=best_case(after);
		//auto w=worst_case(after);
		auto d_b=b-b1;
		//auto d_w=w-w1;
		//all|=make_pair(d_b,d_w);
		all|=d_b;

		/*if(d_b!=d_w){
			cout<<"\ndb1:\n";
			cout<<d_b<<"\n";
			cout<<d_w<<"\n";// "<<after<<"\n";
		}*/

		//obviously the worst case scenario inclues pushing the boost button at the wrong time.

		/*if(d_b.points<0 || d_w.points<-10){
			PRINT(state);
			PRINT(cubes);
			PRINT(p);
			PRINT(after);
			PRINT(b1);
			PRINT(b);
			PRINT(w1);
			PRINT(w);
		}*/

		//assert(d_b.points>=0);
		assert(d_b>=0);
		//assert(d_w.points>=-10);
	}
	//cout<<"Found:\n";
	//print_lines(all);

	//cout<<"points:\n";
	//auto m=mapf([](auto a){ return make_pair(a.first.points,a.second.points); },all);
	//print_lines(m);

	//why are there places where the worst case gets worse? -> indicates a bug.

	//arbitrarily choosing the best case scenario.
	//return max(firsts(all)).points;
	//return max(all).points;
	return max(all);
}

template<typename T>
T median(vector<T> v){
	assert(!v.empty());
	sort(begin(v),end(v));
	return v[v.size()/2];
}

int main(){
	//state_test();
	//nyi
	/*for(auto state:all_states()){
		for(auto cubes:range(1,2)){
			cout<<state<<"\t"<<max_value(state,cubes)<<"\n";
		}
	}*/
	for(auto time:times()){
		PRINT(time);
		for(auto cubes:range(1,4)){
			//PRINT(cubes);
			cout<<"\tcubes:"<<cubes;
			auto value=mapf(
				[&](auto state){
					return max_value(state,cubes);
				},
				states_at_time(time)
			);
			cout<<"\t"<<min(value)<<"\t"<<median(value)<<"\t"<<max(value)<<"\n";
		}
	}
}
