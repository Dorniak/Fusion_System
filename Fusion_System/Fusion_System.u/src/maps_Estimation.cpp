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
	//Inicializar
	initObjects(&Estimation);
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

	//Ordenar la lista de estimaciones por id
	shortObjects(&Estimation);

}

void MAPSEstimation::Estimate()
{
	//Añadir objetos no asociados
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

	//Si la asociacion ya ha pasado antes se le asigna el id
	//Si no se genera un id
	//Si esos objetos ya han pasado por aqui pero en asociaciones distintas hay que eliminar esas asociaciones
	for (int i = 0; i < joined.size(); i++)
	{
		Estimation.object[Estimation.number_of_objects] = calculateObj(&findLaserObj(joined.vector[i][0]), &findCameraObj(joined.vector[i][1]));
		Estimation.object[Estimation.number_of_objects].id = generateIdLC(joined.vector[i][0], joined.vector[i][1]);
		Estimation.number_of_objects++;
	}
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

int MAPSEstimation::generateIdLC(int idL, int idC)
{
	//TODO:: Crear la funcion de generar el id
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

AUTO_Object MAPSEstimation::calculateObj(AUTO_Object * objL, AUTO_Object * objC)
{
	//TODO::Hacer la funcion de estimacion
	return AUTO_Object();
	AUTO_Object result;
	float32_t  paramA, sigmaA, paramB, sigmaB;
	float32_t paramResult, sigmaResult;
	//Funcion final
	//TODO::Recorer lista de parametros

	//id
#pragma region id
	
#pragma endregion
	//gnss
#pragma region gnss

#pragma endregion
	//sub_class
#pragma region sub_class

#pragma endregion
	//class
#pragma region class

#pragma endregion
	//class_age
#pragma region class_age

#pragma endregion
	//class_confidence
#pragma region class_confidence

#pragma endregion
	//track_age
#pragma region track_age

#pragma endregion
	//track_confiance
#pragma region track_confiance

#pragma endregion
	//length
#pragma region length

#pragma endregion
	//width
#pragma region width

#pragma endregion
	//height
#pragma region height

#pragma endregion
	//x
#pragma region x

#pragma endregion
	//y
#pragma region y

#pragma endregion
	//z
#pragma region z

#pragma endregion
	//yaw
#pragma region yaw

#pragma endregion
	//yaw_rate
#pragma region yaw_rate

#pragma endregion
	//speed
#pragma region speed

#pragma endregion
	//acceleration
#pragma region acceleration

#pragma endregion
	//BBox x_rel
#pragma region BBox x_rel

#pragma endregion
	//BBox y_rel
#pragma region BBox y_rel

#pragma endregion
	//x_rel
#pragma region x_rel
	paramA = objL->x_rel;
	sigmaA = objL->x_sigma;
	paramB = objC->x_rel;
	sigmaB = objC->x_sigma;
	if (sigmaA != 0 && sigmaB != 0) {
		EstimateParameter(paramA, sigmaA, paramB, sigmaB, &paramResult, &sigmaResult);
		result.x_rel = paramResult;
		result.x_sigma = sigmaResult;
	}
	else
	{
		if (sigmaA != 0)
		{
			result.x_rel = paramA;
			result.x_sigma = sigmaA;
		}
		else if (sigmaB != 0)
		{
			result.x_rel = paramB;
			result.x_sigma = sigmaB;
		}
		else
		{
			//TODO::Hacer algo si las dos sigmas son 0 una media o algo
		}
	}
#pragma endregion
	//y_rel
#pragma region y_rel
	paramA = objL->y_rel;
	sigmaA = objL->y_sigma;
	paramB = objC->y_rel;
	sigmaB = objC->y_sigma;
	if (sigmaA != 0 && sigmaB != 0) {
		EstimateParameter(paramA, sigmaA, paramB, sigmaB, &paramResult, &sigmaResult);
		result.y_rel = paramResult;
		result.y_sigma = sigmaResult;
	}
	else
	{
		if (sigmaA != 0)
		{
			result.y_rel = paramA;
			result.y_sigma = sigmaA;
		}
		else if (sigmaB != 0)
		{
			result.y_rel = paramB;
			result.y_sigma = sigmaB;
		}
		else
		{
			//TODO::Hacer algo si las dos sigmas son 0 una media o algo
		}
	}
#pragma endregion
	//z_rel
#pragma region z_rel
	paramA = objL->z_rel;
	sigmaA = objL->z_sigma;
	paramB = objC->z_rel;
	sigmaB = objC->z_sigma;
	if (sigmaA != 0 && sigmaB != 0) {
		EstimateParameter(paramA, sigmaA, paramB, sigmaB, &paramResult, &sigmaResult);
		result.z_rel = paramResult;
		result.z_sigma = sigmaResult;
	}
	else
	{
		if (sigmaA != 0)
		{
			result.z_rel = paramA;
			result.z_sigma = sigmaA;
		}
		else if (sigmaB != 0)
		{
			result.z_rel = paramB;
			result.z_sigma = sigmaB;
		}
		else
		{
			//TODO::Hacer algo si las dos sigmas son 0 una media o algo
		}
	}
#pragma endregion
	//yaw_rel
#pragma region yaw_rel

#pragma endregion
	//speed_x_rel
#pragma region speed_x_rel

#pragma endregion
	//speed_y_rel
#pragma region speed_y_rel

#pragma endregion
	//acceleration_x_rel
#pragma region acceleration_x_rel

#pragma endregion
	//acceleration_y_rel
#pragma region acceleration_y_rel

#pragma endregion
	//distance
#pragma region distance

#pragma endregion
	//speed_rel
#pragma region speed_rel

#pragma endregion
	//acceleration_rel
#pragma region acceleration_rel

#pragma endregion
	//length_sigma
#pragma region length_sigma

#pragma endregion
	//width_sigma
#pragma region width_sigma

#pragma endregion
	//height_sigma
#pragma region height_sigma

#pragma endregion
	//x_sigma
#pragma region x_sigma

#pragma endregion
	//y_sigma
#pragma region y_sigma

#pragma endregion
	//z_sigma
#pragma region z_sigma

#pragma endregion
	//yaw_sigma
#pragma region yaw_sigma

#pragma endregion
	//speed_x_sigma
#pragma region speed_x_sigma

#pragma endregion
	//speed_y_sigma
#pragma region speed_y_sigma

#pragma endregion
	//acceleration_x_sigma
#pragma region acceleration_x_sigma

#pragma endregion
	//acceleration_y_sigma
#pragma region acceleration_y_sigma

#pragma endregion
	//turn_signal
#pragma region turn_signal

#pragma endregion
	//brake_signal
#pragma region brake_signal

#pragma endregion
	//timestamp
#pragma region timestamp

#pragma endregion
	//status
#pragma region status

#pragma endregion
}

void MAPSEstimation::EstimateParameter(float32_t paramA, float32_t sigmaA, float32_t paramB, float32_t sigmaB, float32_t * paramResult, float32_t * sigmaResult)
{
	*paramResult = calcParam(paramA, sigmaA, paramB, sigmaB);
	*sigmaResult = calcSigma(sigmaA, sigmaB);
}
float32_t MAPSEstimation::calcParam(float32_t paramL, float32_t sigmaL, float32_t paramC, float32_t sigmaC)
{
	float32_t param = ((sigmaC / (sigmaL + sigmaC))*paramL) + ((sigmaL / (sigmaL + sigmaC)) *paramC);
	return param;
}

float32_t MAPSEstimation::calcSigma(float32_t sigmaL, float32_t sigmaC)
{
	float32_t sigma = (sigmaL*sigmaC) / (sigmaL + sigmaC);
	return sigma;
}
//TODO::Implemaentar control division entre 0