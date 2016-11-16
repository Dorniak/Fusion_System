#include "auto_objects.h"
#include "STATIC_VECTOR.h"
typedef struct {
	int number_objects;//Numero de objetos del tipoque este definido
	int id[AUTO_MAX_NUM_OBJECTS];//lista con el id de cada objeto para no tener que mirarlo en la lista de objetos
	int number_matched[AUTO_MAX_NUM_OBJECTS];//Vector que indica numero de objetos asociados a cada uno
	int Matrix_matched[AUTO_MAX_NUM_OBJECTS][AUTO_MAX_NUM_OBJECTS][2];//Matriz con todos los objetos
	//0:id objeto camara
	//1:overlap [0,100]
}MATCH_OBJECTS;