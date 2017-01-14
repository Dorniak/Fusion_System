////////////////////////////////
// RTMaps SDK Component
////////////////////////////////

////////////////////////////////
// Purpose of this module :
////////////////////////////////

#include "maps_GraficEst.h"	// Includes the header of this component

const MAPSTypeFilterBase ValeoStructure = MAPS_FILTER_USER_STRUCTURE(AUTO_Objects);
// Use the macros to declare the inputs
MAPS_BEGIN_INPUTS_DEFINITION(MAPSGraficEst)
MAPS_INPUT("EstObject", ValeoStructure, MAPS::FifoReader)
//MAPS_INPUT("iName",MAPS::FilterInteger32,MAPS::FifoReader)
MAPS_END_INPUTS_DEFINITION

// Use the macros to declare the outputs
MAPS_BEGIN_OUTPUTS_DEFINITION(MAPSGraficEst)
//MAPS_OUTPUT("oName",MAPS::Integer32,NULL,NULL,1)
MAPS_END_OUTPUTS_DEFINITION

// Use the macros to declare the properties
MAPS_BEGIN_PROPERTIES_DEFINITION(MAPSGraficEst)
//MAPS_PROPERTY("pName",128,false,false)
MAPS_END_PROPERTIES_DEFINITION

// Use the macros to declare the actions
MAPS_BEGIN_ACTIONS_DEFINITION(MAPSGraficEst)
//MAPS_ACTION("aName",MAPSGraficEst::ActionName)
MAPS_END_ACTIONS_DEFINITION

// Use the macros to declare this component (Grafics) behaviour
MAPS_COMPONENT_DEFINITION(MAPSGraficEst, "Grafics", "1.0", 128,
	MAPS::Threaded, MAPS::Threaded,
	-1, // Nb of inputs. Leave -1 to use the number of declared input definitions
	-1, // Nb of outputs. Leave -1 to use the number of declared output definitions
	-1, // Nb of properties. Leave -1 to use the number of declared property definitions
	-1) // Nb of actions. Leave -1 to use the number of declared action definitions

		//Initialization: Birth() will be called once at diagram execution startup.			  
	void MAPSGraficEst::Birth()
{
	direction = direction + std::to_string(idEst) + "_" + std::to_string(idEst) + "\\";
	_mkdir(direction.c_str());

	fEst.open(direction + "Laser_" + std::to_string(idEst) + ".m", ofstream::out);

	fEst << "%timestamp,x_rel,y_rel,x_sigma,y_sigma,speed,speed_x_rel,speed_y_rel,speed_x_sigma,speed_y_sigma" << '\n' << "LaserN=[";

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
void MAPSGraficEst::Core()
{
	readInputs();
}

//De-initialization: Death() will be called once at diagram execution shutdown.
void MAPSGraficEst::Death()
{
	fEst << "];";
	fEst.close();
}

void MAPSGraficEst::readInputs()
{
	MAPSInput* inputs[2] = { &Input("LaserObject"), &Input("CameraObject") };
	int inputThatAnswered;
	MAPSIOElt* ioeltin = StartReading(2, inputs, &inputThatAnswered);
	if (ioeltin == NULL) {
		return;
	}
	switch (inputThatAnswered)
	{
	case 0:
		ArrayEstObjects_input = static_cast<AUTO_Objects*>(ioeltin->Data());
		ArrayEstObjects = *ArrayEstObjects_input;
		WriteEst();
		break;
	default:
		break;
	}
}

void MAPSGraficEst::WriteEst()
{
	for (int i = 0; i < ArrayEstObjects.number_of_objects; i++)
	{
		if (ArrayEstObjects.object[i].id == idLaser)
		{
			if (!first)
			{
				fEst << ";" << '\n' << ArrayEstObjects.timestamp << "," << ArrayEstObjects.object[i].x_rel << "," << ArrayEstObjects.object[i].y_rel << "," << ArrayEstObjects.object[i].x_sigma
					<< "," << ArrayEstObjects.object[i].y_sigma << "," << ArrayEstObjects.object[i].speed_rel << "," << ArrayEstObjects.object[i].speed_x_rel << "," << ArrayEstObjects.object[i].speed_y_rel
					<< "," << ArrayEstObjects.object[i].speed_x_sigma << "," << ArrayEstObjects.object[i].speed_y_sigma;
			}
			else
			{
				fEst << '\n' << ArrayEstObjects.timestamp << "," << ArrayEstObjects.object[i].x_rel << "," << ArrayEstObjects.object[i].y_rel << "," << ArrayEstObjects.object[i].x_sigma
					<< "," << ArrayEstObjects.object[i].y_sigma << "," << ArrayEstObjects.object[i].speed_rel << "," << ArrayEstObjects.object[i].speed_x_rel << "," << ArrayEstObjects.object[i].speed_y_rel
					<< "," << ArrayEstObjects.object[i].speed_x_sigma << "," << ArrayEstObjects.object[i].speed_y_sigma;
				//fEst = false;
			}
		}
	}
}