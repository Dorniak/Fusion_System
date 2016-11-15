#include "auto_objects.h"
#include "STATIC_VECTOR.h"
typedef struct {
	int number_objects;//Numero de objetos del tipoque este definido
	int id[AUTO_MAX_NUM_OBJECTS];//lista con el id de cada objeto para no tener que mirarlo en la lista de objetos
	int number_matched[AUTO_MAX_NUM_OBJECTS];//Vector que indica numero de objetos asociados a cada uno
	int Matrix_matched[AUTO_MAX_NUM_OBJECTS][AUTO_MAX_NUM_OBJECTS][2];//Matriz con todos los objetos
	//0:id objeto camara
	//1:overlap [0,100]
	//TODO::en pruebas
	//float overlap[AUTO_MAX_NUM_OBJECTS][AUTO_MAX_NUM_OBJECTS][2];//% de coincidencia pos 0 % de coincidencia real pos 1
	//Primer parametro tamaño numer_objects 
	//Segundo parametro lista de identificadores asociados a cada objeto
}MATCH_OBJECTS;