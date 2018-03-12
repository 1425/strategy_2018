#include "util2.h"

using namespace std;

unsigned rand(const unsigned*){
	return rand()%10;
}

Px rand(const Px*){
	return Px{(rand()%101)/100.0};
}

Px& operator+=(Px& a,Px b){
	a.x+=b.x;
	return a;
}

Px& operator/=(Px& a,unsigned long b){
	a.x/=b;
	return a;;
}

Px dead(const Px*){ return Px{0}; }

Px::operator double()const{ return x; }

bool nonblank(string s){
	for(auto c:s){
		if(!isblank(c)){
			return 1;
		}
	}
	return 0;
}

vector<string> split(string haystack,char needle){
	vector<string> r;
	stringstream ss;
	for(auto c:haystack){
		if(c==needle){
			r|=ss.str();
			ss.str("");
		}else{
			ss<<c;
		}
	}
	if(ss.str().size() && nonblank(ss.str())){
		r|=ss.str();
	}
	return r;
}
