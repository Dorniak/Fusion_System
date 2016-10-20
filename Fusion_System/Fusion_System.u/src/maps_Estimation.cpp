////////////////////////////////
// RTMaps SDK Component
////////////////////////////////

////////////////////////////////
// Purpose of this module :
////////////////////////////////

#include "maps_Estimation.h"	// Includes the header of this component
const MAPSTypeFilterBase ValeoStructure = MAPS_FILTER_USER_STRUCTURE(AUTO_Objects);

// Use the macros to declare the inputs
MAPS_BEGIN_INPUTS_DEFINITION(MAPSEstimation)
    //MAPS_INPUT("iName",MAPS::FilterInteger32,MAPS::FifoReader)
	MAPS_INPUT("LaserObject", ValeoStructure, MAPS::FifoReader)
	MAPS_INPUT("CameraObject", ValeoStructure, MAPS::FifoReader)
MAPS_END_INPUTS_DEFINITION

// Use the macros to declare the outputs
MAPS_BEGIN_OUTPUTS_DEFINITION(MAPSEstimation)
    //MAPS_OUTPUT("oName",MAPS::Integer32,NULL,NULL,1)
	MAPS_OUTPUT_USER_STRUCTURE("Output_estimation", AUTO_Objects)
MAPS_END_OUTPUTS_DEFINITION

// Use the macros to declare the properties
MAPS_BEGIN_PROPERTIES_DEFINITION(MAPSEstimation)
    //MAPS_PROPERTY("pName",128,false,false)
MAPS_END_PROPERTIES_DEFINITION

// Use the macros to declare the actions
MAPS_BEGIN_ACTIONS_DEFINITION(MAPSEstimation)
    //MAPS_ACTION("aName",MAPSEstimation::ActionName)
MAPS_END_ACTIONS_DEFINITION

// Use the macros to declare this component (Estimation) behaviour
MAPS_COMPONENT_DEFINITION(MAPSEstimation,"Estimation","1.0",128,
			  MAPS::Threaded,MAPS::Threaded,
			  -1, // Nb of inputs. Leave -1 to use the number of declared input definitions
			  -1, // Nb of outputs. Leave -1 to use the number of declared output definitions
			  -1, // Nb of properties. Leave -1 to use the number of declared property definitions
			  -1) // Nb of actions. Leave -1 to use the number of declared action definitions

//Initialization: Birth() will be called once at diagram execution startup.			  
void MAPSEstimation::Birth()
{

}

void MAPSEstimation::Core()
{
	readInputs();
	if (numReaded < numInputs)
	{
		return;
	}
	numReaded = 0;
	readInputs();
	WriteOutputs();
	//ReportInfo(str);
	str.Clear();
}

void MAPSEstimation::Death()
{ 
}

void MAPSEstimation::readInputs()
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
		numReaded++;
		ArrayLaserObjects = static_cast<AUTO_Objects*>(ioeltin->Data());
		break;
	case 1:
		numReaded++;
		ArrayCameraObjects = static_cast<AUTO_Objects*>(ioeltin->Data());
		break;
	default:
		break;
	}







	

	//while (!DataAvailableInFIFO(Input("LaserObject")) || !DataAvailableInFIFO(Input("CameraObject"))) {}
	////Leer laser
	//if (DataAvailableInFIFO(Input("LaserObject"))) 
	//{
	//	elt = StartReading(Input("LaserObject"));
	//	ArrayLaserObjects = static_cast<AUTO_Objects*>(elt->Data());
	//	StopReading(Input("LaserObject"));
	//}
	////Leer camara
	//if (DataAvailableInFIFO(Input("CameraObject"))) 
	//{
	//	elt = StartReading(Input("CameraObject"));
	//	ArrayCameraObjects = static_cast<AUTO_Objects*>(elt->Data());
	//	StopReading(Input("CameraObject"));
	//}
}

void MAPSEstimation::WriteOutputs()
{

	_ioOutput = StartWriting(Output("Output_estimation"));
	AUTO_Objects &list = *static_cast<AUTO_Objects*>(_ioOutput->Data());
	list = *ArrayLaserObjects;
	StopWriting(_ioOutput);
}