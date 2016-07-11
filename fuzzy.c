/*
	FEDERAL UNIVERSITY OF UBERLÂNDIA
	Faculty of Electrical Engineering
	Biomedical Engineering Lab
	Authors: Gabriel Brito
					 Andrei Nakagawa
*/

//Função que encontra os coeficientes da reta de regressão linear
void LinearRegression(int _npoints, double* _xvalues, double* _yvalues, double* _a, double* _b)
{
	double a=0;
	double b=0;

	double ysum = 0;
	double xsum = 0;

	double sumXY = 0;
	double sumXsq = 0;

	double factor = 0;
	double sumYl = 0;
	double sumXl = 0;
	double subY = 0;
	double subX = 0;

	int i=0;
	for(i=0; i<_npoints; i++)
	{
		xsum += _xvalues[i];
		ysum += _yvalues[i];
	}

	for(i=0; i<_npoints; i++)
	{
		sumXY += _xvalues[i]*_yvalues[i];
		sumXsq += _xvalues[i]*_xvalues[i];
	}

	//Finding "a"
	factor = xsum / _npoints;
	sumXl = xsum * factor;
	sumYl = ysum * factor;
	subX = sumXsq - sumXl;
	subY = sumXY - sumYl;

	a = subY / subX;
	b = (ysum - (xsum * a)) / _npoints;

	*_a = a;
	*_b = b;
}

int main()
{
	return 0;
}
