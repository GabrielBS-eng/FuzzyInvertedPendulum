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

//Função que dado os valores mínimos e máximos, o tipo de regra, os coeficientes
//de regressão e o valor do eixo x, encontra o valor de Mi
double MeasureMi(double _val, double _min, double _max, int _ruletype, double* _coefs)
{
	//Crescente
	if(_ruletype == 1)
	{
		if(_val <= _min)
			return 0;
		else if(_val >= _max)
			return 1;
		else
				return (_coefs[0]*_val) + _coefs[1];

	}
	//Decrescente
	else if(_ruletype == 2)
	{
		if(_val <= _min)
			return 1;
		else if(_val >= _max)
			return 0;
		else
				return (_coefs[0]*_val) + _coefs[1];
	}
	//Triangular
	else
	{
		if(_val <= _min || _val >= _max)
			return 0;
		else
		{
			double media = (_min+_max)/2.0;
			if(_val < media)
				return (_val*_coefs[0]) + _coefs[1];
			else if(_val > media)
				return (_val*_coefs[2]) + _coefs[3];
			else
				return 1;
		}
	}
}

double* Defuzzification(double _valMi, double _min, double _max)
{
	double* resp = (double*)malloc(sizeof(double)*2);
	resp[0]=0; resp[1]=0;
	int numbVals = _max - _min;
	int i=0;
	for (i=0; i<numbVals; i++)
	{
		resp[0] = resp[0] + (_valMi*(_min+i));
		resp[1] = resp[1] + _valMi;
	}
	return resp;
}

int main()
{
	//Criar as regras para Teta
	//Valores mínimos e máximos de teta
	double tetaMax = 75;
	double tetaMin = 15;
  //Para teta grande
	double tetaGrandeTipo = 1;
	double* tetaGrandeCoefs = Fuzzyfication(tetaMin,tetaMax,tetaGrandeTipo);
	double tetaGrandeMi = MeasureMi(60,tetaMin,tetaMax,tetaGrandeTipo,tetaGrandeCoefs);
	//Para Teta pequeno
	double tetaPeqTipo = 2;
	double* tetaPeqCoefs = Fuzzyfication(tetaMin,tetaMax,tetaPeqTipo);
	double tetaPeqMi = MeasureMi(60,tetaMin,tetaMax,tetaPeqTipo,tetaPeqCoefs);
	//Para Teta médio
	double tetaMedTipo = 3;
	double* tetaMedCoefs = Fuzzyfication(tetaMin,tetaMax,tetaMedTipo);
	double tetaMedMi = MeasureMi(60,tetaMin,tetaMax,tetaMedTipo,tetaMedCoefs);

	//Criar regras para Omega
	double omegaMin = 1;
	double omegaMax = 3;
	//Para omega grande
	double omegaGrandeTipo = 1;
	double* omegaGrandeCoefs = Fuzzyfication(omegaMin,omegaMax,omegaGrandeTipo);
	//Para omega pequeno
	double omegaPeqTipo = 2;
	double* omegaPeqCoefs = Fuzzyfication(omegaMin,omegaMax,omegaPeqTipo);

	//Criar regras para Força
	double forcaMin = 0;
	double forcaMax = 500;
	//Força grande
	double forcaGrandeTipo = 1;
	double* forcaGrandeCoefs = Fuzzyfication(forcaMin,forcaMax,forcaGrandeTipo);
	//Força Pequena
	double forcaPeqTipo = 2;
	double* forcaPeqCoefs = Fuzzyfication(forcaMin,forcaMax,forcaPeqTipo);
	//Força Média
	double forcaMedTipo = 3;
	double* forcaMedCoefs = Fuzzyfication(forcaMin,forcaMax,forcaMedTipo);
	double* x1 = Defuzzification(0.3,forcaMin,50);
	double* x2 = Defuzzification(0.55,50,200);
	double* x3 = Defuzzification(0.8,200,forcaMax);

	double forcaFinal = (x1[0]+x2[0]+x3[0]) / (x1[1]+x2[1]+x3[1]);
	printf("Forca final: %lf\n", forcaFinal);

	return 0;
}
