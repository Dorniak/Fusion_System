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

NonAssociated::NonAssociated()
{
	number_objects = AUTO_MAX_NUM_OBJECTS;
	clear();
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

float Point3D::dist(Point3D other)
{
	Point3D result;
	result = this->sub(other);
	return result.module();
}

float Point3D::module()
{
	float module;
	module = sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
	return 0;
}

Point3D Point3D::sub(Point3D other)
{
	return Point3D(x - other.x, y - other.y, z - other.z);
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
float32_t Point2D::dist(Point2D other) 
{
	float32_t xd = x - other.x;
	float32_t yd = y - other.y;

	return sqrt(xd*xd + yd*yd);
}

float32_t Point2D::module()
{
	float32_t module;

	module = this->dist(Point2D(0, 0));

	return module;
}

// Add or subtract two Point2Ds.
//Point2D Point2D::add(Point2D b)
//{
//	return Point2D(x + b.x, y + b.y);
//}

Point2D Point2D::operator+(Point2D b)
{
	// TODO: insert return statement here
	return Point2D(x + b.x, y + b.y);
}

//Point2D Point2D::sub(Point2D b)
//{
//	return Point2D(x - b.x, y - b.y);
//}

Point2D Point2D::operator-(Point2D b)
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

BOUNDIG_BOX::BOUNDIG_BOX(AUTO_Object * object)
{
	point[0].x = object->bounding_box_x_rel[0];
	point[0].y = object->bounding_box_y_rel[0];
	point[1].x = object->bounding_box_x_rel[1];
	point[1].y = object->bounding_box_y_rel[1];
	point[2].x = object->bounding_box_x_rel[2];
	point[2].y = object->bounding_box_y_rel[2];
	point[3].x = object->bounding_box_x_rel[3];
	point[3].y = object->bounding_box_y_rel[3];
}

BOUNDIG_BOX::BOUNDIG_BOX(Point2D p, Point2D sigma)
{
	//1 2
	//0 3

	point[0] = Point2D(p.x - sigma.x, p.y + sigma.y);
	point[1] = Point2D(p.x + sigma.x, p.y + sigma.y);
	point[2] = Point2D(p.x + sigma.x, p.y - sigma.y);
	point[3] = Point2D(p.x - sigma.x, p.y - sigma.y);
}

BOUNDIG_BOX::BOUNDIG_BOX(Point2D a, Point2D b, Point2D c, Point2D d)
{
	point[0] = a;
	point[1] = b;
	point[2] = c;
	point[3] = d;
	this->reordenar();
}

void BOUNDIG_BOX::reordenar()
{
	Point2D aux;
	Point2D p[4];
	for (int i = 0; i < 4; i++)
	{
		p[i] = point[i];
		for (int j = 0; j < 4; j++)
		{
			switch (i) {
			case 0:
				// Code
				if (point[j].x < p[i].x && point[j].y > p[i].y)
				{
					p[i] = point[j];
				}
				break;
			case 1:
				// Code
				if (point[j].x > p[i].x && point[j].y > p[i].y)
				{
					p[i] = point[j];
				}
				break;
			case 2:
				// Code
				if (point[j].x > p[i].x && point[j].y < p[i].y)
				{
					p[i] = point[j];
				}
				break;
			case 3:
				// Code
				if (point[j].x < p[i].x && point[j].y < p[i].y)
				{
					p[i] = point[j];
				}
				break;
			default:
				// Code
				break;
			}
		}
	}
	for (int i = 0; i < 4; i++)
	{
		point[i] = p[i];
	}
}

void BOUNDIG_BOX::rote(double angle)
{
	point[0].rote(angle);
	point[1].rote(angle);
	point[2].rote(angle);
	point[3].rote(angle);
}

float32_t BOUNDIG_BOX::area()
{
	float32_t base, height;

	this->reordenar();
	base = point[0].dist(point[3]);
	height = point[0].dist(point[1]);

	return base*height;
}

BOUNDIG_BOX BOUNDIG_BOX::intersection(BOUNDIG_BOX other)
{
	float x_min, x_max, y_min, y_max;
	float x_min_Box(FLT_MAX), x_max_Box(-FLT_MAX), y_min_Box(FLT_MAX), y_max_Box(-FLT_MAX);
	float x_min_Other(FLT_MAX), x_max_Other(-FLT_MAX), y_min_Other(FLT_MAX), y_max_Other(-FLT_MAX);

	for (int i = 0; i < 4; i++)
	{
		x_min_Box = min(x_min_Box, point[i].x);
		x_max_Box = max(x_max_Box, point[i].x);
		y_min_Box = min(y_min_Box, point[i].y);
		y_max_Box = max(y_max_Box, point[i].y);

		x_min_Other = min(x_min_Other, other.point[i].x);
		x_max_Other = max(x_max_Other, other.point[i].x);
		y_min_Other = min(y_min_Other, other.point[i].y);
		y_max_Other = max(y_max_Other, other.point[i].y);
	}

	x_min = max(x_min_Box, x_min_Other);
	x_max = min(x_max_Box, x_max_Other);
	y_min = max(y_min_Box, y_min_Other);
	y_max = min(y_max_Box, y_max_Other);

	return BOUNDIG_BOX(Point2D(x_min, y_max), Point2D(x_max, y_max), Point2D(x_max, y_min), Point2D(x_min, y_min));
}

float32_t BOUNDIG_BOX::Union_area(BOUNDIG_BOX other)
{
	float32_t areaU;

	areaU = this->area() + other.area() - this->intersection(other).area();

	return areaU;
}

BOUNDIG_BOX_3D::BOUNDIG_BOX_3D()
{
	point[0] = Point3D(0, 0, 0);
	point[1] = Point3D(0, 0, 0);
	point[2] = Point3D(0, 0, 0);
	point[3] = Point3D(0, 0, 0);
	point[4] = Point3D(0, 0, 0);
	point[5] = Point3D(0, 0, 0);
	point[6] = Point3D(0, 0, 0);
	point[7] = Point3D(0, 0, 0);
}

BOUNDIG_BOX_3D::BOUNDIG_BOX_3D(Point3D a, Point3D b, Point3D c, Point3D d, Point3D e, Point3D f, Point3D g, Point3D h)
{
	point[0] = a;
	point[1] = b;
	point[2] = c;
	point[3] = d;
	point[4] = e;
	point[5] = f;
	point[6] = g;
	point[7] = h;
}

BOUNDIG_BOX_3D::BOUNDIG_BOX_3D(Point3D p, Point3D sigma)
{
	//abajo
	//1 2
	//0 3
	//arria
	//5 6
	//4 7
	point[0] = Point3D(p.x - sigma.x, p.y + sigma.y, p.z - sigma.z);
	point[1] = Point3D(p.x + sigma.x, p.y + sigma.y, p.z - sigma.z);
	point[2] = Point3D(p.x + sigma.x, p.y - sigma.y, p.z - sigma.z);
	point[3] = Point3D(p.x - sigma.x, p.y - sigma.y, p.z - sigma.z);
	point[4] = Point3D(p.x - sigma.x, p.y + sigma.y, p.z + sigma.z);
	point[5] = Point3D(p.x + sigma.x, p.y + sigma.y, p.z + sigma.z);
	point[6] = Point3D(p.x + sigma.x, p.y - sigma.y, p.z + sigma.z);
	point[7] = Point3D(p.x - sigma.x, p.y - sigma.y, p.z + sigma.z);
}

float BOUNDIG_BOX_3D::volumen()
{
	float edge1, edge2, edge3;
	float volum;
	edge1 = point[0].dist(point[1]);
	edge2 = point[0].dist(point[3]);
	edge3 = point[0].dist(point[5]);
	volum = edge1 * edge2 * edge3;

	return volum;
}

BOUNDIG_BOX_3D BOUNDIG_BOX_3D::intersection(BOUNDIG_BOX_3D other)
{
	float x_min, x_max, y_min, y_max, z_min, z_max;
	float x_min_Box(FLT_MAX), x_max_Box(-FLT_MAX), y_min_Box(FLT_MAX), y_max_Box(-FLT_MAX), z_min_Box(FLT_MAX), z_max_Box(-FLT_MAX);
	float x_min_Other(FLT_MAX), x_max_Other(-FLT_MAX), y_min_Other(FLT_MAX), y_max_Other(-FLT_MAX), z_min_Other(FLT_MAX), z_max_Other(-FLT_MAX);

	for (int i = 0; i < 8; i++)
	{
		x_min_Box = min(x_min_Box, point[i].x);
		x_max_Box = max(x_max_Box, point[i].x);
		y_min_Box = min(y_min_Box, point[i].y);
		y_max_Box = max(y_max_Box, point[i].y);
		z_min_Box = min(z_min_Box, point[i].z);
		z_max_Box = max(z_max_Box, point[i].z);

		x_min_Other = min(x_min_Other, other.point[i].x);
		x_max_Other = max(x_max_Other, other.point[i].x);
		y_min_Other = min(y_min_Other, other.point[i].y);
		y_max_Other = max(y_max_Other, other.point[i].y);
		z_min_Other = min(z_min_Other, other.point[i].z);
		z_max_Other = max(z_max_Other, other.point[i].z);
	}


	x_min = min(x_min_Box, x_min_Other);
	x_max = max(x_max_Box, x_max_Other);
	y_min = min(y_min_Box, y_min_Other);
	y_max = max(y_max_Box, y_max_Other);
	z_min = min(z_min_Box, z_min_Other);
	z_max = max(z_max_Box, z_max_Other);
	

	return BOUNDIG_BOX_3D(Point3D(x_min,y_max,z_min), Point3D(x_max, y_max, z_min), Point3D(x_max, y_min, z_min), Point3D(x_min, y_min, z_min), Point3D(x_min, y_max, z_max), Point3D(x_max, y_max, z_max), Point3D(x_max, y_min, z_max), Point3D(x_min, y_min, z_max));
}

float BOUNDIG_BOX_3D::Union_Volumen(BOUNDIG_BOX_3D other)
{
	float volumenU;
	
	volumenU = this->volumen() + other.volumen() - this->intersection(other).volumen();
	
	return volumenU;
}
