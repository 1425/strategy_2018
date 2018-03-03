#ifndef SUB_H
#define SUB_H

/*
Part of a class because just the simple setup if rediculously slow. 
double skellam_cdf(double,double,double);
*/

class Skellam_cdf{
	void *inner;

	Skellam_cdf(Skellam_cdf const&)=delete;
	Skellam_cdf& operator=(Skellam_cdf const&)=delete;

	public:
	Skellam_cdf();
	~Skellam_cdf();

	double operator()(double,double,double)const;
};

#endif
