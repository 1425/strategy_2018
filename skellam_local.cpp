#include "skellam_local.h"
#include<iostream>
#include<boost/math/special_functions/bessel.hpp>

using namespace std;

#define PRINT(X) cout<<""#X<<":"<<(X)<<"\n";
#define nyi { cout<<"nyi "<<__LINE__<<"\n"; exit(44); }

template<typename T>
vector<T>& operator|=(vector<T>& a,T b){
	a.push_back(b);
	return a;
}

template<typename A,typename B>
std::ostream& operator<<(std::ostream& o,std::pair<A,B> const& a){
	return o<<"("<<a.first<<","<<a.second<<")";
}

template<typename T>
vector<T> range(T start,T end,T step){
	vector<T> r;
	for(auto i=start;i<end;i+=step){
		r|=i;
	}
	return r;
}

vector<int> range(int a){
	return range(0,a,1);
}

void bessel_demo(){
	//first, just see that can call the modified Bessel function of the first kind
	auto v=7;
	//auto x=0;
	for(auto x:range(-10,10,1)){
		auto out=boost::math::cyl_bessel_i(v,x);
		cout<<x<<" "<<out<<"\n";//PRINT(out);
	}
}

double skellam_pdf(double u1,double u2,int k){
	assert(u1>=0);
	assert(u2>=0);
	//e^-(u1+u2)*(u1/u2)^(k/2)*I|k|(2sqrt(u1*u2))
	//"cyl_bessel_i" = modified Bessel function of the first kind

	if(u1==0){
		u1=.00001;
	}
	if(u2==0){
		u2=.00001;
	}

	/*PRINT(u1)
	PRINT(u2)
	PRINT(k)
	PRINT(exp(-(u1+u2)));
	PRINT(pow(u1/u2,double(k)/2))
	PRINT(boost::math::cyl_bessel_i(k,2*sqrt(u1*u2)));*/
	
	return exp(-(u1+u2))*pow(u1/u2,double(k)/2)*boost::math::cyl_bessel_i(k,2*sqrt(u1*u2));
}

pair<double,double> skellam_totals(double u1,double u2){
	//this has been benchmarked at about .07 ms with optimizations off, and about 1/2 of that with them on.
	double total=0;
	double under=0;
	double over=0;
	total+=skellam_pdf(u1,u2,0);
	int i=1;
	while(total<.99){
		auto lower_side=skellam_pdf(u1,u2,-i);
		auto upper_side=skellam_pdf(u1,u2,i);

		i++;

		under+=lower_side;
		over+=upper_side;
		total+=lower_side;
		total+=upper_side;
	}
	return make_pair(under,over);
}

pair<double,double> skellam_totals(int initial_condition,double u1,double u2){
	//this has been benchmarked at about .07 ms with optimizations off, and about 1/2 of that with them on.
	double total=0;
	double under=0;
	double over=0;
	total+=skellam_pdf(u1,u2,0);
	int i=1;
	while(total<.99){
		auto lower_side=skellam_pdf(u1,u2,initial_condition-i);
		auto upper_side=skellam_pdf(u1,u2,initial_condition+i);

		i++;

		under+=lower_side;
		over+=upper_side;
		total+=lower_side;
		total+=upper_side;
	}
	return make_pair(under,over);
}

void skellam_demo(){
	auto u1=1;
	auto u2=1;
	auto k=-1;
	auto v=skellam_pdf(u1,u2,k);
	PRINT(v);
}

#if 0
//going to try to calculate CDF of the Skellam distribution via Bessel functions
int main(){
	//could just get the value at 0 and double the speed? (by only integrating one half)

	double x=0;
	unsigned calculations=0;
	for(auto i:range<double>(0,15,.1)){
		for(auto j:range<double>(0,10,.1)){
			auto t=skellam_totals(i,j);
			//cout<<i<<" "<<j<<" "<<t<<"\n";
			if(isnan(t.first) || isnan(t.second)){
				PRINT(i) PRINT(j) PRINT(t)
				nyi
			}
			if(isnan(t.second))nyi
			x+=t.first+t.second;
			calculations++;
		}
	}
	PRINT(x);
	PRINT(calculations);
	PRINT(x/calculations);
}
#endif
