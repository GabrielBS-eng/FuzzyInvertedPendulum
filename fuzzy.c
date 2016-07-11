/*
	FEDERAL UNIVERSITY OF UBERLÂNDIA
	Faculty of Electrical Engineering
	Biomedical Engineering Lab
	Authors: Gabriel Brito
		 Andrei Nakagawa
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

//Estrutura para armazenar uma regra fuzzy
typedef struct {
	int type;
	float first;
	float last;
	float* coefs;
}fuzzyRule;

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

//Fuzzyficação
//Consiste em obter os valores mínimos e máximos que constituem a regra Fuzzy
//e obter os valores dos coeficientes das retas de regressão
//A regra fuzzy pode ser de três tipos: Crescente, Decrescente, Triangular
double* Fuzzyfication(double _min, double _max, int _ruletype)
{
	//Crescente
	if(_ruletype == 1)
	{
		double* resp = (double*)malloc(sizeof(double)*2);
		double auxa=0,auxb=0;
		double xvals[2] = {_min,_max};
		double yvals[2] = {0,1};
		LinearRegression(2,xvals,yvals,&auxa,&auxb);
		resp[0] = auxa;
		resp[1] = auxb;
		return resp;
	}
	//Decrescente
	else if(_ruletype == 2)
	{
		double* resp = (double*)malloc(sizeof(double)*2);
		double auxa=0,auxb=0;
		double xvals[2] = {_min,_max};
		double yvals[2] = {1,0};
		LinearRegression(2,xvals,yvals,&auxa,&auxb);
		resp[0] = auxa;
		resp[1] = auxb;
		return resp;
	}
	//Triangular
	else
	{
		double* resp = (double*)malloc(sizeof(double)*4);
		double meanv = (_min+_max)/2;
		double auxa=0,auxb=0;
		double xvals[2] = {_min,meanv};
		double yvals[2] = {0,1};
		LinearRegression(2,xvals,yvals,&auxa,&auxb);
		resp[0] = auxa;
		resp[1] = auxb;
		xvals[0] = meanv; xvals[1] = _max;
		yvals[0] = 1; yvals[1] = 0;
		LinearRegression(2,xvals,yvals,&auxa,&auxb);
		resp[2] = auxa;
		resp[3] = auxb;
		return resp;
	}
}

int main()
{
	fuzzyRule a;
	a.type = 1;
	printf("Tipo da regra: %d\n",a.type);
	return 0;
}
