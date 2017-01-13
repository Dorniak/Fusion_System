////////////////////////////////
// RTMaps SDK Component header
////////////////////////////////

#ifndef _Maps_GraficEst_H
#define _Maps_GraficEst_H

// Includes maps sdk library header
#include "maps.hpp"
#include "auto_objects.h"
#include <iostream>
#include <fstream>
#include <string>
#include "Common.h"
#include <direct.h>

using namespace std;
// Declares a new MAPSComponent child class
class MAPSGraficEst : public MAPSComponent 
{
	// Use standard header definition macro
	MAPS_COMPONENT_STANDARD_HEADER_CODE(MAPSGraficEst)
private :
	// Place here your specific methods and attributes
	string direction = dir;
	bool first = true;
	// Place here your specific methods and attributes
	MAPSIOElt* elt;
	MAPSStreamedString str;
	MAPSIOElt *_ioOutput;
	AUTO_Objects* ArrayEstObjects_input;

	AUTO_Objects ArrayEstObjects;
	ofstream fEst;

	void readInputs();
	void WriteEst();
};

#endif
