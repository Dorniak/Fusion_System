#pragma once

#include "Hipotesys.h"
#include <algorithm>
#define tamanoVV 300
#define tamanoV 50

using namespace std;

class VECTOR_INT
{
public:
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
		
	VECTOR_INT vector[tamanoVV];

	void clear();

	int size();

	void resize(int tam);
	
	void push_back(int input);
};