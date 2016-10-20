////////////////////////////////
// RTMaps SDK Component header
////////////////////////////////

#ifndef _Maps_synchronizer_H
#define _Maps_synchronizer_H

// Includes maps sdk library header
#include "maps.hpp"
#include "auto_objects.h"

// Declares a new MAPSComponent child class
class MAPSsynchronizer : public MAPSComponent 
{
	// Use standard header definition macro
	MAPS_COMPONENT_STANDARD_HEADER_CODE(MAPSsynchronizer)
private :
	MAPSIOElt* elt;
	MAPSIOElt *_ioOutput;

	AUTO_Objects* Object_I;

	AUTO_Objects* Result;

	void readInputs();
	void writeOutputs();
	// Place here your specific methods and attributes
};

#endif
