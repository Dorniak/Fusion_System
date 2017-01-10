////////////////////////////////
// RTMaps SDK Component header
////////////////////////////////

#ifndef _Maps_Estimation_H
#define _Maps_Estimation_H

// Includes maps sdk library header
#include "maps.hpp"
#include "auto_objects.h"
#include "STATIC_VECTOR.h"
#include <vector>
#include <array>

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
	vector<array<int, 3>>LCAssociations;//Ids de camara no asociados
	int lastID = 0;
	void printVectors();
	void readInputs();
	void ProcessData();
	void WriteOutputs();
	void Estimate();
	void shortVectorLCA(vector<array<int, 3>> vect);
	void shortObjects(AUTO_Objects * objects);
	int generateIdLas(int id);
	int generateIdCam(int id);
	int generateIdLC(int idL, int idC);
	int findAssoc(int idL, int idC);//Return position
	AUTO_Object findLaserObj(int id);
	AUTO_Object findCameraObj(int id);

	AUTO_Object calculateObj(AUTO_Object * objL, AUTO_Object * objC);
	void EstimateParameter(float32_t paramA, float32_t sigmaA, float32_t paramB, float32_t sigmaB, float32_t * paramResult, float32_t * sigmaResult);
	float32_t calcParam(float32_t paramL, float32_t sigmaL, float32_t paramC, float32_t sigmaC);
	float32_t calcSigma(float32_t sigmaL, float32_t sigmaC);
};

#endif
