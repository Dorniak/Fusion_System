////////////////////////////////
// RTMaps SDK Component header
////////////////////////////////

#ifndef _Maps_Estimation_H
#define _Maps_Estimation_H

// Includes maps sdk library header
#include "maps.hpp"
#include "auto_objects.h"

// Declares a new MAPSComponent child class
class MAPSEstimation : public MAPSComponent 
{
	// Use standard header definition macro
	MAPS_COMPONENT_STANDARD_HEADER_CODE(MAPSEstimation)
private :
	MAPSIOElt* elt;
	MAPSStreamedString str;
	MAPSIOElt *_ioOutput;
	AUTO_Objects* ArrayLaserObjects;
	AUTO_Objects* ArrayCameraObjects;
	void readInputs();
	void WriteOutputs();
	
};

#endif
