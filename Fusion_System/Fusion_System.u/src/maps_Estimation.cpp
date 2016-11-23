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

	shortVector(&IdL);
	shortVector(&IdC);
	shortVectorLCA(&LCAssociations);

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

	shortObjects(&Estimation);

}

void MAPSEstimation::Estimate()
{
	//TODO:A�adir objetos no asociados
		//TODO:Comprobar ids
		//TODO:Generar nuevos ids
	//Tratar objetos no asociados
	for (int i = 0; i < nonLaserJoined.size(); i++)
	{
		Estimation.object[Estimation.number_of_objects] = findLaserObj(nonLaserJoined.vector[i]);
		Estimation.object[Estimation.number_of_objects].id = generateIdLas(nonLaserJoined.vector[i]);
		Estimation.number_of_objects++;
	}

	for (int i = 0; i < nonCameraJoined.size(); i++)
	{
		Estimation.object[Estimation.number_of_objects] = findCameraObj(nonCameraJoined.vector[i]);
		Estimation.object[Estimation.number_of_objects].id = generateIdLas(nonCameraJoined.vector[i]);
		Estimation.number_of_objects++;
	}

	//TODO:A�adir objetos asociados
		//TODO:Comprobar y generar nuevos ids
		//Si la asociacion ya ha pasado antes se le asigna el id
		//Si no se genera un id
		//Si esos objetos ya han pasado por aqui pero en asociaciones distintas hay que eliminar esas asociaciones




	//TODO:Ordenar la lista de estimaciones por id
}

void MAPSEstimation::shortVector(vector<int[2]> * vect)
{
	int id, newId;
	for (int i = 0; i < vect->size(); i++)
	{
		id = *vect[i][0];
		newId = *vect[i][1];

		for (int j = i - 1; j > 0; j++)
		{
			if (*vect[j][0] > id)
			{
				*vect[j + 1][0] = *vect[j][0];
				*vect[j + 1][1] = *vect[j][1];
			}
			else
			{
				*vect[j + 1][0] = id;
				*vect[j + 1][1] = newId;
				break;
			}
		}
	}
}

void MAPSEstimation::shortVectorLCA(vector<int[3]> * vect)
{
	int idL, idC, newId;
	for (int i = 0; i < vect->size(); i++)
	{
		idL = *vect[i][0];
		idC = *vect[i][1];
		newId = *vect[i][2];

		for (int j = i - 1; j > 0; j++)
		{
			if (*vect[j][0] > idL)
			{
				*vect[j + 1][0] = *vect[j][0];
				*vect[j + 1][1] = *vect[j][1];
				*vect[j + 1][2] = *vect[j][2];
			}
			else if (*vect[j][0] == idL)
			{
				if (*vect[j][1] > idC)
				{
					*vect[j + 1][0] = *vect[j][0];
					*vect[j + 1][1] = *vect[j][1];
					*vect[j + 1][2] = *vect[j][2];
				}
				else
				{
					*vect[j + 1][0] = idL;
					*vect[j + 1][1] = idC;
					*vect[j + 1][2] = newId;
					break;
				}
			}
			else
			{
				*vect[j + 1][0] = idL;
				*vect[j + 1][1] = idC;
				*vect[j + 1][2] = newId;
				break;
			}
		}
	}
}

void MAPSEstimation::shortObjects(AUTO_Objects * objects)
{
	AUTO_Object objectaux;
	for (int i = 0; i < objects->number_of_objects; i++)
	{
		objectaux = objects->object[i];
		for (int j = i - 1; j > 0; j++)
		{
			//TODO:poner numero de objetos antes de este punto
			if (objects->object[j].id > objectaux.id)
			{
				objects->object[j + 1] = objects->object[j];
			}
			else
			{
				objects->object[j + 1] = objectaux;
			}
		}
	}
}

int MAPSEstimation::generateIdLas(int id)
{
	//TODO::Generar ids
	return 0;
}

int MAPSEstimation::generateIdCam(int id)
{
	//TODO::Generar ids
	return 0;
}

AUTO_Object MAPSEstimation::findLaserObj(int id)
{
	for (int i = 0; i < ArrayLaserObjects.number_of_objects; i++)
	{
		if (ArrayLaserObjects.object[i].id == id)
		{
			return ArrayLaserObjects.object[i];
		}
	}
	return AUTO_Object();
}

AUTO_Object MAPSEstimation::findCameraObj(int id)
{
	for (int i = 0; i < ArrayCameraObjects.number_of_objects; i++)
	{
		if (ArrayCameraObjects.object[i].id == id)
		{
			return ArrayCameraObjects.object[i];
		}
	}
	return AUTO_Object();
}
