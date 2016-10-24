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

void VECTOR_INT::push_back(int input) {
	if (useful_tam == tamano)
	{
		throw 0;
	}
	else {
		vector[useful_tam] = input;
		useful_tam++;
	}
}

void VVECTOR_INT::clear() {
	for (int i = 0; i < useful_tam; i++)
	{
		useful_tam = 0;
	}
}

void VVECTOR_INT::push_back(int input)
{
	if (useful_tam < tamano) {
		useful_tam++;
	}
	else
	{
		throw 0;
	}
}

void VVECTOR_INT::actualizarmaximos()
{
	num_maximo = max(num_maximo, useful_tam);
	for (int i = 0; i < useful_tam; i++)
	{
		tam_maximo_interno = max((int)vector[i].size(), tam_maximo_interno);
	}
}

int VVECTOR_INT::size() 
{
	return useful_tam;
}

void VVECTOR_INT::resize(int tam)
{
	if (tam < tamano) {
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