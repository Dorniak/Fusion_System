////////////////////////////////
// RTMaps SDK Component header
////////////////////////////////

#ifndef _Maps_Join_H
#define _Maps_Join_H
#define MIN_SCORE 20

//sum = 1
#define weight_type 0.2
#define weight_pos 0.4
#define weight_dis 0
#define weight_size 0
#define weight_speed 0
#define weight_accel 0
#define weight_over 0.4




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
	//Para controlar ambiguedades
	//Matriz con el score de todas las asociaciones y su id
	int MatrixOfAssociations[AUTO_MAX_NUM_OBJECTS][AUTO_MAX_NUM_OBJECTS][2];
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
	
	AssociatedObjs joined;
	NonAssociated nonLaserJoined;
	NonAssociated nonCameraJoined;


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
	void addAssociation(int posLaser, int idCam, int score);
	void shortMatrixAssociations();
	void selectAssociations();
	void cleanAssociationMatrix();
	bool findAmbiguities();
	int findAmbiguities(int pos);
	void selectNextAssociation(int pos);
	bool lastCheck();
	bool IsAssociated(int id);
	float calcScoreType(AUTO_Object * Object_Laser, AUTO_Object * Object_Camera);//[0,100]
	float calcScorePos(AUTO_Object * Object_Laser, AUTO_Object * Object_Camera);//[0,100]
	float calcScoreDis(AUTO_Object * Object_Laser, AUTO_Object * Object_Camera);//[0,100]
	float calcScoreSize(AUTO_Object * Object_Laser, AUTO_Object * Object_Camera);//[0,100]
	float calcScoreSpeed(AUTO_Object * Object_Laser, AUTO_Object * Object_Camera);//[0,100]
	float calcScoreAccel(AUTO_Object * Object_Laser, AUTO_Object * Object_Camera);//[0,100]
	float getOverlapScore(AUTO_Object * Object_Laser, AUTO_Object * Object_Camera);//[0,100]
	int findID(int id_object, MATCH_OBJECTS * vector);
	int findID(int id_object, int id_target, MATCH_OBJECTS * vector);
	bool compatibles(AUTO_OBJECT_CLASS classL, AUTO_OBJECT_CLASS classC);
};

#endif
