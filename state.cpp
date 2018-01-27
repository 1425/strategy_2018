#include "state.h"
#include<iostream>
#include "util.h"

using namespace std;

Powerups::Powerups(bool a,bool b,bool c):
	data( (a<<2)+(b<<1)+c )
{}

bool Powerups::force()const{ return 1&(data>>2); }
bool Powerups::boost()const{ return 1&(data>>1); }
bool Powerups::levitate()const{ return 1&data; }

void Powerups::force(bool a){ data=(data&0x3)|(a<<2); }
void Powerups::boost(bool a){ data=(data&0x5)|(a<<1); }
void Powerups::levitate(bool a){ data=(data&0x6)|a; }

ostream& operator<<(ostream& o,Powerups const& a){
	return o<<"Powerups(force:"<<a.force()<<" boost:"<<a.boost()<<" levitate:"<<a.levitate()<<")";
}

#define CMP(A) if(a.A<b.A) return 1; if(b.A<a.A) return 0;

bool operator<(Powerups a,Powerups b){
	CMP(force())
	CMP(boost())
	CMP(levitate())
	return 0;
}

bool operator==(Powerups a,Powerups b){
	return a.force()==b.force() && a.boost()==b.boost() && a.levitate()==b.levitate();
}

bool operator!=(Powerups a,Powerups b){
	return !(a==b);
}

Powerups operator-(Powerups a,Powerups b){
	return Powerups{
		a.force()-b.force(),
		a.boost()-b.boost(),
		a.levitate()-b.levitate()
	};
}

vector<Powerups> powerups(){
	vector<Powerups> r;
	for(auto a:bools()){
		for(auto b:bools()){
			for(auto c:bools()){
				r|=Powerups{a,b,c};
			}
		}
	}
	return r;
}

//using Time=unsigned char;

/*vector<Time> times(){
	//return {0};
	return range(Time(7));
	//return range(Time(13));
}*/
array<Time,7> times(){
	return range_st2<Time,7>();
}

ostream& operator<<(ostream& o,Alliance a){
	#define X(A) if(a==Alliance::A) return o<<""#A;
	ALLIANCES(X)
	#undef X
	assert(0);
}

array<Alliance,2> alliances(){
	return {Alliance::RED,Alliance::BLUE};
}

vector<Owner> owners(){
	vector<Owner> r;
	#define X(A) r|=Owner::A;
	OWNER(X)
	#undef X
	return r;
}

ostream& operator<<(ostream& o,Owner a){
	nyi
}

bool operator==(Owner a,Alliance b){
	return (a==Owner::RED && b==Alliance::RED) || (a==Owner::BLUE && b==Alliance::BLUE);
}

using Blocks=unsigned char;

//State::State():a(0),b(0),c(0),d(0){}
State::State():d(0),c(0),b(0),a(0){}

State::State(
	#define X(A,B) A B,
	STATE_ITEMS(X)
	#undef X
	bool ignored
):
	d( (vault_red<<4)+vault_blue ),
	c(
		(red.force()<<6)+
		(red.boost()<<5)+
		(red.levitate()<<4)+
		(blue.force()<<2)+
		(blue.boost()<<1)+
		blue.levitate()
	),
	b( (sw_blue.get()<<4)+(scale.get()&0xf) ),
	a( (time<<4)+(sw_red.get()&0xf) )
{}

Time State::time()const{
	return a>>4;
}

void State::time(Time x){
	a=(a&0xf)+(x<<4);
}

Cube_differential State::sw_red()const{
	auto v=a&0xf;
	if(v<8) return Cube_differential(v);
	return Cube_differential(v-16);
}

void State::sw_red(Cube_differential x){
	a=(a&0xf0)+(unsigned(x.get())&0xf);
}

Cube_differential State::sw_blue()const{
	auto v=(b>>4);
	if(v<8) return Cube_differential(v);
	return Cube_differential(v-16);
}

void State::sw_blue(Cube_differential x){
	b=(b&0xf)+unsigned(x.get()<<4);
}

Cube_differential State::scale()const{
	auto v=(b&0xf);
	if(v<8) return Cube_differential(v);
	return Cube_differential(v-16);
}

void State::scale(Cube_differential x){
	b=(b&0xf0)+unsigned(x.get()&0xf);
}

Vault_blocks State::vault_red()const{
	return d>>4;
}

void State::vault_red(Vault_blocks x){
	d=(d&0xf)+(x<<4);
}

Vault_blocks State::vault_blue()const{
	 return d&0xf;
}

void State::vault_blue(Vault_blocks x){
	d=(d&0xf0)+x;
}

Powerups State::red()const{
	return Powerups(
		(c>>6)&1,
		(c>>5)&1,
		(c>>4)&1
	);
}

void State::red(Powerups x){
	c=(c&0xf)+(x.force()<<6)+(x.boost()<<5)+(x.levitate()<<4);
}

Powerups State::blue()const{
	return Powerups{
		(c>>2)&1,
		(c>>1)&1,
		c&1
	};
}

void State::blue(Powerups x){
	c=(c&0xf0)+(x.force()<<2)+(x.boost()<<1)+x.levitate();
}

vector<int> examples(int*)nyi

vector<Int_char> examples(Int_char*){
	return range(Int_char{-8},Int_char{8});
}

vector<Uint_char> examples(Uint_char*){
	return range(Uint_char{16});
}

vector<Powerups> examples(Powerups*){
	return powerups();
}

void state_test(){
	auto all=all_states();
	set<State> r{begin(all),end(all)};
	assert(r.size()==all.size());

	for(auto a:all){
		#define X(A,B) for(auto e:examples((A*)0)){\
			auto orig=a;\
			auto prev=a.B();\
			a.B(e);\
			if(a.B()!=e){\
				PRINT(a.B());\
				PRINT(e);\
			}\
			assert(a.B()==e);\
			a.B(prev);\
			if(a!=orig){\
				cout<<""#B<<":"<<"\n";\
				PRINT(a);\
				PRINT(orig);\
			}\
			assert(a==orig);\
		}
		STATE_ITEMS(X)
		#undef X
	}
}

ostream& operator<<(ostream& o,State const& a){
	o<<"State(";
	#define X(A,B) o<<" "#B<<":"<<a.B();
	STATE_ITEMS(X)
	#undef X
	return o<<")";
}

bool operator<(State a,State b){
	assert(sizeof(a)==4);
	return (*(int32_t*)&a)<(*(int32_t*)&b);

	//assert(sizeof(a)==8);
	nyi//return (*(long long*)&a)<(*(long long*)&b);
	/*#define X(A,B) CMP(B)
	STATE_ITEMS(X)
	#undef X
	return 0;*/
}

State operator-(State a,State b){
	return State{
		#define X(A,B) a.B()-b.B(),
		STATE_ITEMS(X)
		#undef X
	};
}

bool operator==(State a,State b){
	//assert(sizeof(a)==8);
	//nyi//    return (*(long long*)&a)==(*(long long*)&b);//assume all the padding bits are equal.
	assert(sizeof(a)==4);
	return (*(uint32_t*)&a)==(*(uint32_t*)&b);
}

bool operator!=(State a,State b){
	return !(a==b);
}

/*namespace std{
	template<> struct hash<State>{
		long long operator()(State a)const noexcept{
			assert(sizeof(State)==8);
			return (*(long long*)&a);
		}
	};
}*/

namespace std{
	long long hash<State>::operator()(State a)const noexcept{
		/*assert(sizeof(State)==8);
		return (*(long long*)&a);*/
		assert(sizeof(State)==4);
		return *(int32_t*)&a;
	}
}

vector<Cube_differential> cube_differentials(){
	//return range<Cube_differential>(-3,4);
	//return range<Cube_differential>(-1,2);
	return range<Cube_differential>(-2,3);
}

vector<State> all_states(){
	vector<State> r;
	for(auto t:times()){
		for(auto red:powerups()){
			for(auto blue:powerups()){
				for(auto red_switch:cube_differentials()){
					for(auto blue_switch:cube_differentials()){
						for(auto scale:cube_differentials()){
							for(auto blocks_red:range_st<Blocks,10>()){
								for(auto blocks_blue:range_st<Blocks,10>()){
									r|=State{t,red_switch,blue_switch,scale,red,blue,blocks_red,blocks_blue};
								}
							}
						}
					}
				}
			}
		}
	}
	return r;
}

vector<State> states_at_time(Time time){
	//this is the stupid slow way.
	return filter(
		[time](auto s){ return s.time()==time; },
		all_states()
	);
}
