////////////////////////////////
// RTMaps SDK Component header
////////////////////////////////

#ifndef _Maps_Join_H
#define _Maps_Join_H
#define MIN_SCORE 20
// Includes maps sdk library header
#include "maps.hpp"
#include <math.h>
#include <vector>
#include <array>
#include "Match_Objects.h"
#include "auto_objects.h"
#include "Hipotesys.h"
#include "Properties.h"
#include "STATIC_VECTOR.h"

// Declares a new MAPSComponent child class
class MAPSJoin : public MAPSComponent 
{
	// Use standard header definition macro
	MAPS_COMPONENT_STANDARD_HEADER_CODE(MAPSJoin)
private :
	//Structures
	MAPSIOElt *_ioOutput;
	MAPSStreamedString str;
	bool readed[4] = { false,false, false, false };
	int numInputs = 4;

	AUTO_Objects* input_Laser_Objects;
	AUTO_Objects* input_Camera_Objects;
	MATCH_OBJECTS* input_Laser_Matched;
	MATCH_OBJECTS* input_Camera_Matched;

	//Internal objects

	AUTO_Objects Laser_Objects;
	AUTO_Objects Camera_Objects;
	MATCH_OBJECTS Laser_Matched;
	MATCH_OBJECTS Camera_Matched;
	int score;
	//List with n rows n= number of Laser objects
	//3 columns 
	//First column = id Laser Object
	//Second column = id Camera Object
	//Third colum = score 
	
	vector<array<int, 3>>joined;
	//int joined[AUTO_MAX_NUM_OBJECTS][3];
	vector<int>nonLaserJoined;
	vector<int>nonCameraJoined;


private:
	//functions
	void ReadInputs();
	void ProcessData();
	void WriteOutputs();
	int calculateScore(int id_Laser, int id_Camera);
	int findPositionObject(int id, AUTO_Objects *objects);
	bool findAssociatedLaser(int id_Laser);
	bool findAssociatedCamera(int id_Camera);
	void cleanAssociatedList();
	void cleanStructures();
};

#endif
