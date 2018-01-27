#ifndef UTIL_H
#define UTIL_H

#include<set>
#include<iostream>
#include<vector>
#include<algorithm>
#include<cassert>

#define nyi { std::cout<<"nyi "<<__FILE__<<":"<<__LINE__<<"\n"; exit(44); }

#define PRINT(X) { std::cout<<""#X<<":"<<(X)<<"\n"; }

template<typename T>
std::multiset<T>& operator|=(std::multiset<T> &a,T t){
	a.insert(t);
	return a;
}

template<typename A,typename B,typename C>
std::ostream& operator<<(std::ostream& o,std::tuple<A,B,C> const& a){
	return o<<"("<<std::get<0>(a)<<","<<std::get<1>(a)<<","<<std::get<2>(a)<<")";
}

template<typename A,typename B,typename C,typename D>
std::ostream& operator<<(std::ostream& o,std::tuple<A,B,C,D> const& a){
	o<<"("<<std::get<0>(a)<<","<<std::get<1>(a)<<",";
	return o<<std::get<2>(a)<<","<<std::get<3>(a)<<")";
}

template<typename A,typename B>
std::ostream& operator<<(std::ostream& o,std::pair<A,B> const& a){
	return o<<"("<<a.first<<","<<a.second<<")";
}

template<typename T>
std::vector<T>& operator|=(std::vector<T> &a,T t){
	a.push_back(t);
	return a;
}

template<typename T,typename T2>
std::vector<T>& operator|=(std::vector<T> &a,std::vector<T2> b){
	for(auto elem:b){
		a|=elem;
	}
	return a;
}

template<typename T>
void print_lines(T t){
	for(auto a:t){
		std::cout<<a<<"\n";
	}
}

template<typename Func,typename T>
auto mapf(Func f,std::vector<T> const& a)->std::vector<decltype(f(*begin(a)))>{
	std::vector<decltype(f(*begin(a)))> r;
	for(auto elem:a){
		r|=f(elem);
	}
	return r;
}

template<typename T>
T max(std::vector<T> const& v){
	assert(v.size());
	T r=v[0];
	for(auto elem:v){
		r=std::max(r,elem);
	}
	return r;
}

template<typename T>
T min(std::vector<T> const& v){
	assert(v.size());
	T r=v[0];
	for(auto elem:v){
		r=std::min(r,elem);
	}
	return r;
}

template<typename T>
std::vector<T> range(T lim){
	std::vector<T> r;
	for(T i=0;i<lim;i++){
		r|=i;
	}
	return r;
}

template<typename T>
std::vector<T> range(T min,T lim){
	std::vector<T> r;
	for(T i=min;i<lim;i++){
		r|=i;
	}
	return r;
}

template<typename T,T LIM>
std::array<T,LIM> range_st(){
	std::array<T,LIM> r;
	for(T i=0;i<LIM;i++){
		r[i]=i;
	}
	return r;
}

template<typename Func,typename T,size_t LEN>
auto mapf(Func f,std::array<T,LEN> const& in)->
	std::array<decltype(f(*begin(in))),LEN>
{
	std::array<decltype(f(*begin(in))),LEN> r;
	for(auto i:range(LEN)){
		r[i]=f(in[i]);
	}
	return r;
}

template<typename T,size_t LIM>
std::array<T,LIM> range_st2(){
	std::array<T,LIM> r;
	for(T i=0;i<LIM;i++){
		r[i]=i;
	}
	return r;
}

template<typename T>
std::vector<T> take(size_t n,std::vector<T> const& a){
	std::vector<T> r;
	for(auto i:range(n)){
		r|=a[i];
	}
	return r;
}

template<typename Func,typename T>
std::vector<T> filter(Func f,std::vector<T> in){
	std::vector<T> r;
	std::copy_if(begin(in),end(in),back_inserter(r),f);
	return r;
}

template<typename T>
std::ostream& operator<<(std::ostream& o,std::optional<T> const& a){
	if(a) return o<<*a;
	return o<<"NULL";
}

template<typename T>
std::ostream& operator<<(std::ostream& o,std::vector<T> const& a){
	o<<"[ ";
	for(auto elem:a){
		o<<elem<<" ";
	}
	return o<<"]";
}

template<typename T,size_t N>
class vector_limited{
	using Data=std::array<T,N>;
	union{
		char buffer[sizeof(Data)];
		Data data;
	};
	size_t size_=0;

	public:
	vector_limited(){}

	/*vector_limited(vector_limited const& a):
		data(move(a.data)),
		size_(a.size_)
	{}*/
	vector_limited(vector_limited const& a){
		for(auto elem:a){
			push_back(elem);
		}
	}

	vector_limited(vector_limited &&);

	void push_back(T t){
		if(size_==N){
			PRINT(t);
		}
		assert(size_<N);
		new(data.begin()+size_) T(std::move(t));
		size_++;
	}

	const T* begin()const{ return data.begin(); }
	const T* end()const{ return data.begin()+size_; }
	bool empty()const{ return size_==0; }

	/*operator vector<T>()const{
		vector<T> r;
		for(auto a:*this){
			r|=a;
		}
		return r;
	}*/
};

template<typename T,size_t N>
vector_limited<T,N>& operator|=(vector_limited<T,N> &a,T t){
	a.push_back(t);
	return a;
}

template<typename Func,typename T,size_t N>
vector_limited<T,N> filter(Func f,vector_limited<T,N> a){
	vector_limited<T,N> r;
	for(auto elem:a){
		if(f(elem)){
			r|=elem;
		}
	}
	return r;
}

template<typename Func,typename T,size_t N>
auto mapf(Func f,vector_limited<T,N> in)->vector_limited<decltype(f(*begin(in))),N>{
	vector_limited<decltype(f(*begin(in))),N> r;
	for(auto elem:in){
		r|=f(elem);
	}
	return r;
}

template<typename T,size_t N>
T max(vector_limited<T,N> a){
	assert(!a.empty());
	T r=*std::begin(a);
	for(auto elem:a){
		r=std::max(r,elem);
	}
	return r;
}

template<typename T,size_t N>
T min(vector_limited<T,N> a){
	assert(!a.empty());
	T r=*std::begin(a);
	for(auto elem:a){
		r=std::min(r,elem);
	}
	return r;
}

template<typename T>
int sgn(T val){
	//the sign function
	return (T(0)<val)-(val<T(0));
}

class Int_char{
	char i;

	public:
	Int_char(int);
	Int_char& operator=(int);

	char get()const;

	Int_char& operator++();
	Int_char operator++(int);
	Int_char& operator+=(int);
};

std::ostream& operator<<(std::ostream&,Int_char);
bool operator<(Int_char,Int_char);
bool operator!=(Int_char,Int_char);
bool operator==(Int_char,Int_char);
bool operator>(Int_char,int);
Int_char operator+(Int_char,unsigned);
Int_char operator-(Int_char,Int_char);

struct Uint_char{
	unsigned char i;

	public:
	Uint_char();
	Uint_char(int);
	Uint_char(unsigned);

	//so that can index arrays
	operator unsigned()const;
	unsigned char get()const;

	Uint_char operator++(int);
	Uint_char& operator+=(int);
	Uint_char& operator-=(int);
};

std::ostream& operator<<(std::ostream&,Uint_char);
bool operator<(Uint_char,Uint_char);
bool operator<(Uint_char,long unsigned);

std::vector<bool> bools();

template<typename A,typename B>
std::vector<A> firsts(std::vector<std::pair<A,B>> in){
	return mapf([](auto a){ return a.first; },in);
}

#endif
