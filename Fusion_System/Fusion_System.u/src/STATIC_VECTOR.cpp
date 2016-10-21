#include "STATIC_VECTOR.h"

size_t VECTOR_INT::size()
{
	return useful_tam;
}

void VECTOR_INT::clear() {
	for (int i = 0; i < useful_tam; i++)
	{
		vector[i] = 0;
	}
	useful_tam = 0;
}

void VECTOR_INT::erase(int posicion)
{
	for (int posicion = 0; posicion < useful_tam; posicion++)
	{
		vector[posicion] = vector[posicion + 1];
	}
	useful_tam--;
}

void VVECTOR_INT::clear() {
	for (int i = 0; i < useful_tam; i++)
	{
		useful_tam = 0;
	}
}

void VECTOR_INT::push_back(int input) {
	if (useful_tam == tamaño)
	{
		throw 0;
	}
	else {
		vector[useful_tam] = input;
		useful_tam++;
	}
}

void VVECTOR_INT::push_back(int input)
{
	if (useful_tam < tamaño) {
		useful_tam++;
	}
	else
	{
		throw 0;
	}
}

int VVECTOR_INT::size() 
{
	return useful_tam;
}

void VVECTOR_INT::resize(int tam)
{
	if (tam < tamaño) {
		while (useful_tam < tam)
		{
			vector[useful_tam].clear();
			useful_tam++;
		}
	}
	else
	{
		throw 0;
	}
}
