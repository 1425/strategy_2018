#include<iostream>
#include<iomanip>
#include<cmath>
#include "util.h"
#include "util2.h"
#include "decode.h"

//see what the correlation is between how a team does in a match at scale vs. switch

using namespace std;

template<typename A,typename B>
Nonempty_vector<pair<A,B>> zip(Nonempty_vector<A> a,Nonempty_vector<B> b){
	vector<pair<A,B>> r;
	for(size_t i=0;i<a.size() && i<b.size();i++){
		r|=make_pair(a[i],b[i]);
	}
	return make_nonempty(r);
}

template<typename A,typename B>
vector<pair<A,B>> cross(vector<A> a,vector<B> b){
	vector<pair<A,B>> r;
	for(auto a1:a){
		for(auto b1:b){
			r|=make_pair(a1,b1);
		}
	}
	return r;
}

template<typename A,typename B>
Nonempty_vector<pair<A,B>> cross(Nonempty_vector<A> a,Nonempty_vector<B> b){
	return make_nonempty(cross(a.get(),b.get()));
}

double variance(Nonempty_vector<double> in){
	return mean(mapf(
		[in](auto x){
			auto d=x-mean(in);
			return d*d;
		},
		in
	));
}

vector<double> deviations(vector<double> in){
	//subtract the mean from each of the values
	return mapf(
		[in](auto x){ return x-mean(make_nonempty(in)); },
		in
	);
}

Nonempty_vector<double> deviations(Nonempty_vector<double> a){
	return make_nonempty(deviations(a.get()));
}

double covariance(Nonempty_vector<pair<double,double>> in){
	auto xd=deviations(firsts(in));
	auto yd=deviations(seconds(in));
	return mean(mapf([](auto p){ return p.first*p.second; },zip(xd,yd)));
}

using Line=pair<double,double>; //first=m, second=b

double run(Line line,double x){
	return line.first*x+line.second;
}

Line best_fit_line(Nonempty_vector<pair<double,double>> in /*pairs of x,y*/){
	auto xs=firsts(in);
	auto ys=seconds(in);
	auto mx=mean(xs);
	auto my=mean(ys);

	auto vx=variance(xs);
	if(vx==0){
		//avoid dividing by 0.
		return make_pair(0,my);
	}

	auto m=covariance(in)/variance(xs);
	auto b=my-m*mx;

	return make_pair(m,b);
}

//double sub(double,double)nyi
double sub(pair<double,double>)nyi

double get_r_squared(Nonempty_vector<pair<double,double>> data,Line line){
	const auto xs=firsts(data);
	const auto ys=seconds(data);
	const auto fs=mapf([line](auto x){ return run(line,x); },xs);
	const auto residuals=mapf(
		[line](auto p){
			auto x=p.first;
			auto y=p.second;
			return y-run(line,x);
		},
		data
	);
	auto my=mean(ys);
	auto ss_tot=sum(mapf(
		[my](auto yi){ return pow(yi-my,2); },
		ys
	));
	/*auto ss_reg=sum(mapf(
		[my](auto fi){ return pow(fi-my,2); },
		fs
	));*/
	auto ss_res=sum(mapf(
		[](auto p){
			auto yi=p.first;
			auto fi=p.second;
			return pow(yi-fi,2);
		},
		zip(ys,fs)
	));
	return 1-ss_res/ss_tot;
}

/*
 * Try to fit the teams in a different way:
 * look at max scale & max switch to get a slope, then try to fit based on that & see how it fits
 * */

Line new_fit(Nonempty_vector<pair<double,double>> points){
	auto max_x=max(firsts(points));
	auto max_y=max(seconds(points));
	double slope=max_y/max_x;
	PRINT(slope);
	nyi
}

int main(){
	Nonempty_vector<pair<double,double>> x{{0,1},{1,2}};
	cout<<best_fit_line(x)<<"\n";

	//1) read in all the data
	auto r=interpret(read_csv("2018orwil/post_event.csv"));

	//2) group by team #
	vector<tuple<Team,double,double,double>> out;
	for(auto p:r){
		//cout<<p.first<<"\t";
		auto d=mapf([](auto a){ return pair<double,double>(a.scale_cubes,a.switch_cubes); },p.second.cubes);
		if(d.size()){
			auto nd=make_nonempty(d);
			//3) calculate best fit
			auto b=best_fit_line(nd);
			auto r2=get_r_squared(nd,b);
			out|=make_tuple(p.first,b.first,b.second,r2);
			//cout<<setprecision(2);
			//cout<<b.first<<"\t"<<b.second<<"\n";
		}
	}

	cout<<setprecision(2);
	for(auto row:sorted(out,[](auto a){ return get<1>(a); })){
		cout<<get<0>(row)<<"\t"<<get<1>(row)<<"\t"<<get<2>(row)<<"\t"<<get<3>(row)<<"\n";
	}

	return 0;
}
