////////////////////////////////
// RTMaps SDK Component header
////////////////////////////////
/// 
#ifndef _Maps_Grafics_H
#define _Maps_Grafics_H

// Includes maps sdk library header
#include "maps.hpp"
#include "auto_objects.h"
#include <iostream>
#include <fstream>
#include <string>
#include <direct.h>
#include "Common.h"

using namespace std;
// Declares a new MAPSComponent child class
class MAPSGraficResult : public MAPSComponent 
{

	// Use standard header definition macro
	MAPS_COMPONENT_STANDARD_HEADER_CODE(MAPSGraficResult)
private:
	string direction = dir;
	// Place here your specific methods and attributes
	bool firstL = true;
	bool firstC = true;
	bool firstE = true;
	bool readed[2] = { false,false };
	int numInputs = 2;
	MAPSIOElt* elt;
	MAPSStreamedString str;
	MAPSIOElt *_ioOutput;
	AUTO_Objects* ArrayLaserObjects_input;
	AUTO_Objects* ArrayCameraObjects_input;

	AUTO_Objects ArrayLaserObjects;
	AUTO_Objects ArrayCameraObjects;

	ofstream fLaser;
	ofstream fCamera;
	ofstream fEstimation;

	void readInputs();
	void WriteLaser();
	void WriteCamera();
	void WriteResult();
	float32_t calcParam(float32_t paramL, float32_t sigmaL, float32_t paramC, float32_t sigmaC);
	float32_t calcSigma(float32_t sigmaL, float32_t sigmaC);

};

#endif
