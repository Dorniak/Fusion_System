#pragma once

#include "Hipotesys.h"
#define tama�o 10000
#define max_size 5000

class VECTOR_INT
{
public:
	//VECTOR_INT();
	//~VECTOR_INT();

	void erase(int posicion);
	size_t size();
	int vector[tama�o];

	void clear();
	void push_back(int input);
private:
	int useful_tam;
};

class VVECTOR_INT
{
public:
	int useful_tam;

	VECTOR_INT vector[tama�o];

	void clear();

	int size();

	void resize(int tam);
	
	void push_back(int input);

};