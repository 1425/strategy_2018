#ifndef UTIL_H
#define UTIL_H

#include<set>
#include<iostream>
#include<vector>
#include<algorithm>
#include<cassert>
#include<sstream>
#include<map>
#include<optional>

#define nyi { std::cout<<"nyi "<<__FILE__<<":"<<__LINE__<<"\n"; exit(44); }

#define PRINT(X) { std::cout<<""#X<<":"<<(X)<<"\n"; }

#define INST(A,B) A B;
#define SHOW(A,B) o<<""#B<<":"<<a.B<<" ";
#define CMP1(name) if(a.name<b.name) return 1; if(b.name<a.name) return 0;

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
auto mapf(Func f,T const& a)->std::vector<decltype(f(*begin(a)))>{
	std::vector<decltype(f(*begin(a)))> r;
	for(auto elem:a){
		r.push_back(f(elem));
	}
	return r;
}

#define MAP(F,IN) mapf([&](auto elem){ return F(elem); },IN)

template<typename T>
std::vector<T> to_vector(std::set<T> a){
	return std::vector<T>(begin(a),end(a));
}

/*template<typename Func,typename T>
auto mapf(Func f,std::set<T> const& in){
	return mapf(f,to_vector(in));
}*/

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
	/*
	using Out_elem=decltype(f(*begin(in)));
	using Out=std::array<Out_elem,LEN>;
	char s[sizeof(Out)];//assuming that we don't have to care about alignment requirements.
	Out &data=*(Out*)s;
	
	for(auto i:range(LEN)){
		new(&data[i]) Out_elem(f(in[i]));
	}
	return data;*/
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
	for(auto i:range(std::min(n,a.size()))){
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
class Nonempty_vector{
	std::vector<T> data;

	public:
	explicit Nonempty_vector(std::initializer_list<T> a):data(a){
		assert(a.size());
	}

	explicit Nonempty_vector(std::vector<T> a):data(a){
		assert(a.size());
	}

	std::vector<T> const& get()const{
		return data;
	}

	operator std::vector<T> const&()const{ return data; }

	auto size()const{ return data.size(); }
	auto begin()const{ return data.begin(); }
	auto end()const{ return data.end(); }
	T const& operator[](size_t i)const{ return data[i]; }
};

template<typename T>
std::ostream& operator<<(std::ostream& o,Nonempty_vector<T> const& a){
	return o<<a.get();
}

template<typename T>
Nonempty_vector<T> make_nonempty(std::vector<T> a){
	assert(a.size());
	return Nonempty_vector<T>(a);
}

template<typename T>
T max(Nonempty_vector<T> a){
	return max(a.get());
}

template<typename Func,typename T>
auto mapf(Func f,Nonempty_vector<T> in) -> Nonempty_vector<decltype(f(*std::begin(in)))>{
	return make_nonempty(mapf(f,in.get()));
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

template<typename A,typename B>
Nonempty_vector<A> firsts(Nonempty_vector<std::pair<A,B>> in){
	return mapf([](auto a){ return a.first; },in);
}

void write_file(std::string const& filename,std::string const& contents);

std::string tag(std::string name,std::string contents);
std::string tag(std::string name,std::string extra,std::string contents);
std::string html(std::string);
std::string body(std::string);
std::string table(std::string);
std::string tr(std::string);
std::string td(std::string);
std::string th(std::string);
std::string small(std::string);
std::string title(std::string);
std::string head(std::string);
std::string h1(std::string);

template<typename T>
std::vector<std::pair<size_t,T>> enumerate(std::vector<T> a){
	std::vector<std::pair<size_t,T>> r;
	for(auto i:range(a.size())){
		r|=std::make_pair(i,a[i]);
	}
	return r;
}

std::string join(std::vector<std::string> const&);

template<size_t LEN>
std::string join(std::array<std::string,LEN> a){
	std::stringstream ss;
	for(auto elem:a){
		ss<<elem;
	}
	return ss.str();
}

template<typename T>
std::string as_string(T t){
	std::stringstream ss;
	ss<<t;
	return ss.str();
}

template<typename T>
std::vector<T> operator+(std::vector<T> a,std::vector<T> b){
	a|=b;
	return a;
}

template<typename T>
T sum(std::vector<T> const& v){
	T r{};
	for(auto &a:v){
		r+=a;
	}
	return r;
}

template<typename T>
T sum(Nonempty_vector<T> const& v){
	return sum(v.get());
}

/*template<typename T,size_t N>
T sum(std::array<T,N> const& a){
	T r{};
	for(auto elem:a){
		r+=elem;
	}
	return r;
}*/

template<typename T>
T sum(std::array<T,0> const& a){
	return 0;
}

template<typename T,size_t N>
T sum(std::array<T,N> const& a){
	assert(N);
	T r=a[0];
	for(auto at=1+begin(a);at!=end(a);++at){
		r+=*at;
	}
	return r;
}

template<typename T>
T sorted(T a){
	std::sort(std::begin(a),std::end(a));
	return a;
}

template<typename T>
T reversed(T a){
	std::reverse(std::begin(a),std::end(a));
	return a;
}

double mean(Nonempty_vector<double> const&);

template<typename A,typename B>
std::ostream& operator<<(std::ostream& o,std::map<A,B> a){
	o<<"{";
	for(auto elem:a) o<<elem;
	return o<<"}";
}

template<typename T>
std::set<T> to_set(std::vector<T> v){
	return std::set<T>(begin(v),end(v));
}

template<typename A,typename B,size_t LEN>
std::array<std::pair<A,B>,LEN> zip(std::array<A,LEN> a,std::array<B,LEN> b){
	std::array<std::pair<A,B>,LEN> r;
	for(auto i:range(LEN)){
		r[i]=std::make_pair(a[i],b[i]);
	}
	return r;
}

template<typename T,size_t N>
T max(std::array<T,N> a){
	assert(N);
	T r=a[0];
	for(auto elem:a){
		r=std::max(elem,r);
	}
	return r;
}

template<typename T,size_t N>
size_t index_max(std::array<T,N> const& a){
	assert(N);
	size_t r=0;
	T value=a[0];
	for(auto p:enumerate(a)){
		if(p.second>value){
			r=p.first;
			value=p.second;
		}
	}
	return r;
}

template<typename Func,typename T,size_t N>
size_t index_max(Func f,std::array<T,N> const& in){
	return index_max(mapf(f,in));
}

template<typename T,size_t N>
std::array<T,N-1> without_index(size_t i,std::array<T,N> a){
	assert(i<N);
	std::array<T,N-1> r;

	size_t in_index=0;
	size_t out_index=0;
	while(in_index<N){
		if(in_index==i) in_index++;

		r[out_index]=a[in_index];

		in_index++;
		out_index++;
	}
	return r;
}

template<typename T,size_t N>
std::array<std::pair<size_t,T>,N> enumerate(std::array<T,N> a){
	std::array<std::pair<size_t,T>,N> r;
	for(auto i:range(N)){
		r[i]=std::make_pair(i,a[i]);
	}
	return r;
}

std::vector<std::string> args(int argc,char **argv);
int atoi(std::string const&);

template<typename K,typename V>
std::vector<K> firsts(std::map<K,V> m){
	return mapf([](auto a){ return a.first; },m);
}

template<typename T>
std::vector<std::array<T,3>> permutations(std::array<T,3> a){
	return {
		{a[0],a[1],a[2]},
		{a[0],a[2],a[1]},
		{a[1],a[0],a[2]},
		{a[1],a[2],a[0]},
		{a[2],a[0],a[1]},
		{a[2],a[1],a[0]}
	};
}


#endif
