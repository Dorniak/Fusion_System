#pragma once

#include "Hipotesys.h"
#include <algorithm>
#define tamano 1000

using namespace std;

class VECTOR_INT
{
public:
	//VECTOR_INT();
	//~VECTOR_INT();

	void erase(int posicion);
	size_t size();
	int vector[tamano];

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
		
	VECTOR_INT vector[tamano];

	void clear();

	int size();

	void resize(int tam);
	
	void push_back(int input);
	void actualizarmaximos();
private:
	
	

};