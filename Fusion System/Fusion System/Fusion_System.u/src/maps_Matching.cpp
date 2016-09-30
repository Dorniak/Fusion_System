////////////////////////////////
// RTMaps SDK Component
////////////////////////////////

////////////////////////////////
// Purpose of this module :
////////////////////////////////

#include "maps_Matching.h"	// Includes the header of this component

const MAPSTypeFilterBase ValeoStructure = MAPS_FILTER_USER_STRUCTURE(AUTO_Objects); 
const MAPSTypeFilterBase matchedVector = MAPS_FILTER_USER_STRUCTURE(std::vector<bool>);
// Use the macros to declare the inputs
MAPS_BEGIN_INPUTS_DEFINITION(MAPSMatching)
    //MAPS_INPUT("iName",MAPS::FilterInteger32,MAPS::FifoReader)
	MAPS_INPUT("LaserObject", ValeoStructure, MAPS::FifoReader)
	MAPS_INPUT("CameraObject", ValeoStructure, MAPS::FifoReader)
MAPS_END_INPUTS_DEFINITION

// Use the macros to declare the outputs
MAPS_BEGIN_OUTPUTS_DEFINITION(MAPSMatching)
    //MAPS_OUTPUT("oName",MAPS::Integer32,NULL,NULL,1)
	MAPS_OUTPUT_USER_STRUCTURE("MatchedLaser", ValeoStructure)
	MAPS_OUTPUT_USER_STRUCTURE("MatchedCamera", ValeoStructure)
	MAPS_OUTPUT_USER_STRUCTURE("LaserObjects", ValeoStructure)
	MAPS_OUTPUT_USER_STRUCTURE("CameraObjects", ValeoStructure)
	//MAPS_OUTPUT_USER_STRUCTURES_VECTOR("foo_vector", IMARA_Object, 16)
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
		// Lectura de la variable (Además se filtran los obstáculos etc...)
	



    // Reports this information to the RTMaps console. You can remove this line if you know when Birth() is called in the component lifecycle.
    ReportInfo("Passing through Birth() method");
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
	LMatched.clear();
	CMatched.clear();
	ArrayLaserObjects = nullptr;
	ArrayCameraObjects = nullptr;
	str.Clear();
	if (DataAvailableInFIFO(Input("LaserObject")) || DataAvailableInFIFO(Input("CameraObject"))) {
		str << '\n' << "Objects detected";
		//Leer laser
		MAPSIOElt* elt = StartReading(Input("LaserObject"));
		ArrayLaserObjects = static_cast<AUTO_Objects*>(elt->Data());
		StopReading(Input("LaserObject"));
		//Leer camara
		MAPSIOElt* elt2;
		elt2 = StartReading(Input("CameraObject"));
		ArrayCameraObjects = static_cast<AUTO_Objects*>(elt2->Data());
		StopReading(Input("LaserObject"));
		str << '\n' << "Box Matchig "<< ArrayLaserObjects->number_of_objects <<" "<< ArrayCameraObjects->number_of_objects;
		findMatches(ArrayLaserObjects, ArrayCameraObjects);
		str << '\n' << "Imprimir coincidencias " << '\n';
		for (int printer = 0; printer < LMatched.size(); printer++) {
			str << " " << LMatched[printer];
		}
		str << '\n';
		for (int printer = 0; printer < CMatched.size(); printer++) {
			str << " " << CMatched[printer];
		}
		str << '\n' << "fin de impresion";
	}

	

    // Sleeps during 500 milliseconds (500000 microseconds).
	//This line will most probably have to be removed when you start programming your component.
	// Replace it with another blocking function. (StartReading?)
	ReportInfo(str);
	Rest(500000);
}

//De-initialization: Death() will be called once at diagram execution shutdown.
void MAPSMatching::Death()
{
    // Reports this information to the RTMaps console. You can remove this line if you know when Death() is called in the component lifecycle.
    ReportInfo("Passing through Death() method");
}

void MAPSMatching::findMatches(AUTO_Objects* ArrayLaserObjects, AUTO_Objects* ArrayCameraObjects)
{
	for (int i = 0; i < ArrayLaserObjects->number_of_objects; i++) {
		LMatched.push_back(false);
	}
	for (int j = 0; j < ArrayCameraObjects->number_of_objects; j++) {
		CMatched.push_back(false);
	}

	for (int i = 0; i < ArrayLaserObjects->number_of_objects; i++) {
		for (int j = 0; j < ArrayCameraObjects->number_of_objects; j++) {
			if (BoxMatching(ArrayLaserObjects->object[i], ArrayCameraObjects->object[j])) {
				LMatched[i] = true;
				CMatched[j] = true;
			}
		}
	}
}

bool MAPSMatching::BoxMatching(AUTO_Object Object1, AUTO_Object Object2)
{
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
	return false;
}
