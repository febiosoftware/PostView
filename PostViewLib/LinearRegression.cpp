#include "stdafx.h"
#include "LinearRegression.h"

bool LinearRegression(const vector<pair<double, double> >& data, pair<double, double>& res)
{
	res.first = 0.0;
	res.second = 0.0;

	int n = (int)data.size();
	if (n == 0) return false;

	double mx = 0.0, my = 0.0;
	double sxx = 0.0, sxy = 0.0;
	for (int i=0; i<n; ++i) 
	{
		double xi = data[i].first;
		double yi = data[i].second;
		mx += xi;
		my += yi;

		sxx += xi*xi;
		sxy += xi*yi;
	}
	mx /= (double) n;
	my /= (double) n;
	sxx /= (double)n;
	sxy /= (double)n;

	double D = sxx - mx*mx;
	if (D == 0.0) return false;

	double a = (sxy - mx*my)/D;
	double b = my - a*mx;

	res.first = a;
	res.second = b;

	return true;
}
