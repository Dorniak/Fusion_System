////////////////////////////////
// RTMaps SDK Component header
////////////////////////////////

#ifndef _Maps_Predictor_H
#define _Maps_Predictor_H

// Includes maps sdk library header
#include "maps.hpp"
#include "auto_objects.h"
#include <math.h>

typedef struct {
	float32_t x;
	float32_t y;
	float32_t z;
} Point3D;

// Declares a new MAPSComponent child class
class MAPSPredictor : public MAPSComponent 
{
	// Use standard header definition macro
	MAPS_COMPONENT_STANDARD_HEADER_CODE(MAPSPredictor)
private :
	int framerate;
	float prueba;
	LARGE_INTEGER Frecuency, StartingTime, EndingTime;
	double ElapsedTime;
	MAPSStreamedString str;

	uint32_t timestamp;
	MAPSIOElt *_ioOutput;
	//Fill control of the structures
	bool predicted;
	bool updated[2];
	bool ready;
	bool completedL[2];
	bool completedC[2];
	bool firsttime;
	//Inputs
	AUTO_Objects* ArrayLaserObjects;
	AUTO_Objects* ArrayCameraObjects;
	AUTO_Objects LaserObjects[2];
	AUTO_Objects CameraObjects[2];

	AUTO_Objects LaserObjectsOutput, CameraObjectsOutput;

	//Functions
	void readInputs();
	void WriteOutputs();
	void predecir();
	int findPosition(AUTO_Objects objects, int id);
	void moveObstacle(AUTO_Object * obstacle, int timestamp);
};

#endif
