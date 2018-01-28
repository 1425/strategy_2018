#include "util.h"
#include<fstream>

Int_char::Int_char(int a):i(a){
	assert(a>=-128);
	assert(a<=127);
}

char Int_char::get()const{
	return i;
}

std::ostream& operator<<(std::ostream& o,Int_char a){
	return o<<int(a.get());
}

Int_char Int_char::operator++(int){
	auto r=*this;
	i++;
	return r;
}

Int_char& Int_char::operator+=(int in){
	i+=in;
	return *this;
}

Int_char operator+(Int_char a,unsigned b){
	return a.get()+int(b);
}

Int_char operator-(Int_char a,Int_char b){
	return a.get()-b.get();
}

bool operator==(Int_char a,Int_char b){
	return a.get()==b.get();
}

bool operator!=(Int_char a,Int_char b){
	return !(a==b);
}

bool operator<(Int_char a,Int_char b){
	return a.get()<b.get();
}

Uint_char::Uint_char(){
	//leave uninitialized
}

Uint_char::Uint_char(int a):i(a){
	assert(a>=0);
	assert(a<=255);
}

Uint_char::Uint_char(unsigned a):i(a){
	assert(a<=255);
}

Uint_char::operator unsigned()const{
	return i;
}

unsigned char Uint_char::get()const{
	return i;
}

Uint_char Uint_char::operator++(int){
	auto r=*this;
	i++;
	return r;
}

Uint_char& Uint_char::operator+=(int a){
	//could add bounds checking.
	i+=a;
	return *this;
}

Uint_char& Uint_char::operator-=(int a){
	i-=a;
	return *this;
}

std::ostream& operator<<(std::ostream& o,Uint_char a){
	return o<<unsigned(a);
}

bool operator<(Uint_char a,Uint_char b){
	return a.get()<b.get();
}

bool operator<(Uint_char a,unsigned long b){
	return unsigned(a)<b;
}

bool operator>(Int_char a,int b){
	return a.get()>b;
}

std::vector<bool> bools(){
	return {0,1};
}

void write_file(std::string const& filename,std::string const& contents){
	std::ofstream f(filename);
	f<<contents;
}

