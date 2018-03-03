#include<map>
#include "util.h"
#include "state.h"
#include "limits.h"

using namespace std;

/*
Robot types:
-speed to put up cubes (later)
-where can put (vault only/ v & sw / v, sw & scale)
-climbing ability

Categories of game strategies:
-put stuff on scale
-put stuff on switch(es)
-put stuff in vault
-climb (later)

Robot strategies (ignoring climb):
1) Vault only
2) Switch only
3) Scale only
4) Do two:
	1) vw
	2) vc
	3) wv
	4) wc
	5) cv
	6) cw
	switchover points: time/number placed
5) Do three:
	vwc
	vcw
	...

todo: only account for the differentials between the teams
-instead of saying can place 3 blocks, say that have to place negative values? (letting the other team get it)

if know what they're going to do:
1) put just enough to get each of the things if can get ahead and then go climb, taking whichever can get fastest first

how to deal w/ whole alliance as a single team:
just add all the cube-scoring capabilities together.

simplest way to deal w/ stuff: assume that the other alliance will do nothing for the rest of the match and maximize margin or victory
problems: when the score gets out of reach.

TODO List:
DONE 1) Make it so that can only do powerups when have the right number of blocks.
*/

void tab(int i){
	for(unsigned _:range(i)){
		(void)_;
		cout<<"\t";
	}
}

void explore_inner(int /*i*/,State /*state*/){
	nyi/*for(auto a:allowable_actions(state)){
		auto result=do_action(state,a);
		auto st=result.second;
		tab(i);
		cout<<a<<"("<<result.first<<"): "<<st<<"\n";
		explore_inner(i+1,st);
	}*/
}

void explore(State state){
	cout<<state<<"\n";
	explore_inner(1,state);
}

template<typename T>
void show_dist(multiset<T> a){
	while(a.size()){
		auto v=*begin(a);
		auto c=a.count(v);
		cout<<v<<"\t"<<c<<"\n";
		a.erase(v);
	}
}

int main(){
	//print_lines(all_states());
	//for(auto state:take(100,all_states())){
	multiset<Points> changes;
	for(auto state:all_states()){
		auto b=best_case(state);
		auto c=worst_case(state);

		/*PRINT(state);
		cout<<"\t"<<b<<"\n";
		cout<<"\t"<<c<<"\n";*/
		//explore(state);
		//auto max_change=b.points-c.points;
		auto max_change=b-c;
		//cout<<max_change<<" "<<state<<"\n";
		changes|=max_change;
	}
	show_dist(changes);
	/*State state;
	for(auto action:allowable_actions(state)){
		PRINT(action);
	}*/

	/*
	interesting questions to ask:
	-what is the value of having an extra block in various situations?
	-when I have various situations, what should I do (as whichever alliance)

	bigger questions:
	-given match state, how many pts can earn in different ways assuming that opposint alliance does nothing
	-
	try modeling team and robot actions:
	-each robot can put up 1 block per turn or climb if it is available
	robot capabilities:
		-climb type
		-hanging ability -> initially assume all can do put cubes everywhere
	8 seconds to do whole match @ 10 second intervals

	-use bonuses with fewer items as blockers

	in match program:
	-differentials on the switches/scale
	-current score
	-
	*/
}
