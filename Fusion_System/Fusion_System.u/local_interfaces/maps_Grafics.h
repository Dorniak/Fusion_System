////////////////////////////////
// RTMaps SDK Component header
////////////////////////////////

#ifndef _Maps_Grafics_H
#define _Maps_Grafics_H


// Includes maps sdk library header
#include "maps.hpp"
#include "auto_objects.h"
#include <iostream>
#include <fstream>
#include <string>
#include "Common.h"


using namespace std;
// Declares a new MAPSComponent child class
class MAPSGrafics : public MAPSComponent 
{
	// Use standard header definition macro
	MAPS_COMPONENT_STANDARD_HEADER_CODE(MAPSGrafics)
private :
	string direction = dir;
	bool firstL = true;
	bool firstC = true;
	// Place here your specific methods and attributes
	MAPSIOElt* elt;
	MAPSStreamedString str;
	MAPSIOElt *_ioOutput;
	AUTO_Objects* ArrayLaserObjects_input;
	AUTO_Objects* ArrayCameraObjects_input;

	AUTO_Objects ArrayLaserObjects;
	AUTO_Objects ArrayCameraObjects;
	ofstream fLaser;
	ofstream fCamera;

	void readInputs();
	void WriteLaser();
	void WriteCamera();
};

#endif
