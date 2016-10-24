#pragma once

#include "Hipotesys.h"
#include <algorithm>
#define tamanoVV 300
#define tamanoV 50

using namespace std;

class VECTOR_INT
{
public:
	int maximo = 0;
	void actualizarm();
	void erase(int posicion);
	size_t size();
	int vector[tamanoV];

	void clear();
	void push_back(int input);
private:
	int useful_tam;
};

class VVECTOR_INT
{
public:
	int useful_tam;
	int num_maximo = 0;
	int tam_maximo_interno = 0;
		
	VECTOR_INT vector[tamanoVV];

	void clear();

	int size();

	void resize(int tam);
	
	void push_back(int input);
	void actualizarmaximos();
private:
	
	

};