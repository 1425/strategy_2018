#include "util.h"

template<typename T>
std::vector<T> without(T t,std::vector<T> in){
	return filter([t](auto x){ return x!=t; },in);
}

template<typename Strategy,typename Value>
std::vector<std::pair<Strategy,Strategy>> find_nash_equilibriums(Value value,std::vector<Strategy> t1,std::vector<Strategy> t2){
	std::vector<std::pair<Strategy,Strategy>> r;

	//assume zero-sum game in which t1 is attempting to maximize value() and t2 is trying to minimize it.

	for(auto p1:t1){
		auto t1_other=without(p1,t1);
		for(auto p2:t2){
			auto t2_other=without(p2,t2);
			auto here=value(p1,p2);

			//see if there is a way for player 1 to improve their outcome
			auto better_for_p1=filter([&](auto p1x){ return value(p1x,p2)>here; },t1_other);
			if(better_for_p1.size()){
				continue;
			}

			auto better_for_p2=filter([&](auto p2x){ return value(p1,p2x)<here; },t2_other);
			if(better_for_p2.size()){
				continue;
			}

			r|=std::make_pair(p1,p2);
		}
	}
	return r;
}

using namespace std;

int main(){
	using Strategy=int;

	vector<Strategy> alliance1;
	vector<Strategy> alliance2;
	
	auto value=[&](Strategy a,Strategy b){
		return a*b;
	};
	auto f=find_nash_equilibriums(value,range(0,10),range(-5,10));
	PRINT(f.size());
	print_lines(f);
	return 0;
}
