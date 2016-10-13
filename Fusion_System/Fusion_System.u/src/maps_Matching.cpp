////////////////////////////////
// RTMaps SDK Component
////////////////////////////////

////////////////////////////////
// Purpose of this module :
////////////////////////////////

#include "maps_Matching.h"	// Includes the header of this component

const MAPSTypeFilterBase ValeoStructure = MAPS_FILTER_USER_STRUCTURE(AUTO_Objects); 
// Use the macros to declare the inputs
MAPS_BEGIN_INPUTS_DEFINITION(MAPSMatching)
    //MAPS_INPUT("iName",MAPS::FilterInteger32,MAPS::FifoReader)
	MAPS_INPUT("LaserObject", ValeoStructure, MAPS::FifoReader)
	MAPS_INPUT("CameraObject", ValeoStructure, MAPS::FifoReader)
MAPS_END_INPUTS_DEFINITION

// Use the macros to declare the outputs
MAPS_BEGIN_OUTPUTS_DEFINITION(MAPSMatching)
    //MAPS_OUTPUT("oName",MAPS::Integer32,NULL,NULL,1)
	MAPS_OUTPUT_USER_STRUCTURE("LaserObjects", AUTO_Objects)
	MAPS_OUTPUT_USER_STRUCTURE("CameraObjects", AUTO_Objects)
	MAPS_OUTPUT_USER_STRUCTURE("MatchedLaser", MATCH_OBJECTS)
	MAPS_OUTPUT_USER_STRUCTURE("MatchedCamera", MATCH_OBJECTS)
MAPS_END_OUTPUTS_DEFINITION

// Use the macros to declare the properties
MAPS_BEGIN_PROPERTIES_DEFINITION(MAPSMatching)
    //MAPS_PROPERTY("pName",128,false,false)
MAPS_END_PROPERTIES_DEFINITION

// Use the macros to declare the actions
MAPS_BEGIN_ACTIONS_DEFINITION(MAPSMatching)
    //MAPS_ACTION("aName",MAPSMatching::ActionName)
MAPS_END_ACTIONS_DEFINITION

// Use the macros to declare this component (Matching) behaviour
MAPS_COMPONENT_DEFINITION(MAPSMatching,"Matching","1.0",128,
			  MAPS::Threaded,MAPS::Threaded,
			  -1, // Nb of inputs. Leave -1 to use the number of declared input definitions
			  -1, // Nb of outputs. Leave -1 to use the number of declared output definitions
			  -1, // Nb of properties. Leave -1 to use the number of declared property definitions
			  -1) // Nb of actions. Leave -1 to use the number of declared action definitions

//Initialization: Birth() will be called once at diagram execution startup.			  
void MAPSMatching::Birth()
{
	
}

//ATTENTION: 
//	Make sure there is ONE and ONLY ONE blocking function inside this Core method.
//	Consider that Core() will be called inside an infinite loop while the diagram is executing.
//	Something similar to: 
//		while (componentIsRunning) {Core();}
//
//	Usually, the one and only blocking function is one of the following:
//		* StartReading(MAPSInput& input); //Data request on a single BLOCKING input. A "blocking input" is an input declared as FifoReader, LastOrNextReader, Wait4NextReader or NeverskippingReader (declaration happens in MAPS_INPUT: see the beginning of this file). A SamplingReader input is non-blocking: StartReading will not block with a SamplingReader input.
//		* StartReading(int nCount, MAPSInput* inputs[], int* inputThatAnswered, int nCountEvents = 0, MAPSEvent* events[] = NULL); //Data request on several BLOCKING inputs.
//		* SynchroStartReading(int nb, MAPSInput** inputs, MAPSIOElt** IOElts, MAPSInt64 synchroTolerance = 0, MAPSEvent* abortEvent = NULL); // Synchronized reading - waiting for samples with same or nearly same timestamps on several BLOCKING inputs.
//		* Wait(MAPSTimestamp t); or Rest(MAPSDelay d); or MAPS::Sleep(MAPSDelay d); //Pauses the current thread for some time. Can be used for instance in conjunction with StartReading on a SamplingReader input (in which case StartReading is not blocking).
//		* Any blocking grabbing function or other data reception function from another API (device driver,etc.). In such case, make sure this function cannot block forever otherwise it could freeze RTMaps when shutting down diagram.
//**************************************************************************/
//	In case of no blocking function inside the Core, your component will consume 100% of a CPU.
//  Remember that the StartReading function used with an input declared as a SamplingReader is not blocking.
//	In case of two or more blocking functions inside the Core, this is likely to induce synchronization issues and data loss. (Ex: don't call two successive StartReading on FifoReader inputs.)
/***************************************************************************/
void MAPSMatching::Core()
{
	clear_Matched();
	ArrayLaserObjects = nullptr;
	ArrayCameraObjects = nullptr;
	str.Clear();
	//Read input objects
	readInputs();

	str << '\n' << "Box Matchig " << ArrayLaserObjects->number_of_objects << " " << ArrayCameraObjects->number_of_objects;

	//Buscamos objetos en el gating window de cada obstaculo
	//We look for objects inside the gating window of each object
	findMatches(ArrayLaserObjects, ArrayCameraObjects);
	printResults();
	WriteOutputs();
	//ReportInfo(str);
}

//De-initialization: Death() will be called once at diagram execution shutdown.
void MAPSMatching::Death()
{
}

void MAPSMatching::readInputs()
{
	while (!DataAvailableInFIFO(Input("LaserObject")) || !DataAvailableInFIFO(Input("CameraObject"))) {}
	str << '\n' << "Objects detected";
	//Leer laser
	if (DataAvailableInFIFO(Input("LaserObject"))) {
		elt = StartReading(Input("LaserObject"));
		ArrayLaserObjects = static_cast<AUTO_Objects*>(elt->Data());
		StopReading(Input("LaserObject"));
	}
	//Leer camara
	if (DataAvailableInFIFO(Input("CameraObject"))) {
		elt = StartReading(Input("CameraObject"));
		ArrayCameraObjects = static_cast<AUTO_Objects*>(elt->Data());
		StopReading(Input("CameraObject"));
	}
}

void MAPSMatching::WriteOutputs()
{
	_ioOutput = StartWriting(Output("LaserObjects"));
	AUTO_Objects &list = *static_cast<AUTO_Objects*>(_ioOutput->Data());
	list = *ArrayLaserObjects;
	StopWriting(_ioOutput);

	_ioOutput = StartWriting(Output("CameraObjects"));
	AUTO_Objects &list2 = *static_cast<AUTO_Objects*>(_ioOutput->Data());
	list2 = *ArrayCameraObjects;
	StopWriting(_ioOutput);

	_ioOutput = StartWriting(Output("MatchedLaser"));
	MATCH_OBJECTS &list3 = *static_cast<MATCH_OBJECTS*>(_ioOutput->Data());
	list3 = LaserMatched;
	StopWriting(_ioOutput);

	_ioOutput = StartWriting(Output("MatchedCamera"));
	MATCH_OBJECTS &list4 = *static_cast<MATCH_OBJECTS*>(_ioOutput->Data());
	list4 = CameraMatched;
	StopWriting(_ioOutput);
}

void MAPSMatching::printResults()
{
	str << '\n' << "Imprimir coincidencias " << '\n';
	str << '\n' << "Laser" << '\n';
	for (int printer = 0; printer < LaserMatched.number_objects; printer++) {
		str << "ID: " << ArrayLaserObjects->object[printer].id << '\n';
		for (int printer2 = 0; printer2 < LaserMatched.number_matched[printer]; printer2++) {
			str << " " << LaserMatched.Matrix_matched[printer][printer2];
		}
		str << '\n';
	}
	str << '\n';
	str << '\n' << "Camara" << '\n';
	for (int printer = 0; printer < CameraMatched.number_objects; printer++) {
		str << "ID: " << ArrayCameraObjects->object[printer].id << '\n';
		for (int printer2 = 0; printer2 < CameraMatched.number_matched[printer]; printer2++) {
			str << " " << CameraMatched.Matrix_matched[printer][printer2];
		}
		str << '\n';
	}

	str << '\n' << "fin de impresion";
}

void MAPSMatching::findMatches(AUTO_Objects* ArrayLaserObjects, AUTO_Objects* ArrayCameraObjects)
{
	LaserMatched.number_objects = ArrayLaserObjects->number_of_objects;
	CameraMatched.number_objects = ArrayCameraObjects->number_of_objects;
	for (int i = 0; i < ArrayLaserObjects->number_of_objects; i++) {
		for (int j = 0; j < ArrayCameraObjects->number_of_objects; j++) {
			if (BoxMatching(ArrayLaserObjects->object[i], ArrayCameraObjects->object[j])) {
				
				LaserMatched.Matrix_matched[i][LaserMatched.number_matched[i]] = ArrayCameraObjects->object[j].id;
				LaserMatched.number_matched[i]++;
			}
		}
	}
	//Comparar cada obstaculo del camara con cada uno de la laser
	for (int i = 0; i < ArrayCameraObjects->number_of_objects; i++) {
		for (int j = 0; j < ArrayLaserObjects->number_of_objects; j++) {
			if (BoxMatching(ArrayLaserObjects->object[j], ArrayCameraObjects->object[i])) {
				CameraMatched.Matrix_matched[i][CameraMatched.number_matched[i]] = ArrayLaserObjects->object[j].id;
				CameraMatched.number_matched[i]++;
			}
		}
	}
}

bool MAPSMatching::BoxMatching(AUTO_Object Object1, AUTO_Object Object2)
{
	/*
	for (int i = 0; i < 4; i++) {
		if (Object1.bounding_box_x_rel[i] >= Object2.bounding_box_x_rel[0] || Object1.bounding_box_x_rel[i] >= Object2.bounding_box_x_rel[3]) {
			if (Object1.bounding_box_x_rel[i] <= Object2.bounding_box_x_rel[0] || Object1.bounding_box_x_rel[i] <= Object2.bounding_box_x_rel[3]) {
				if (Object1.bounding_box_y_rel[i] >= Object2.bounding_box_y_rel[0] || Object1.bounding_box_y_rel[i] >= Object2.bounding_box_y_rel[1]) {
					if (Object1.bounding_box_y_rel[i] <= Object2.bounding_box_y_rel[2] || Object1.bounding_box_y_rel[i] <= Object2.bounding_box_y_rel[3]) {
						return true;
					}
				}
			}
		}
	}

	return false;*/
	double tolerance = Object1.distance *0.02;
	double tolerance_x_1 = Object1.x_sigma;
	double tolerance_y_1 = Object1.y_sigma;
	double tolerance_x_2 = Object2.x_sigma;
	double tolerance_y_2 = Object2.y_sigma;
	double x_max(Object1.bounding_box_x_rel[0]), x_min(Object1.bounding_box_x_rel[0]), y_max(Object1.bounding_box_y_rel[0]), y_min(Object1.bounding_box_y_rel[0]);
	//Paralelizar bounding box del objeto 1
	for (int i = 1; i < 4; i++) {
		if (x_max < Object1.bounding_box_x_rel[i]) x_max = Object1.bounding_box_x_rel[i];
		if (x_min > Object1.bounding_box_x_rel[i]) x_min = Object1.bounding_box_x_rel[i];
		if (y_max < Object1.bounding_box_y_rel[i]) y_max = Object1.bounding_box_y_rel[i];
		if (y_min > Object1.bounding_box_y_rel[i]) y_min = Object1.bounding_box_y_rel[i];
	}
	//Aumentar bounding box del objeto 1
	x_min -= tolerance_x_1;
	x_max += tolerance_x_1;
	y_min -= tolerance_y_1;
	y_max += tolerance_y_1;
	//Comparar las bounding box ampiadas
	for (int i = 0; i < 4; i++) {
		if (Object2.bounding_box_x_rel[i] + tolerance_x_2 > x_min && Object2.bounding_box_x_rel[i] - tolerance_x_2 < x_max) {
			if (Object2.bounding_box_y_rel[i] + tolerance_y_2 > y_min && Object2.bounding_box_y_rel[i] - tolerance_y_2 < y_max) {
				return true;
			}
		}
	}
	return false;
}

void MAPSMatching::clear_Matched()
{
	for (int i = 0; i < AUTO_MAX_NUM_OBJECTS; i++) {
		for (int j = 0; j < AUTO_MAX_NUM_OBJECTS; j++) {
			LaserMatched.Matrix_matched[i][j] = 0;
			CameraMatched.Matrix_matched[i][j] = 0;
		}
		LaserMatched.number_matched[i] = 0;
		CameraMatched.number_matched[i] = 0;
	}
	LaserMatched.number_objects = 0;
	CameraMatched.number_objects = 0;
}