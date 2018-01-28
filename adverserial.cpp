#include<iostream>
#include<sstream>
#include<fstream>
#include "util.h"
#include "placement.h"
#include "state.h"
#include "limits.h"

using namespace std;

//start generic functions

template<typename T>
vector<pair<size_t,T>> enumerate(vector<T> a){
	vector<pair<size_t,T>> r;
	for(auto i:range(a.size())){
		r|=make_pair(i,a[i]);
	}
	return r;
}

string tag(string name,string contents){
	stringstream ss;
	ss<<"<"<<name<<">"<<contents<<"</"<<name<<">";
	return ss.str();
}

string tag(string name,string extra,string contents){
	stringstream ss;
	ss<<"<"<<name<<" "<<extra<<">"<<contents<<"</"<<name<<">";
	return ss.str();
}

string html(string s){ return tag("html",s); }
string body(string s){ return tag("body",s); }
string table(string s){ return tag("table",s); }
string tr(string s){ return tag("tr",s); }
string td(string s){ return tag("td",s); }
string th(string s){ return tag("th",s); }

string join(vector<string> a){
	stringstream ss;
	for(auto elem:a){
		ss<<elem;
	}
	return ss.str();
}

template<typename T>
string as_string(T t){
	stringstream ss;
	ss<<t;
	return ss.str();
}

template<typename T>
vector<T> operator+(vector<T> a,vector<T> b){
	a|=b;
	return a;
}

template<typename A,typename B,typename F,typename Sa,typename Sb>
string build_table(A as,B bs,F f,string xlabel,Sa show_a,string ylabel,Sb show_b){
	return html(body(tag("table","border",
		tr(
			tag("td","colspan=2 rowspan=2","")+
			tag("th","colspan="+as_string(as.size()),xlabel)
		)+
		tr(join(
			//vector<string>{td("")}+
			mapf(
				[&](auto a){ return th(show_a(a)); },
				as
			)
		))+
		join(
			mapf(
				[&](auto b1){
					auto i=b1.first;
					auto b=b1.second;
					return tr(
						[&](){
							if(i==0){
								return tag(
									"th",
									"rowspan="+as_string(bs.size()),
									ylabel
								);
							}
							return string{};
						}()+
						join(
							vector<string>{th(show_b(b))}+
							mapf(
								[&](auto a){ return td(f(a,b)); },
								as
							)
						)
					);
				},
				enumerate(bs)
			)
		)
	)));
}

//start program-specific functions.

#define EXCLUSIVE_POWERUP(X)\
	X(BOOST1)\
	X(BOOST2)\
	X(BOOST3)\
	X(FORCE1)\
	X(FORCE2)\
	X(FORCE3)

//the types of powerups which cannot be run simultaneously.
enum class Exclusive_powerup{
	#define X(A) A,
	EXCLUSIVE_POWERUP(X)
	#undef X
};

ostream& operator<<(ostream& o,Exclusive_powerup a){
	#define X(A) if(a==Exclusive_powerup::A) return o<<""#A;
	EXCLUSIVE_POWERUP(X)
	#undef X
	assert(0);
}

vector<Exclusive_powerup> exclusive_powerups(){
	return {
		#define X(A) Exclusive_powerup::A,
		EXCLUSIVE_POWERUP(X)
		#undef X
	};
}

#define TURN(X)\
	X(Cube_placement,placement)\
	X(optional<Exclusive_powerup>,exclusive_powerup)\
	X(bool,levitate)

struct Turn{
	#define X(A,B) A B;
	TURN(X)
	#undef X
};

ostream& operator<<(ostream& o,Turn const& a){
	o<<"Turn( ";
	#define X(A,B) o<<""#B<<":"<<a.B<<" ";
	TURN(X)
	#undef X
	return o<<")";
}

string abbrev(Cube_placement a){
	stringstream ss;
	#define X(NAME) if(a.NAME){\
		ss<<""#NAME;\
		if(a.NAME!=1){\
			ss<<"("<<a.NAME<<")";\
		}\
	}
	CUBE_PLACEMENTS(X)
	#undef X
	return ss.str();
}

string abbrev(optional<Exclusive_powerup> a){
	if(!a) return "";
	return as_string(*a);
}

string abbrev(Turn a){
	stringstream ss;
	ss<<abbrev(a.placement);
	ss<<abbrev(a.exclusive_powerup);
	if(a.levitate){
		ss<<"levitate";
	}
	return ss.str();
}

vector<Exclusive_powerup> available_exclusive_powerups(State state,Alliance alliance){
	auto p=(alliance==Alliance::RED)?state.red():state.blue();
	auto vault_blocks=(alliance==Alliance::RED)?state.vault_red():state.vault_blue();
	vector<Exclusive_powerup> r;
	if(!p.force()){
		if(vault_blocks>=1) r|=Exclusive_powerup::FORCE1;
		if(vault_blocks>=2) r|=Exclusive_powerup::FORCE2;
		if(vault_blocks>=3) r|=Exclusive_powerup::FORCE3;
	}
	if(!p.boost()){
		if(vault_blocks>=1) r|=Exclusive_powerup::BOOST1;
		if(vault_blocks>=2) r|=Exclusive_powerup::BOOST2;
		if(vault_blocks>=3) r|=Exclusive_powerup::BOOST3;
	}
	return r;
}

bool levitate_allowed(State state,Alliance alliance){
	auto p=(alliance==Alliance::RED)?state.red():state.blue();
	if(p.levitate()) return 0;
	auto vault_blocks=(alliance==Alliance::RED)?state.vault_red():state.vault_blue();
	return vault_blocks>=3;
}

vector<bool> levitate_options(State state,Alliance alliance){
	vector<bool> r;
	r|=false;
	if(levitate_allowed(state,alliance)){
		r|=true;
	}
	return r;
}

vector<Turn> play_options(State state,int max_cubes_to_place,Alliance alliance){
	if(state.time()==0) return {};
	vector<Turn> r;
	for(auto cubes:cube_placements(state,max_cubes_to_place)){
		for(auto levitate:levitate_options(state,alliance)){
			State a=state;
			if(levitate){
				if(alliance==Alliance::RED){
					a.vault_red(a.vault_red()-3);
				}else{
					a.vault_blue(a.vault_blue()-3);
				}
			}
			vector<optional<Exclusive_powerup>> p;
			p|=optional<Exclusive_powerup>();
			for(auto a:available_exclusive_powerups(a,alliance)){
				p|=optional<Exclusive_powerup>(a);
			}
			for(auto powerup:p){
				r|=Turn{cubes,powerup,levitate};
			}
		}
	}
	return r;
}

vector<Turn> red_play_options(State state,int max_cubes_to_place){
	return play_options(state,max_cubes_to_place,Alliance::RED);
}

vector<Turn> blue_play_options(State state,int max_cubes_to_place){
	return play_options(state,max_cubes_to_place,Alliance::BLUE);
}

string get1(Turn,Turn){
	return "item";
}

State apply(State state,Cube_placement red,Cube_placement blue){
	state.sw_red(state.sw_red()+red.red_switch-blue.red_switch);
	state.sw_blue(state.sw_blue()-red.blue_switch+blue.blue_switch);
	state.scale(state.scale()+red.scale-blue.scale);
	state.vault_red(state.vault_red()+red.vault);
	state.vault_blue(state.vault_blue()+blue.vault);
	return state;
}

Powerup_type as_basic(Exclusive_powerup a){
	#define X(A) if(a==Exclusive_powerup::A) return Powerup_type::A;
	EXCLUSIVE_POWERUP(X)
	#undef X
	assert(0);
}

State apply(State state,Turn red,Turn blue){
	/*X(Cube_placement,placement)\
	X(optional<Exclusive_powerup>,exclusive_powerup)\
	X(bool,levitate)*/
	auto s2=apply(state,red.placement,blue.placement);
	if(red.exclusive_powerup){
		nyi
	}
	if(blue.exclusive_powerup){
		/*switch(*blue.exclusive_powerup){
			case BOOST1:
			case BOOST2:
			case BOOST3:
			case 
		nyi*/
		auto r=do_action(
			state,
			Action{make_pair(Alliance::BLUE,as_basic(*blue.exclusive_powerup))}
		);
		state=r.state;
	}
	if(red.levitate)nyi
	if(blue.levitate){
		auto p=state.blue();
		assert(p.levitate()==0);
		p.levitate(1);
		state.blue(p);
	}
	return s2;
}

int main(){
	State state;
	state.time(7);
	state.vault_blue(5);
	auto r=red_play_options(state,1);
	auto b=blue_play_options(state,1);
	PRINT(state);
	cout<<"options:\n";
	print_lines(r);
	cout<<"x:\n";
	print_lines(b);

	//cout<<build_table(r,b,get1);
	write_file(
		"out.html",
		build_table(
			r,b,
			[state](Turn red,Turn blue)->string{
				return as_string(best_case(apply(state,red,blue)));
				return "what";
				//return as_string(best_case(
			},
			"red",
			[](auto a){ return abbrev(a); },
			"blue",
			[](auto a){ return abbrev(a); }
		)
	);

	return 0;
}
