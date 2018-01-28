#include "limits.h"
#include<map>
#include<unordered_map>
#include<strings.h>

using namespace std;

Result operator-(Result a,Result b){
	return Result{
		#define X(A,B) a.B-b.B,
		RESULT(X)
		#undef X
	};
}

bool operator<(Result a,Result b){
	#define X(A,B) if(a.B<b.B) return 1; if(b.B<a.B) return 0;
	RESULT(X)
	#undef X
	return 0;
}

bool operator==(Result a,Result b){
	#define X(A,B) if(a.B!=b.B) return 0;
	RESULT(X)
	#undef X
	return 1;
}

bool operator!=(Result a,Result b){
	return !(a==b);
}

ostream& operator<<(ostream& o,Result a){
	o<<"Result( ";
	#define X(A,B) o<<""#B<<":"<<a.B<<" ";
	RESULT(X)
	#undef X
	return o<<")";
}

ostream& operator<<(ostream& o,Powerup_type a){
	#define X(A) if(a==Powerup_type::A) return o<<""#A;
	POWERUP_TYPES(X)
	#undef X
	assert(0);
}

vector<Powerup_type> powerup_types(){
	return {
		#define X(A) Powerup_type::A,
		POWERUP_TYPES(X)
		#undef X
	};
}

vector_limited<Action,15> all_actions(){
	vector_limited<Action,15> r;
	r|=Action{};//do nothing
	for(auto a:alliances()){
		for(auto b:powerup_types()){
			r|=Action{make_pair(a,b)};
		}
	}
	return r;
}

unsigned cubes_needed(Powerup_type a){
	switch(a){
		case Powerup_type::BOOST1: return 1;
		case Powerup_type::BOOST2: return 2;
		case Powerup_type::BOOST3: return 3;
		case Powerup_type::FORCE1: return 1;
		case Powerup_type::FORCE2: return 2;
		case Powerup_type::FORCE3: return 3;
		case Powerup_type::LEVITATE: return 3;
		default: assert(0);
	}
}

bool allowed(State state,Action action){
	if(state.time()==0) return 0;
	if(!action) return 1;
	auto b=(action->first==Alliance::RED)?state.red():state.blue();
	auto blocks=(action->first==Alliance::RED)?state.vault_red():state.vault_blue();
	auto blocks_needed=cubes_needed(action->second);
	if(blocks_needed>blocks){
		return 0;
	}
	switch(action->second){
		case Powerup_type::BOOST1:
		case Powerup_type::BOOST2:
		case Powerup_type::BOOST3:
			return !b.boost();
		case Powerup_type::FORCE1:
		case Powerup_type::FORCE2:
		case Powerup_type::FORCE3:
			return !b.force();
		case Powerup_type::LEVITATE:
			return !b.levitate();
		default:
			assert(0);
	}
}

//vector<Action> allowable_actions(State state){
auto allowable_actions(State state){
	return filter(
		[state](Action action){ return allowed(state,action); },
		all_actions()
	);
}

vector<State> descendants(State a){
	//direct descendants only

	vector<State> r;
	if(a.time()==0) return r;
	#define X(NAME) if(!a.NAME()){\
		State b=a;\
		b.time(b.time()-1);\
		b.NAME(1);\
		r|=b;\
	}
	X(red().boost)
	X(red().force)
	X(red().levitate)
	X(blue().boost)
	X(blue().force)
	X(blue().levitate)
	#undef X
	return r;
}

tuple<int,int,int,int> effective_weights(Action a){
	//points per second for each of: red switch,scale owner,blue switch,independent of switch/scale settings
	if(!a){
		return make_tuple(1,1,-1,0);
	}
	if(a->first==Alliance::RED){
		switch(a->second){
			case Powerup_type::BOOST1:return make_tuple(2,1,-1,0);
			case Powerup_type::BOOST2:return make_tuple(1,2,-1,0);
			case Powerup_type::BOOST3:return make_tuple(2,2,-1,0);
			case Powerup_type::FORCE1:return make_tuple(0,1,-1,1);
			case Powerup_type::FORCE2:return make_tuple(1,0,-1,1);
			case Powerup_type::FORCE3:return make_tuple(0,0,-1,2);
			case Powerup_type::LEVITATE:return make_tuple(1,1,-1,3);//note use of time-based item even though instant for climb.
			default: assert(0);
		}
	}
	if(a->first==Alliance::BLUE){
		switch(a->second){
			case Powerup_type::BOOST1:return make_tuple(1,1,-2,0);
			case Powerup_type::BOOST2:return make_tuple(1,2,-1,0);
			case Powerup_type::BOOST3:return make_tuple(1,2,-2,0);
			case Powerup_type::FORCE1:return make_tuple(1,1,0,-1);
			case Powerup_type::FORCE2:return make_tuple(1,0,-1,-1);
			case Powerup_type::FORCE3:return make_tuple(1,0,0,-2);
			case Powerup_type::LEVITATE: return make_tuple(1,1,-1,-3); //note use of time-based item even though instant for climb.
			default: assert(0);
		}
	}
	assert(0);
}

Result do_action(State state,Action action){
	if(!allowed(state,action)){
		PRINT(state);
		PRINT(action);
	}
	assert(allowed(state,action));

	//state.time-=1;
	state.time(state.time()-1);

	Points vault_cube_points=0;

	if(action){
		auto a=*action;
		auto powerups=(a.first==Alliance::RED)?state.red():state.blue();

		{
			auto vault_cubes=(a.first==Alliance::RED)?state.vault_red():state.vault_blue();
			auto used=cubes_needed(a.second);
			vault_cube_points=used*5;	
			vault_cubes-=used;
			if(a.first==Alliance::RED){
				state.vault_red(vault_cubes);
			}else{
				state.vault_blue(vault_cubes);
			}
		}

		switch(a.second){
			case Powerup_type::BOOST1:
			case Powerup_type::BOOST2:
			case Powerup_type::BOOST3:
				powerups.boost(1);
				break;
			case Powerup_type::FORCE1:
			case Powerup_type::FORCE2:
			case Powerup_type::FORCE3:
				powerups.force(1);
				break;
			case Powerup_type::LEVITATE:
				powerups.levitate(1);
				break;
			default:
				assert(0);
		}
		if(a.first==Alliance::RED){
			state.red(powerups);
		}else{
			state.blue(powerups);
		}
	}
	auto e=effective_weights(action);
	Points pts=vault_cube_points+
		(state.sw_red()>0)*10*get<0>(e)+
		sgn(state.scale())*10*get<1>(e)+
		(state.sw_blue()<0)*10*get<2>(e)+
		10*get<3>(e);
	return Result{pts,state};
}

Points best_case(State);

Points best_case_inner(State state){
	//the best case scenario for the red team
	//TODO: Memoize
	/*auto d=descendants(a);
	auto m=mapf(best_case,d);
	if(m.empty()){
		return make_pair(0,a);
	}
	return max(m);*/
	auto actions=allowable_actions(state);
	if(actions.empty()){
		assert(state.time()==0);
		//return Result{0,state};
		return 0;
	}
	return max(mapf(
		[state](Action action)->Points{
			auto result=do_action(state,action);
			auto b=best_case(result.state);
			//TODO: Will eventually want the path.
			//return Result{result.points+b.points,b.state};
			return result.points+b;
		},
		actions
	));
}

Points best_case1(State state){
	//memoize.  Not thread safe!
	static unordered_map<State,Points> cache;
	auto f=cache.find(state);
	if(f!=cache.end()){
		return f->second;
	}
	//return cache[state]=best_case_inner(state);
	auto r=best_case_inner(state);
	cache.insert(make_pair(state,r));
	return r;
}

Points best_case2(State state){
	//this is slower than the usual way.
	static array<unordered_map<State,Points>,13> cache;
	auto &c=cache[state.time()];
	auto f=c.find(state);
	if(f!=c.end()){
		return f->second;
	}
	auto r=best_case_inner(state);
	c.insert(make_pair(state,r));
	return r;
}

class M_pt{
	static const int SCALE=5;
	unsigned char data;

	public:
	M_pt():data(0){}

	bool valid()const{ return !!(data&0x80); }

	void set(int i){
		assert(!valid());

		assert(i/SCALE*SCALE==i);
		/*if(i%SCALE){
			PRINT(i);
			PRINT(i%SCALE);
		}
		assert( (i%SCALE)==0 );*/
		int a=i/SCALE;
		assert(a>=-32);
		assert(a<32);
		auto v0=[&]()->int{
			if(a>=0) return a;
			return a+64;
			//return (unsigned char)(a)&0x7;
		}();//(a&0x7);
		data=0x80 | v0;
		//PRINT(int(data));
		if(get()!=i){
			PRINT(i);
			PRINT(a);
			PRINT(v0);
			PRINT(int(data));
			PRINT(get());
		}
		assert(get()==i);
	}

	int get()const{
		assert(valid());
		//PRINT(int(data));
		//auto v=int(data)&0x7;
		auto v=data-128;
		//PRINT(v);
		auto unscaled=[&]()->int{
			if(v<32){
				return v;
			}
			return int(v)-64;
		}();
		return SCALE*unscaled;
	}
};

Points best_case(State state){
	static M_pt *cache=NULL;
	static const unsigned SIZE=2u*1024*1024*1024;//2GB;
	if(!cache){
		cache=new M_pt[SIZE];
		assert(cache);
	}
	auto index=*(uint32_t*)&state;
	if(index>=SIZE){
		PRINT(state);
		cout<<hex<<index<<"\n";
	}
	assert(index<SIZE);
	auto &v=cache[index];
	if(v.valid()){
		return v.get();
	}
	auto value=best_case_inner(state);
	v.set(value);
	assert(v.valid());
	assert(v.get()==value);
	return value;
}

Points best_case3(State state){
	//static array<pair<bool,Points>,1ull<<32> data;
	static unsigned char *cache=NULL;
	static const unsigned SIZE=2u*1024*1024*1024;//2GB;
	if(!cache){
		cache=new unsigned char[SIZE];
		assert(cache);
		bzero(cache,SIZE);
	}

	assert(sizeof(State)==4);
	auto index=*(uint32_t*)&state;
	if(index>=SIZE){
		PRINT(state.time());
		PRINT(state);
		cout<<"got:\n";
		cout<<hex<<index<<"\n";
		cout<<SIZE<<"\n";
	}
	assert(index<SIZE);
	auto &v=cache[index];
	if(v&0x80){
		auto a=v&0x7;
		//a ranges 0-64
		auto w_sign=[&](){
			if(a<32) return a;
			return int(a)-64;
		}();
		return 10*w_sign;
	}

	auto value=best_case_inner(state);
	assert( (value%10)==0 );
	auto v1=value/10;
	assert(v1>=-32);
	assert(v1<32);
	
	v=0x80|(v1&0x7);
	PRINT(v);
	return value;
}

Points worst_case(State state);

Points worst_case_inner(State state){
	auto actions=allowable_actions(state);
	if(actions.empty()){
		//return Result{0,state};
		return 0;
	}
	return min(mapf(
		[state](Action action)->Points{
			auto result=do_action(state,action);
			auto b=worst_case(result.state);
			//return Result{result.points+b.points,b.state};
			return result.points+b;
		},
		actions
	));
}

Points worst_case(State state){
	//memoize.  Not thread safe!
	static map<State,Points> cache;
	auto f=cache.find(state);
	if(f!=cache.end()){
		return f->second;
	}
	//return cache[state]=worst_case_inner(state);
	auto r=worst_case_inner(state);
	cache.insert(make_pair(state,r));
	return r;
}

