#ifndef STATE_H
#define STATE_H

#include<iosfwd>
#include<vector>
#include<array>
#include "util.h"

class Powerups{
	unsigned char data;

	public:
	Powerups(bool,bool,bool);

	bool force()const;
	bool boost()const;
	bool levitate()const;

	void force(bool);
	void boost(bool);
	void levitate(bool);
};

std::ostream& operator<<(std::ostream& o,Powerups const&);
bool operator==(Powerups,Powerups);
bool operator!=(Powerups,Powerups);

#define CMP(A) if(a.A<b.A) return 1; if(b.A<a.A) return 0;

bool operator<(Powerups,Powerups);
Powerups operator-(Powerups,Powerups);

std::vector<Powerups> powerups();

using Time=Uint_char;

std::array<Time,7> times();

#define ALLIANCES(X) X(RED) X(BLUE)

enum class Alliance{
	#define X(NAME) NAME,
	ALLIANCES(X)
	#undef X
};

std::ostream& operator<<(std::ostream&,Alliance);

std::array<Alliance,2> alliances();

#define OWNER(X) X(RED) X(BLUE) X(NONE)

enum class Owner:unsigned char{
	#define X(A) A,
	OWNER(X)
	#undef X
};

std::vector<Owner> owners();

std::ostream& operator<<(std::ostream&,Owner);

bool operator==(Owner,Alliance);

using Vault_blocks=Uint_char;

using Cube_differential=Int_char;

#define STATE_ITEMS(X)\
	X(Time,time)\
	X(Cube_differential,sw_red)\
	X(Cube_differential,sw_blue)\
	X(Cube_differential,scale)\
	X(Powerups,red)\
	X(Powerups,blue)\
	X(Vault_blocks,vault_red)\
	X(Vault_blocks,vault_blue)

/*struct State{
	Time time=Time{0};//0-12; at least 4 bits
	Cube_differential sw_red=0,sw_blue=0;//number in favor of the red alliance
	Cube_differential scale=0;//at least 4 bits?
	Powerups red,blue;//6 bits total
	Vault_blocks vault_red=0,vault_blue=0;//the number of blocks that have not already been committed. at least 4 bits each
};*/

class State{
	//unsigned char a,b,c,d;
	unsigned char d,c,b,a;//note: taking advantage of the top bit not being used to save memory.

	public:
	State();

	State(
		#define X(A,B) A B,
		STATE_ITEMS(X)
		#undef X
		bool ignored=0 //not used
	);

	Time time()const;
	void time(Time);

	Cube_differential sw_red()const;
	void sw_red(Cube_differential);

	Cube_differential sw_blue()const;
	void sw_blue(Cube_differential);

	Cube_differential scale()const;
	void scale(Cube_differential);

	Powerups red()const;
	void red(Powerups);

	Powerups blue()const;
	void blue(Powerups);

	Vault_blocks vault_red()const;
	void vault_red(Vault_blocks);

	Vault_blocks vault_blue()const;
	void vault_blue(Vault_blocks);
};

std::ostream& operator<<(std::ostream&,State const&);
bool operator<(State,State);
bool operator==(State,State);
bool operator!=(State,State);
State operator-(State,State);
void state_test();

namespace std{
	template<> struct hash<State>{
		long long operator()(State a)const noexcept;
	};
}

std::vector<State> all_states();
std::vector<State> states_at_time(Time);

#endif
