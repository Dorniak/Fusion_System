////////////////////////////////
// RTMaps SDK Component
////////////////////////////////

////////////////////////////////
// Purpose of this module :
////////////////////////////////

#include "maps_Estimation.h"	// Includes the header of this component
const MAPSTypeFilterBase ValeoStructure = MAPS_FILTER_USER_STRUCTURE(AUTO_Objects);
const MAPSTypeFilterBase Associations = MAPS_FILTER_USER_STRUCTURE(AssociatedObjs);
const MAPSTypeFilterBase NonAssociations = MAPS_FILTER_USER_STRUCTURE(NonAssociated);

// Use the macros to declare the inputs
MAPS_BEGIN_INPUTS_DEFINITION(MAPSEstimation)
    //MAPS_INPUT("iName",MAPS::FilterInteger32,MAPS::FifoReader)
	MAPS_INPUT("LaserObject", ValeoStructure, MAPS::FifoReader)
	MAPS_INPUT("CameraObject", ValeoStructure, MAPS::FifoReader)
	MAPS_INPUT("Associations", Associations, MAPS::FifoReader)
	MAPS_INPUT("NonAssocLaser", NonAssociations, MAPS::FifoReader)
	MAPS_INPUT("NonAssocCamera", NonAssociations, MAPS::FifoReader)

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
	if (!readed[0] || !readed[1] || !readed[2] || !readed[3] || !readed[4])
	{
		return;
	}
	for (int i = 0; i < numInputs; i++)
	{
		readed[i] = false;
	}
	ProcessData();
	WriteOutputs();
	//ReportInfo(str);
	str.Clear();
}

void MAPSEstimation::Death()
{ 
}

void MAPSEstimation::readInputs()
{
	MAPSInput* inputs[5] = { &Input("LaserObject"), &Input("CameraObject"), &Input("Associations") , &Input("NonAssocLaser") , &Input("NonAssocCamera") };
	int inputThatAnswered;
	MAPSIOElt* ioeltin = StartReading(5, inputs, &inputThatAnswered);
	if (ioeltin == NULL) {
		return;
	}
	switch (inputThatAnswered)
	{
	case 0:
		ArrayLaserObjects_input = static_cast<AUTO_Objects*>(ioeltin->Data());
		ArrayLaserObjects = *ArrayLaserObjects_input;
		readed[0] = true;
		break;
	case 1:
		ArrayCameraObjects_input = static_cast<AUTO_Objects*>(ioeltin->Data());
		ArrayCameraObjects = *ArrayCameraObjects_input;
		readed[1] = true;
		break;
	case 2:
		joined_input = static_cast<AssociatedObjs*>(ioeltin->Data());
		joined = *joined_input;
		readed[2] = true;
		break;
	case 3:
		nonLaserJoined_input = static_cast<NonAssociated*>(ioeltin->Data());
		nonLaserJoined = *nonLaserJoined_input;
		readed[3] = true;
		break;
	case 4:
		nonCameraJoined_input = static_cast<NonAssociated*>(ioeltin->Data());
		nonCameraJoined = *nonCameraJoined_input;
		readed[4] = true;
		break;
	default:
		break;
	}
}

void MAPSEstimation::WriteOutputs()
{
}

void MAPSEstimation::ProcessData()
{
	Estimate();
}

void MAPSEstimation::Estimate()
{
	int a;
	//TODO:Añadir objetos no asociados
		//TODO:Comprobar ids
		//TODO:Generar nuevos ids

	//TODO:Añadir objetos asociados
		//TODO:Comprobar y generar nuevos ids
		//Si la asociacion ya ha pasado antes se le asigna el id
		//Si no se genera un id
		//Si esos objetos ya han pasado por aqui pero en asociaciones distintas hay que eliminar esas asociaciones




	//TODO:Ordenar la lista de estimaciones por id
}
