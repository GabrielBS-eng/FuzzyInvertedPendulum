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
	float centroid;
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

/*void getArea2Fuzzification(fuzzyRule* _aux, float _valor)
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
		float aux1 = 2*fabsf(meanval-_valor);
		float aux2 = (_aux->last - _aux->first);
		_aux->area = ((aux1+aux2)*_aux->GPA)/2.0;
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
}*/

void getArea2Defuzzification(fuzzyRule* _aux)
{
	float _valor;

	if(_aux->type == 1) //Decrescente
	{
		if(_aux->GPA == 1)
		{
			_aux->area = ((_aux->first-_aux->min) + (_aux->last-_aux->min))/2.0;
		}
		else
		{
			//Aqui encontra-se o ponto onde o valor de GPA de área cruza com o valor no eixo x da equação de reta
			_valor = (_aux->GPA - _aux->coefs[1])/_aux->coefs[0];

			//Faz o cálculo da primeira área de região nebulosa
			_aux->area = (((_valor-_aux->min) + (_aux->last-_aux->min))*_aux->GPA)/2.0;
		}
	}
	else if(_aux->type == 2) //Triangular
	{
		//Aqui encontra-se um dos pontos onde o valor de GPA de área cruza com os valores no eixo x das equações de reta da área triangular
		_valor = (_aux->GPA - _aux->coefs[1])/_aux->coefs[0];

		//Faz o cálculo da área da região nebulosa triangular, que quando cortada é vista como trapézio equilátero
		float meanval = (_aux->first + _aux->last)/2.0;
		float aux1 = 2*fabsf(meanval-_valor);
		float aux2 = (_aux->last - _aux->first);
		_aux->area = ((aux1+aux2)*_aux->GPA)/2.0;
	}
	else if(_aux->type == 3) //Crescente
	{
		if(_aux->GPA == 1)
		{
			_aux->area = (((_aux->max - _aux->last) + (_aux->max - _aux->first)) * _aux->GPA)/2.0;
		}
		else
		{
			//Aqui encontra-se o ponto onde o valor de GPA de área cruza com o valor no eixo x da equação de reta
			_valor = (_aux->GPA - _aux->coefs[1])/_aux->coefs[0];

			//Faz o cálculo da última área de região nebulosa
			_aux->area = (((_aux->max - _valor) + (_aux->max - _aux->first)) * _aux->GPA)/2.0;
		}
	}
}

//Função que calcula o centróide para fuzzificar as informações, caso necessário
/*void getCentroid2Fuzzification(fuzzyRule* _aux, float _valor)
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
			_aux->centroid = ((aux1*aux2) + (aux3*aux4)) / _aux->area;
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
			_aux->centroid = ((aux1*aux2) + (aux3*aux4)) / _aux->area;
		}
	}
	else if(_aux->type == 2) //Triangular
	{
		float meanval = (_aux->first + _aux->last)/2.0;
		_aux->centroid = meanval;
	}
	else if(_aux->type == 3) //Crescente
	{
		if(_aux->GPA == 1)
		{
			//Area do retangulo
			float aux1 = (_aux->max - _aux->last) * ((_aux->max + _aux->last)/2.0);
			//Area do triangulo
			float aux2 = ((_aux->last - _aux->first)/2.0) * ((((_aux->last - _aux->first)/3.0)*2.0) + _aux->first);
			_aux->centroid = (aux1+aux2) / _aux->area;
		}
		else
		{
			//Area do retangulo
			float aux1 = (((_aux->max - _valor) * _aux->GPA) * (_valor + _aux->max))/2.0;
			//Area do triangulo
			float aux2 = ((_valor - _aux->first)*_aux->GPA)/2.0;
			//Correcao do centroide
			float aux3 = (((_valor - _aux->first)*2.0)/3.0) + _aux->first;
			_aux->centroid = (aux1+(aux2*aux3)) / _aux->area;
		}
	}
}*/

//Função que calcula o centróide para a defuzzificação das áreas
void getCentroid2Defuzzification(fuzzyRule* _aux)
{
	float _valor;

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
			_aux->centroid = ((aux1*aux2) + (aux3*aux4)) / _aux->area;
		}
		else
		{
			//Aqui encontra-se o ponto onde o valor de GPA de área cruza com o valor no eixo x da equação de reta
			_valor = (_aux->GPA - _aux->coefs[1])/_aux->coefs[0];

			//Area Quadrado
			float aux1 = (_valor - _aux->min) * _aux->GPA;
			//Centroide Quadrado
			float aux2 = ((_valor - _aux->min)/2.0) + _aux->min;
			//Area triangulo
			float aux3 = ((_aux->last - _valor) * _aux->GPA)/2.0;
			//Centroide triangulo
			float aux4 = ((_aux->last - _valor)/3.0) + _valor;
			//Centroide final
			_aux->centroid = ((aux1*aux2) + (aux3*aux4)) / _aux->area;
		}
	}
	else if(_aux->type == 2) //Triangular
	{
		float meanval = (_aux->first + _aux->last)/2.0;
		_aux->centroid = meanval;
	}
	else if(_aux->type == 3) //Crescente
	{
		if(_aux->GPA == 1)
		{
			//Area do retangulo
			float aux1 = (_aux->max - _aux->last) * ((_aux->max + _aux->last)/2.0);
			//Area do triangulo
			float aux2 = ((_aux->last - _aux->first)/2.0) * ((((_aux->last - _aux->first)/3.0)*2.0) + _aux->first);
			_aux->centroid = (aux1+aux2) / _aux->area;
		}
		else
		{
			//Aqui encontra-se o ponto onde o valor de GPA de área cruza com o valor no eixo x da equação de reta
			_valor = (_aux->GPA - _aux->coefs[1])/_aux->coefs[0];

			//Area do retangulo
			float aux1 = (((_aux->max - _valor) * _aux->GPA) * (_valor + _aux->max))/2.0;
			//Area do triangulo
			float aux2 = ((_valor - _aux->first)*_aux->GPA)/2.0;
			//Correcao do centroide
			float aux3 = (((_valor - _aux->first)*2.0)/3.0) + _aux->first;
			_aux->centroid = (aux1+(aux2*aux3)) / _aux->area;
		}
	}
}


void Fuzzification(fuzzyRule* _aux, float _valor)
{
	getFuzzyCoefs(_aux);
	getGPA(_aux,_valor);
	//getArea2Fuzzification(_aux,_valor);
	//getCentroid2Fuzzification(_aux,_valor);
}

void Analysis(float GPA0, float GPA1, float GPA2, float GPA3, float GPA4, float GPA5, float* GPA6, float* GPA7, float* GPA8, float* GPA9, float* GPA10)
{
	if(GPA0 > 0 && GPA3 > 0)
	{
		if(GPA0 > GPA3) *GPA6 = GPA3;
		else *GPA6 = GPA0;
	}
	if(GPA0 > 0 && GPA4 > 0)
	{
		if(GPA0 > GPA4)
		{
			if(GPA4 > *GPA6) *GPA6 = GPA4;
		}
		else
		{
			if(GPA0 > *GPA6) *GPA6 = GPA0;
		}
	}

	if(GPA0 > 0 && GPA5 > 0)
	{
		if(GPA0 > GPA5) *GPA7 = GPA5;
		else *GPA7 = GPA0;
	}

	if(GPA1 > 0 && GPA3 > 0)
	{
		if(GPA1 > GPA3)
		{
			if(GPA3 < *GPA7) *GPA7 = GPA3;
		}
		else
		{
			if(GPA1 < *GPA7) *GPA7 = GPA3;
		}
	}

	if(GPA1 > 0 && GPA4 > 0)
	{
		if(GPA1 > GPA4) *GPA8 = GPA4;
		else *GPA8 = GPA1;
	}

	if(GPA1 > 0 && GPA5 > 0)
	{
		if(GPA1 > GPA5) *GPA9 = GPA5;
		else *GPA9 = GPA1;
	}

	if(GPA2 > 0 && GPA3 > 0)
	{
		if(GPA2 > GPA3)
		{
			if(GPA3 < *GPA9) *GPA9 = GPA3;
		}
		else
		{
			if(GPA2 < *GPA9) *GPA9 = GPA3;
		}
	}

	if(GPA2 > 0 && GPA4 > 0)
	{
		if(GPA2 > GPA4) *GPA10 = GPA4;
		else *GPA10 = GPA2;
	}

	if(GPA2 > 0 && GPA5 > 0)
	{
		if(GPA2 > GPA5)
		{
			if(GPA5 > *GPA10) *GPA10 = GPA5;
		}
		else
		{
			if(GPA2 > *GPA10) *GPA10 = GPA2;
		}
	}
}

float Defuzzification(fuzzyRule* A6, fuzzyRule* A7, fuzzyRule* A8, fuzzyRule* A9, fuzzyRule* A10)
{
		float giveTorq;
		float totalArea;
		float aux1;

		getFuzzyCoefs(A6);
		getArea2Defuzzification(A6);
		getCentroid2Defuzzification(A6);

		getFuzzyCoefs(A7);
		getArea2Defuzzification(A7);
		getCentroid2Defuzzification(A7);

		getFuzzyCoefs(A8);
		getArea2Defuzzification(A8);
		getCentroid2Defuzzification(A8);

		getFuzzyCoefs(A8);
		getArea2Defuzzification(A8);
		getCentroid2Defuzzification(A8);

		getFuzzyCoefs(A8);
		getArea2Defuzzification(A8);
		getCentroid2Defuzzification(A8);

		//Defuzzificação final
		totalArea = (A6->area + A7->area + A8->area + A9->area + A10->area);

		aux1 = (A6->area * A6->centroid) + (A7->area * A7->centroid) + (A8->area * A8->centroid) + (A9->area * A9->centroid) + (A10->area * A10->centroid);

		giveTorq = aux1/totalArea;

		return giveTorq;
}


int main()
{
	float v, vw, valorResposta;

	//Regra positiva
  fuzzyRule* teta = (fuzzyRule*)malloc(sizeof(fuzzyRule)*3);
	teta[0].type = 1;
	teta[0].first = 853;
	teta[0].last = 1023.5;
	teta[0].min = 0;
	teta[0].max = 2047;

	teta[1].type = 2;
	teta[1].first = 853;
	teta[1].last = 1194;
	teta[1].min = 0;
	teta[1].max = 2047;

	teta[2].type = 3;
	teta[2].first = 1023.5;
	teta[2].last = 1194;
	teta[2].min = 0;
	teta[2].max = 2047;

	v = 800;
	Fuzzification(&teta[0],v);
	Fuzzification(&teta[1],v);
	Fuzzification(&teta[2],v);

	printf("Teta\n");
	printf("Tipo da regra: %d\t",teta[0].type);
	printf("Coefs: %f , %f\t",teta[0].coefs[0],teta[0].coefs[1]);
	printf("GPA: %f\n",teta[0].GPA);

	printf("Tipo da regra: %d\t",teta[1].type);
	printf("Coefs: %f , %f\t",teta[1].coefs[0],teta[1].coefs[1]);
	printf("GPA: %f\n",teta[2].GPA);

	printf("Tipo da regra: %d\t",teta[2].type);
	printf("Coefs: %f , %f\t",teta[2].coefs[0],teta[2].coefs[1]);
	printf("GPA: %f\n",teta[0].GPA);

	printf("\n\n\n");

	fuzzyRule* omega = (fuzzyRule*)malloc(sizeof(fuzzyRule)*3);
	omega[0].type = 1;
	omega[0].min = -8;
	omega[0].max = 8;
	omega[0].first = -1;
	omega[0].last = 0;

	omega[1].type = 2;
	omega[1].min = -8;
	omega[1].max = 8;
	omega[1].first = -1;
	omega[1].last = 1;

	omega[2].type = 3;
	omega[2].min = -8;
	omega[2].max = 8;
	omega[2].first = 0;
	omega[2].last = 1;

	vw = 0.5;
	Fuzzification(&omega[0],vw);
	Fuzzification(&omega[1],vw);
	Fuzzification(&omega[2],vw);

	printf("Omega\n");
	printf("Tipo da regra: %d\t",omega[0].type);
	printf("Coefs: %f , %f\t",omega[0].coefs[0],omega[0].coefs[1]);
	printf("GPA: %f\n",omega[0].GPA);

	printf("Tipo da regra: %d\t",omega[1].type);
	printf("Coefs: %f , %f\t",omega[1].coefs[0],omega[1].coefs[1]);
	printf("GPA: %f\n",omega[1].GPA);

	printf("Tipo da regra: %d\t",omega[2].type);
	printf("Coefs: %f , %f\t",omega[2].coefs[0],omega[2].coefs[1]);
	printf("GPA: %f\n",omega[2].GPA);

	printf("\n\n");

	fuzzyRule* torq = (fuzzyRule*)malloc(sizeof(fuzzyRule)*5);
	torq[0].type = 1;
	torq[0].min = -255;
	torq[0].max = 255;
	torq[0].first = -170;
	torq[0].last = -170/3;
	torq[0].GPA = 0.0;

	torq[1].type = 2;
	torq[1].min = -255;
	torq[1].max = 255;
	torq[1].first = -85;
	torq[1].last = 0;
	torq[1].GPA = 0.0;

	torq[2].type = 2;
	torq[2].min = -255;
	torq[2].max = 255;
	torq[2].first = -85/3;
	torq[2].last = 85/3;
	torq[2].GPA = 0.0;

	torq[3].type = 2;
	torq[3].min = -255;
	torq[3].max = 255;
	torq[3].first = 0;
	torq[3].last = 85;
	torq[3].GPA = 0.0;

	torq[4].type = 3;
	torq[4].min = -255;
	torq[4].max = 255;
	torq[4].first = 170/3;
	torq[4].last = 170;
	torq[4].GPA = 0.0;

	Analysis(teta[0].GPA, teta[1].GPA, teta[2].GPA, omega[0].GPA, omega[1].GPA, omega[2].GPA, &torq.GPA[0], &torq.GPA[1], &torq.GPA[2], &torq.GPA[3], &torq.GPA[4]);

	valorResposta = Defuzzification(&torq[0], &torq[1], &torq[2], &torq[3], &torq[4]);


	printf("\n");
	printf("Torque a ser dado: %f", valorResposta);

	return 0;
}
