////////////////////////////////
// RTMaps SDK Component header
////////////////////////////////

#ifndef _Maps_Estimation_H
#define _Maps_Estimation_H

// Includes maps sdk library header
#include "maps.hpp"
#include "auto_objects.h"
#include "STATIC_VECTOR.h"

// Declares a new MAPSComponent child class
class MAPSEstimation : public MAPSComponent 
{
	// Use standard header definition macro
	MAPS_COMPONENT_STANDARD_HEADER_CODE(MAPSEstimation)
private :
	bool readed[5] = { false,false, false, false ,false };
	int numInputs = 5;

	MAPSIOElt* elt;
	MAPSStreamedString str;
	MAPSIOElt *_ioOutput;
	AUTO_Objects* ArrayLaserObjects_input;
	AUTO_Objects* ArrayCameraObjects_input;

	AUTO_Objects ArrayLaserObjects;
	AUTO_Objects ArrayCameraObjects;

	AssociatedObjs* joined_input;
	NonAssociated* nonLaserJoined_input;
	NonAssociated* nonCameraJoined_input;

	AssociatedObjs joined;
	NonAssociated nonLaserJoined;
	NonAssociated nonCameraJoined;
	
	AUTO_Objects Estimation;
	vector<int[2]>IdL;//Ids de laser no asociados
	vector<int[2]>IdC;//Ids de camara no asociados
	vector<int[3]>LCAssociations;//Ids de camara no asociados

	void readInputs();
	void WriteOutputs();
	//TODO:Generador de ids

	void ProcessData();
	void Estimate();
	
};

#endif
