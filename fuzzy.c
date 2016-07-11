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
	float min;
	float max;
	float first;
	float last;
	float* coefs;
	float GPA;
	float area;
	float centroide;
}fuzzyRule;

//Função que encontra os coeficientes da reta de regressão linear
void LinearRegression(int _npoints, float* _xvalues, float* _yvalues, float* _a, float* _b)
{
	float a=0;
	float b=0;

	float ysum = 0;
	float xsum = 0;

	float sumXY = 0;
	float sumXsq = 0;

	float factor = 0;
	float sumYl = 0;
	float sumXl = 0;
	float subY = 0;
	float subX = 0;

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

//Função que calcula os coeficientes das retas que compõem a regra fuzzy
int getFuzzyCoefs(fuzzyRule* _aux)
{
	float a=0,b=0;
	float xvals[2] = {_aux->first,_aux->last};
	float yvals[2];

	if(_aux->type == 3)
	{
		yvals[0] = 0;
		yvals[1] = 1;
		LinearRegression(2,xvals,yvals,&a,&b);
		_aux->coefs = (float*)malloc(sizeof(float)*2);
		if(_aux->coefs == NULL)
			return 0;

		_aux->coefs[0] = a;
		_aux->coefs[1] = b;
	}
	else if(_aux->type == 1)
	{
		yvals[0] = 1;
		yvals[1] = 0;
		LinearRegression(2,xvals,yvals,&a,&b);
		_aux->coefs = (float*)malloc(sizeof(float)*2);
		if(_aux->coefs == NULL)
			return 0;

		_aux->coefs[0] = a;
		_aux->coefs[1] = b;
	}
	else if(_aux->type == 2)
	{
		_aux->coefs = (float*)malloc(sizeof(float)*4);
		if(_aux->coefs == NULL)
			return 0;
		float mean = (_aux->first + _aux->last)/2.0;

		xvals[0] = _aux->first; xvals[1] = mean;
		yvals[0] = 0; yvals[1] = 1;
		LinearRegression(2,xvals,yvals,&a,&b);
		_aux->coefs[0] = a;
		_aux->coefs[1] = b;

		xvals[0] = mean; xvals[1] = _aux->last;
		yvals[0] = 1; yvals[1] = 0;
		LinearRegression(2,xvals,yvals,&a,&b);
		_aux->coefs[2] = a;
		_aux->coefs[3] = b;

	}

	return 1;
}

void getGPA(fuzzyRule* _aux, float _valor)
{
	if(_aux->type == 1)
	{
		if(_valor < _aux->first)
		{
			_aux->GPA = 1;
		}
		else if(_valor >= _aux->last)
		{
			_aux->GPA = 0;
		}
		else
		{
			_aux->GPA = (_aux->coefs[0]*_valor) + _aux->coefs[1];
		}
	}
	else if(_aux->type == 2)
	{
		float meanval = (_aux->first + _aux->last)/2.0;
		if(_valor <= meanval)
		{
			_aux->GPA = (_aux->coefs[0]*_valor) + _aux->coefs[1];
		}
		else
		{
			_aux->GPA = (_aux->coefs[2]*_valor) + _aux->coefs[3];
		}
	}
	else if(_aux->type == 3)
	{
		if(_valor <= _aux->first)
		{
			_aux->GPA = 0;
		}
		else if(_valor > _aux->last)
		{
			_aux->GPA = 1;
		}
		else
		{
			_aux->GPA = (_aux->coefs[0]*_valor) + _aux->coefs[1];
		}
	}
}

void getArea(fuzzyRule* _aux, float _valor)
{
	if(_aux->type == 1) //Decrescente
	{
		if(_aux->GPA == 1)
		{
			_aux->area = ((_aux->first-_aux->min) + (_aux->last-_aux->min))/2.0;
		}
		else
		{
			_aux->area = (((_valor-_aux->min) + (_aux->last-_aux->min))*_aux->GPA)/2.0;
		}
	}
	else if(_aux->type == 2) //Triangular
	{
		float meanval = (_aux->first + _aux->last)/2.0;
		_aux->area = (((2*(abs(meanval-_valor))) + (_aux->last-_aux->first))*_aux->GPA)/2.0;
	}
	else if(_aux->type == 3) //Crescente
	{
		if(_aux->GPA == 1)
		{
			_aux->area = (((_aux->max - _aux->last) + (_aux->max - _aux->first)) * _aux->GPA)/2.0;
		}
		else
		{
			_aux->area = (((_aux->max - _valor) + (_aux->max - _aux->first)) * _aux->GPA)/2.0;
	}
		}
}

//Função que calcula o centróide
void getCentroide(fuzzyRule* _aux, float _valor)
{
	//Decrescente
	if(_aux->type == 1)
	{
		if(_aux->GPA == 1)
		{
			//Area Quadrado
			float aux1 = (_aux->first - _aux->min);
			//Centroide Quadrado
			float aux2 = ((_aux->first - _aux->min)/2.0) + _aux->min;
			//Area Triangulo
			float aux3 = ((_aux->last - _aux->first)/2.0);
			//Centroide triangulo
			float aux4 = (((_aux->last - _aux->first)/3.0)+_aux->first);
			//Centroide
			_aux->centroide = ((aux1*aux2) + (aux3*aux4)) / _aux->area;
		}
		else
		{
			//Area Quadrado
			float aux1 = (_valor - _aux->min) * _aux->GPA;
			//Centroide Quadrado
			float aux2 = ((_valor - _aux->min)/2.0) + _aux->min;
			//Area triangulo
			float aux3 = ((_aux->last - _valor) * _aux->GPA)/2.0;
			//Centroide triangulo
			float aux4 = ((_aux->last - _valor)/3.0) + _valor;
			//Centroide final
			_aux->centroide = ((aux1*aux2) + (aux3*aux4)) / _aux->area;
		}
	}
		else if(_aux->type == 2) //Triangular
		{
			float meanval = (_aux->first + _aux->last)/2.0;
			_aux->centroide = meanval;
		}
		else if(_aux->type == 3) //Crescente
		{
			if(_aux->GPA == 1)
			{
				//Area do retangulo
				float aux1 = (_aux->max - _aux->last) * ((_aux->max + _aux->last)/2.0);
				//Area do triangulo
				float aux2 = ((_aux->last - _aux->first)/2.0) * ((((_aux->last - _aux->first)/3.0)*2.0) + _aux->first);
				_aux->centroide = (aux1+aux2) / _aux->area;
			}
			else
			{
				//Area do retangulo
				float aux1 = (((_aux->max - _valor) * _aux->GPA) * (_valor + _aux->max))/2.0;
				//Area do triangulo
				float aux2 = ((_valor - _aux->first)*_aux->GPA)/2.0;
				//Correcao do centroide
				float aux3 = (((_valor - _aux->first)*2.0)/3.0) + _aux->first;
				_aux->centroide = (aux1+(aux2*aux3)) / _aux->area;
			}
		}
	}


void Fuzzification(fuzzyRule* _aux, float _valor)
{
	getFuzzyCoefs(_aux);
	getGPA(_aux,_valor);
	getArea(_aux,_valor);
	getCentroide(_aux,_valor);
}


int main()
{
	//Regra positiva
	/*fuzzyRule a;
	a.type = 1;
	a.first = 853;
	a.last = 1023.5;
	a.min = 0;
	a.max = 2047;

	fuzzyRule* teta = (fuzzyRule*)malloc(sizeof(fuzzyRule)*3);
	teta[0] = a;*/

	fuzzyRule* omega = (fuzzyRule*)malloc(sizeof(fuzzyRule)*3);
	omega[0].type = 1;
	omega[0].min = -8;
	omega[0].max = 8;
	omega[0].first = -1;
	omega[0].last = 0;

	float vw = -0.5;
	Fuzzification(&omega[0],vw);

	//float v = 800;
	//Fuzzification(&teta[0],v);

	printf("\n");

	/*printf("Teta\n");
	printf("Tipo da regra: %d\n",teta[0].type);
	printf("Coefs: %f , %f\n",teta[0].coefs[0],teta[0].coefs[1]);
	printf("GPA: %f\n",teta[0].GPA);
	printf("Area: %f\n",teta[0].area);
	printf("Centroide: %f\n",teta[0].centroide);

	printf("\n\n\n");*/

	printf("Omega\n");
	printf("Tipo da regra: %d\n",omega[0].type);
	printf("Coefs: %f , %f\n",omega[0].coefs[0],omega[0].coefs[1]);
	printf("GPA: %f\n",omega[0].GPA);
	printf("Area: %f\n",omega[0].area);
	printf("Centroide: %f\n",omega[0].centroide);

	printf("\n");

	return 0;
}
