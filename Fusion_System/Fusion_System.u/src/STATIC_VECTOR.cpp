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
	if (useful_tam == tamanoV)
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
	if (useful_tam < tamanoVV) {
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
	if (tam < tamanoVV) {
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

void NonAssociated::clear()
{
	for (int i = 0; i < number_objects; i++)
	{
		vector[i] = -1;
	}
	number_objects = 0;
}

int NonAssociated::size()
{
	return number_objects;
}

void NonAssociated::resize(int tam)
{
	for (int i = tam; i < number_objects; i++)
	{
		vector[i] = -1;
	}
	number_objects = tam;
}

void NonAssociated::erase(int pos)
{
	for (int i = pos; i < number_objects; i++)
	{
		vector[i] = vector[i + 1];
	}
	number_objects--;
	vector[number_objects] = -1;
}

void NonAssociated::push_back(int id)
{
	vector[number_objects] = id;
	number_objects++;
}

void AssociatedObjs::clear()
{
	for (int i = 0; i < AUTO_MAX_NUM_OBJECTS; i++)
	{
		clear(i);
	}
}

void AssociatedObjs::clear(int pos)
{
	vector[pos][0] = -1;//id Laser
	vector[pos][1] = -1;//id Camera
	vector[pos][2] = 0;//Score
}

int AssociatedObjs::size()
{
	return number_objects;
}

void AssociatedObjs::resize(int tam)
{
	for (int i = tam; i < number_objects; i++)
	{
		clear(i);
	}
	number_objects = tam;
}

void AssociatedObjs::erase(int pos)
{
	for (int i = pos; i < number_objects; i++)
	{
		vector[i][0] = vector[i + 1][0];
		vector[i][1] = vector[i + 1][1];
		vector[i][2] = vector[i + 1][2];
	}
	number_objects--;
	clear(number_objects);
}

Point3D::Point3D()
{
	x = -1;
	y = -1;
	z = -1;
}

Point3D::Point3D(float x, float y, float z)
{
	this->x = x;
	this->y = y;
	this->z = z;
}

Point2D::Point2D()
{
	x = 0;
	y = 0;
}

// Constructor uses default arguments to allow calling with zero, one,
// or two values.
Point2D::Point2D(float32_t x , float32_t y ) {
	this->x = x;
	this->y = y;
}

// Distance to another Point2D.  Pythagorean thm.
float32_t Point2D::dist(Point2D other) {
	float32_t xd = x - other.x;
	float32_t yd = y - other.y;
	return sqrt(xd*xd + yd*yd);
}

// Add or subtract two Point2Ds.
Point2D Point2D::add(Point2D b)
{
	return Point2D(x + b.x, y + b.y);
}
Point2D Point2D::sub(Point2D b)
{
	return Point2D(x - b.x, y - b.y);
}

// Move the existing Point2D.
void Point2D::move(float32_t a, float32_t b)
{
	x += a;
	y += b;
}
void Point2D::rote(double angle)
{
	x = (x * (float32_t)cos(angle)) - (y * (float32_t)sin(angle));
	y = (x * (float32_t)sin(angle)) + (y * (float32_t)cos(angle));
}


BOUNDIG_BOX::BOUNDIG_BOX()
{
	for (int i = 0; i < 4; i++)
	{
		point[i] = Point2D();;
	}
	
}
