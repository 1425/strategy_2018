#ifndef UTIL2_H
#define UTIL2_H

template<typename T>
T last(Nonempty_vector<T> const& a){
	return a[a.size()-1];
}

template<typename T,typename Func>
std::vector<T> sorted(std::vector<T> a,Func f){
	std::sort(std::begin(a),std::end(a),[&](auto e1,auto e2){ return f(e1)<f(e2); });
	return a;
}

template<typename T>
using Pair2=std::pair<T,T>;

template<typename K,typename V>
std::set<K> keys(std::map<K,V> const& m){
	std::set<K> r;
	for(auto elem:m) r|=elem.first;
	return r;
}

template<typename Func,typename T>
std::set<T> filter(Func f,std::set<T> a){
	std::set<T> r;
	for(auto elem:a){
		if(f(elem)){
			r|=elem;
		}
	}
	return r;
}

template<typename A,typename B>
std::pair<A,B> mean(Nonempty_vector<std::pair<A,B>> a){
	return make_pair(mean(firsts(a)),mean(seconds(a)));
}

/*template<typename A,typename B,typename C>
std::tuple<A,B,C> mean(Nonempty_vector<std::tuple<A,B,C>>){
	nyi
}*/

template<typename T>
T mean(Nonempty_vector<T> const& a){
	assert(a.size());
	return sum(a)/a.size();
}

template<typename T>
auto mean_or_0(std::vector<T> a){
	if(a.empty()) return decltype(mean(make_nonempty(a))){};
	return mean(make_nonempty(a));
}

template<typename T>
std::ostream& operator<<(std::ostream& o,std::set<T> const& a){
	o<<"{ ";
	for(auto const& elem:a){
		o<<elem<<" ";
	}
	return o<<"}";
}

template<typename T>
std::set<T>& operator|=(std::set<T>& s,T t){
	s.insert(t);
	return s;
}

template<typename T>
std::pair<T,T> sorted(std::pair<T,T> a){
	if(a.first>a.second) std::swap(a.first,a.second);
	return a;
}

template<typename K,typename V>
std::vector<V> values(std::map<K,V> const& a){
	std::vector<V> r;
	for(auto p:a){
		r|=p.second;
	}
	return r;
}

template<typename T>
std::set<std::pair<T,T>> unique_pairs(std::set<T> const& a){
	std::set<std::pair<T,T>> r;
	for(auto const& elem1:a){
		for(auto const& elem2:a){
			if(elem1!=elem2){
				r|=sorted(std::make_pair(elem1,elem2));
			}
		}
	}
	return r;
}

template<typename T>
auto unique_pairs(std::vector<T> a){
	//return unique_pairs(to_set(move(a)));
	std::vector<std::pair<T,T>> r;
	for(auto i:range(a.size())){
		for(auto j:range(i+1,a.size())){
			r|=std::make_pair(a[i],a[j]);
		}
	}
	return r;
}

template<typename K,typename V>
std::map<K,V>& operator|=(std::map<K,V> &a,std::pair<K,V> p){
	a.insert(move(p));
	return a;
}

template<typename K,typename V>
std::map<K,V>& operator|=(std::map<K,V> &a,std::pair<const K,V> p){
	a.insert(move(p));
	return a;
}

template<typename Func,typename K,typename V>
std::map<K,V> filter(Func f,std::map<K,V> const& in){
	std::map<K,V> r;
	for(auto p:in){
		if(f(p)){
			r|=p;
		}
	}
	return r;
}

unsigned rand(const unsigned*){
	return rand()%100;
}

template<typename A,typename B>
std::pair<A,B> rand(const std::pair<A,B>*){
	return std::make_pair(rand((A*)0),rand((B*)0));
}

template<typename T>
std::vector<T> rand(const std::vector<T>*){
	//return mapf([](auto){ return rand((T*)0); },range(rand()%40));
	return mapf([](auto){ return rand((T*)0); },range(10));
}

int rand(const int*){
	return (rand()%100)-50;
}

template<typename K,typename V>
std::map<K,V> rand(const std::map<K,V>*){
	std::map<K,V> r;
	for(auto _:range(67)){
		(void)_;
		r[rand((K*)nullptr)]=rand((V*)nullptr);
	}
	return r;
}

template<typename T>
std::pair<std::vector<T>,std::vector<T>> take_rest(size_t len,std::vector<T> in){
	std::vector<T> a;
	auto actual_len=std::min(len,in.size());
	for(auto i:range(actual_len)){
		a|=std::move(in[i]);
	}

	std::vector<T> b;
	for(auto i:range(actual_len,in.size())){
		b|=std::move(in[i]);
	}
	return make_pair(a,b);
}

template<typename A,typename B>
std::pair<A,B> operator+(std::pair<A,B> a,std::pair<A,B> b){
	return std::make_pair(a.first+b.first,a.second+b.second);
}

template<typename A,typename B>
std::pair<A,B>& operator+=(std::pair<A,B> &a,std::pair<A,B> const& b){
	a.first+=b.first;
	a.second+=b.second;
	return a;
}

template<typename T>
std::vector<double> to_doubles(std::vector<T> a){
	return mapf([](auto x){ return double(x); },a);
}

template<typename T>
Nonempty_vector<double> to_doubles(Nonempty_vector<T> const& a){
	return mapf([](auto x){ return double(x); },a);
}

double mean(Nonempty_vector<unsigned> const& a){
	return mean(to_doubles(a));
}

template<typename T>
T car(std::vector<T> const& a){
	assert(a.size());
	return a[0];
}

template<typename T>
std::vector<T> cdr(std::vector<T> const& a){
	std::vector<T> r;
	for(auto i:range(size_t(1),a.size())){
		r|=a[i];
	}
	return r;
}

template<typename T,size_t N>
std::ostream& operator<<(std::ostream& o,std::array<T,N> const& a){
	o<<"[ ";
	for(auto elem:a){
		o<<elem<<" ";
	}
	return o<<"]";
}

template<typename T,size_t N>
std::array<T,N> as_array(std::vector<T> in){
	assert(in.size()==N);

	/*std::array<T,N> r;
	for(auto i:range(N)){
		r[i]=std::move(in[i]);
	}
	return r;*/

	//TODO: Check this is safe.
	union alignas(std::array<T,N>){
		char s[sizeof(std::array<T,N>)];
	};
	std::array<T,N> &data=*(std::array<T,N>*)s;
	for(auto i:range(N)){
		new(&data[i]) T(std::move(in[i]));
	}
	return data;
}

template<typename A,typename B>
std::vector<B> seconds(std::vector<std::pair<A,B>> const& a){
	return mapf([](auto const& x){ return x.second; },a);
}

template<typename A,typename B>
auto seconds(Nonempty_vector<std::pair<A,B>> const& a){
	return mapf([](auto const& x){ return x.second; },a);
}

template<typename K,typename V>
std::vector<V> seconds(std::map<K,V> const& a){
	return mapf([](auto const& x){ return x.second; },a);
}

template<typename Func,typename T>
auto group(Func f,std::vector<T> const& in)->std::map<decltype(f(in[0])),std::vector<T>>{
	std::map<decltype(f(in[0])),std::vector<T>> r;
	for(auto const& elem:in){
		r[f(elem)]|=elem;
	}
	return r;
}

template<typename Func,typename K,typename V>
auto map_map(Func f,std::map<K,V> in)->std::map<K,decltype(f(in.begin()->second))>{
	std::map<K,decltype(f(in.begin()->second))> r;
	for(auto p:in){
		r.insert(std::make_pair(p.first,f(p.second)));
	}
	return r;
}

std::vector<std::string> split(std::string haystack,char needle){
	std::vector<std::string> r;
	std::stringstream ss;
	for(auto c:haystack){
		if(c==needle){
			r|=ss.str();
			ss.str("");
		}else{
			ss<<c;
		}
	}
	if(ss.str().size()){
		r|=ss.str();
	}
	return r;
}

#define RM_CONST(X) typename std::remove_const<X>::type
#define RM_REF(X) typename std::remove_reference<X>::type
#define ELEMENT_TYPE(X) RM_CONST(RM_REF(decltype(*std::begin(X))))

template<typename A,typename B>
auto zip(A a,B b){
	using AE=ELEMENT_TYPE(a);
	using BE=ELEMENT_TYPE(b);
	std::vector<std::pair<AE,BE>> r;
	for(auto ai=std::begin(a),bi=std::begin(b);ai!=std::end(a) && bi!=std::end(b);++ai,++bi){
		r|=std::make_pair(*ai,*bi);
	}
	return r;
}

template<typename T>
class Sorted_pair{
	std::pair<T,T> data;

	public:
	Sorted_pair(T a,T b):
		data(std::make_pair(std::min(a,b),std::max(a,b)))
	{}

	std::pair<T,T> const& get()const{ return data; }
};

template<typename T>
bool operator<(Sorted_pair<T> const& a,Sorted_pair<T> const& b){
	return a.get()<b.get();
}

template<typename T>
bool operator==(Sorted_pair<T> a,Sorted_pair<T> b){
	return a.get()==b.get();
}

template<typename T>
std::ostream& operator<<(std::ostream& o,Sorted_pair<T> const& a){
	return o<<a.get();
}

template<typename T,size_t N>
class Set_limited{
	//the point of this class is to have a set that has a max size.
	//theoretically, this could be implemented in a much more efficient way, but to start, just make it so that it does checks on insert.
	std::set<T> inner;

	public:
	Set_limited();

	Set_limited(std::array<T,N> a){
		for(auto elem:a) inner|=elem;
	}

	auto size()const{ return inner.size(); }

	auto begin()const{ return inner.begin(); }
	auto end()const{ return inner.end(); }

	std::set<T> const& get()const{ return inner; }
};

template<typename T,size_t N>
bool operator<(Set_limited<T,N> const& a,Set_limited<T,N> const& b){
	return a.get()<b.get();
}

template<typename T,size_t N>
bool operator==(Set_limited<T,N> const& a,Set_limited<T,N> const& b){
	return a.get()==b.get();
}

template<typename T,size_t N>
std::ostream& operator<<(std::ostream& o,Set_limited<T,N> const& a){
	return o<<a.get();
}

template<typename T,size_t N>
class Sorted_array{
	std::array<T,N> data;

	public:
	Sorted_array(std::array<T,N> a):data(sorted(a)){}

	std::array<T,N> const& get()const{ return data; }
};

template<typename T,size_t N>
bool operator<(Sorted_array<T,N> const& a,Sorted_array<T,N> const& b){
	return a.get()<b.get();
}

struct Px{ //probability
	double x; //note: not checking 0-1 bounds!

	operator double()const{ return x; }
};

std::ostream& operator<<(std::ostream& o,Px a){
	return o<<a.x;
}

Px& operator+=(Px& a,Px b){
	a.x+=b.x;
	return a;
}

Px& operator/=(Px &a,long unsigned int b){
	a.x/=b;
	return a;
}

Px rand(const Px*){
	return Px{(rand()%101)/100.0};
}

Px dead(const Px*){
	return Px{0};
}

template<typename A,typename B,typename C>
std::vector<A> firsts(std::vector<std::tuple<A,B,C>> const& in){
	return mapf([](auto a){ return std::get<0>(a); },in);
}

template<typename A,typename B,typename C>
std::vector<B> seconds(std::vector<std::tuple<A,B,C>> const& in){
	return mapf([](auto a){ return std::get<1>(a); },in);
}

template<typename A,typename B,typename C>
std::tuple<A,B,C> rand(const std::tuple<A,B,C>*){
	return {
		rand((A*)nullptr),
		rand((B*)nullptr),
		rand((C*)nullptr)
	};
}

template<typename A,typename B,typename C>
std::tuple<A,B,C> dead(const std::tuple<A,B,C>*){
	return {
		dead((A*)nullptr),
		dead((B*)nullptr),
		dead((C*)nullptr)
	};
}

template<typename A,typename B,typename C>
std::tuple<A,B,C> operator/(std::tuple<A,B,C> t,long unsigned int i){
	std::get<0>(t)/=i;
	std::get<1>(t)/=i;
	std::get<2>(t)/=i;
	return t;
}

template<typename A,typename B,typename C>
std::tuple<A,B,C>& operator+=(std::tuple<A,B,C> &a,std::tuple<A,B,C> b){
	std::get<0>(a)+=std::get<0>(b);
	std::get<1>(a)+=std::get<1>(b);
	std::get<2>(a)+=std::get<2>(b);
	return a;
}

template<typename K,typename V>
std::map<K,V> take(size_t n,std::map<K,V> const& in){
	std::map<K,V> r;
	auto at=std::begin(in);
	size_t i=0;
	for(;at!=std::end(in) && i<n;++at,++i){
		r|=*at;
	}
	return r;
}

template<typename T>
std::vector<T> flatten(Nonempty_vector<std::vector<T>> a){
	std::vector<T> r;
	for(auto elem:a) r|=elem;
	return r;
}

template<typename A,typename B,typename C>
std::vector<C> thirds(std::vector<std::tuple<A,B,C>> a){
	return mapf([](auto elem){ return std::get<2>(elem); },a);
}

template<typename T>
std::vector<std::pair<size_t,T>> enumerate_from(size_t start,std::vector<T> in){
	std::vector<std::pair<size_t,T>> r;
	for(auto elem:in){
		r|=std::make_pair(start++,elem);
	}
	return r;
}

#endif
