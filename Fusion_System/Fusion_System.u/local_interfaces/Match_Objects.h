#include "auto_objects.h"
typedef struct {
	int number_objects;//Numero de objetos del tipoque este definido
	int number_matched[AUTO_MAX_NUM_OBJECTS];//Vector que indica numero de objetos asociados a cada uno
	int Matrix_matched[AUTO_MAX_NUM_OBJECTS][AUTO_MAX_NUM_OBJECTS];//Matriz con todos los objetos
	//TODO::en pruebas
	int overlap[AUTO_MAX_NUM_OBJECTS][AUTO_MAX_NUM_OBJECTS][2];//% de coincidencia pos 0 % de coincidencia real pos 1
	//Primer parametro tamaño numer_objects 
	//Segundo parametro lista de identificadores asociados a cada objeto
}MATCH_OBJECTS;

typedef struct {
	float32_t x;
	float32_t y;
}B_POINT;

typedef struct {
	B_POINT point[4];
}BOUNDIG_BOX;