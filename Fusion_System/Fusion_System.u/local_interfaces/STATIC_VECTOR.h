#pragma once

#include "Hipotesys.h"
#include <algorithm>
#include "auto_objects.h"
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


class AssociatedObjs
{
public:
	int number_objects = 0;
	int vector[AUTO_MAX_NUM_OBJECTS][3];
	void clear();
	void clear(int pos);
	int size();
	void resize(int tam);
	void erase(int pos);
};

class NonAssociated
{
public:
	int number_objects = 0;
	int vector[AUTO_MAX_NUM_OBJECTS];
	void clear();
	int size();
	void resize(int tam);
	void erase(int pos);
	void push_back(int id);
};

class Point3D
{
public:
	Point3D();
	Point3D(float x, float y, float z);
	float x, y, z;
private:
	
};
