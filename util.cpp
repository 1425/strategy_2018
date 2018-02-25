#include "util.h"
#include<fstream>
#include<sstream>

using namespace std;

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

std::string tag(string name,string contents){
	stringstream ss;
	ss<<"<"<<name<<">"<<contents<<"</"<<name<<">";
	return ss.str();
}

std::string tag(std::string name,string extra,string contents){
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
