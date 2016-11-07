////////////////////////////////
// RTMaps SDK Component header
////////////////////////////////

#ifndef _Maps_Matching_H
#define _Maps_Matching_H

// Includes maps sdk library header
#include "maps.hpp"
#include "Match_Objects.h"
#include <math.h>

// Declares a new MAPSComponent child class
class MAPSMatching : public MAPSComponent 
{
	// Use standard header definition macro
	MAPS_COMPONENT_STANDARD_HEADER_CODE(MAPSMatching)
private :
	int timestamp;

	int numInputs = 2;
	bool readed[2];
	AUTO_Objects output_LaserAmpliatedBox;
	AUTO_Objects output_CameraAmpliatedBox;
	MAPSIOElt *_ioOutput;
	MAPSStreamedString str;
	AUTO_Objects* ArrayLaserObjects;
	AUTO_Objects* ArrayCameraObjects;
	MATCH_OBJECTS LaserMatched;
	MATCH_OBJECTS CameraMatched;
	void readInputs();
	void WriteOutputs();
	void printResults();
	void findMatches(AUTO_Objects* ArrayLaserObjects, AUTO_Objects* ArrayCameraObjects);
	bool BoxMatching(AUTO_Object Object1, AUTO_Object Object2);
	void copyBBox(BOUNDIG_BOX BBox, AUTO_Object * Output_ampliated);
	void calculateBoundingBox(AUTO_Object * Object);
	void trasladarBowndingBox(BOUNDIG_BOX * entrada, double x, double y);
	void ampliarBowndingBox(BOUNDIG_BOX * entrada, double x, double y);
	BOUNDIG_BOX finalBox(BOUNDIG_BOX original, BOUNDIG_BOX Lrotated, BOUNDIG_BOX Rrotated);
	void rotarBoundingBox(BOUNDIG_BOX *entrada, double angulo);
	void rotarPunto(B_POINT *punto, double angulo);
	void clear_Matched();


	//Pruebas
	void overlap(AUTO_Object objeto1, AUTO_Object objeto2);
	float compareArea(BOUNDIG_BOX BBox, BOUNDIG_BOX BBoxOriginal);
	int findID(int id_object, MATCH_OBJECTS vector);
	int findID(int id_object,int id_target, MATCH_OBJECTS vector);
	BOUNDIG_BOX calculateBBox(AUTO_Object obj);
};
#endif