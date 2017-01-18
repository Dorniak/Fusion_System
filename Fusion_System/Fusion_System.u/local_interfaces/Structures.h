#pragma once

#include "Hipotesys.h"
#include <algorithm>
#include "auto_objects.h"
#define tamanoVV 300
#define tamanoV 50

using namespace std;

class AssociatedObjs
{
public:
	int number_objects = 0;
	int vector[AUTO_MAX_NUM_OBJECTS][3];//id_Laser id_Camera Score
	void clear();
	void clear(int pos);
	int size();
	void resize(int tam);
	void erase(int pos);
};

class NonAssociated
{
public:
	NonAssociated();
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
	float dist(Point3D other);
	float module();
	Point3D sub(Point3D other);
};

class Point2D 
{
public:
	float32_t x, y;
	Point2D();
	Point2D(float32_t x, float32_t y);
	float32_t dist(Point2D other);
	float32_t module();
	Point2D operator+(Point2D b);
	Point2D operator-(Point2D b);
	void move(float32_t a, float32_t b);
	void rote(double angle);
};

class BOUNDIG_BOX
{
public:
	Point2D point[4];
	BOUNDIG_BOX();
	BOUNDIG_BOX(AUTO_Object * object);
	BOUNDIG_BOX(Point2D p, Point2D sigma);
	BOUNDIG_BOX(Point2D a, Point2D b, Point2D c, Point2D d);
	void reordenar();
	void rote(double angle);
	float32_t area();
	BOUNDIG_BOX intersection(BOUNDIG_BOX other);
	float32_t Union_area(BOUNDIG_BOX other);
	BOUNDIG_BOX ampliate(Point2D sigma);
	bool isInside(Point2D P);
};

class BOUNDIG_BOX_3D
{
public:
	Point3D point[8];
	BOUNDIG_BOX_3D();
	BOUNDIG_BOX_3D(Point3D a, Point3D b, Point3D c, Point3D d, Point3D e, Point3D f, Point3D g, Point3D h);
	BOUNDIG_BOX_3D(Point3D p, Point3D sigma);
	float volumen();
	BOUNDIG_BOX_3D intersection(BOUNDIG_BOX_3D other);
	float Union_Volumen(BOUNDIG_BOX_3D other);
};

typedef struct {
	int number_objects;//Numero de objetos del tipoque este definido
	int id[AUTO_MAX_NUM_OBJECTS];//lista con el id de cada objeto para no tener que mirarlo en la lista de objetos
	int number_matched[AUTO_MAX_NUM_OBJECTS];//Vector que indica numero de objetos asociados a cada uno
	int Matrix_matched[AUTO_MAX_NUM_OBJECTS][AUTO_MAX_NUM_OBJECTS][2];//Matriz con todos los objetos
																	  //0:id objeto camara
																	  //1:overlap [0,100]
}MATCH_OBJECTS;