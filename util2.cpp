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
