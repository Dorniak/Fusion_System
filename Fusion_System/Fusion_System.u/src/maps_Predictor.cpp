////////////////////////////////
// RTMaps SDK Component
////////////////////////////////

////////////////////////////////
// Purpose of this module :
////////////////////////////////

#include "maps_Predictor.h"	// Includes the header of this component

const MAPSTypeFilterBase ValeoStructure = MAPS_FILTER_USER_STRUCTURE(AUTO_Objects);
// Use the macros to declare the inputs
MAPS_BEGIN_INPUTS_DEFINITION(MAPSPredictor)
	MAPS_INPUT("LaserObject", ValeoStructure, MAPS::FifoReader)
	MAPS_INPUT("CameraObject", ValeoStructure, MAPS::FifoReader)
MAPS_END_INPUTS_DEFINITION

// Use the macros to declare the outputs
MAPS_BEGIN_OUTPUTS_DEFINITION(MAPSPredictor)
	MAPS_OUTPUT_USER_STRUCTURE("LaserObjects", AUTO_Objects)
	MAPS_OUTPUT_USER_STRUCTURE("CameraObjects", AUTO_Objects)
MAPS_END_OUTPUTS_DEFINITION

// Use the macros to declare the properties
MAPS_BEGIN_PROPERTIES_DEFINITION(MAPSPredictor)
    //MAPS_PROPERTY("pName",128,false,false)
MAPS_END_PROPERTIES_DEFINITION

// Use the macros to declare the actions
MAPS_BEGIN_ACTIONS_DEFINITION(MAPSPredictor)
    //MAPS_ACTION("aName",MAPSPredictor::ActionName)
MAPS_END_ACTIONS_DEFINITION

// Use the macros to declare this component (Predictor) behaviour
MAPS_COMPONENT_DEFINITION(MAPSPredictor,"Predictor","1.0",128,
			  MAPS::Threaded,MAPS::Threaded,
			  -1, // Nb of inputs. Leave -1 to use the number of declared input definitions
			  -1, // Nb of outputs. Leave -1 to use the number of declared output definitions
			  -1, // Nb of properties. Leave -1 to use the number of declared property definitions
			  -1) // Nb of actions. Leave -1 to use the number of declared action definitions
	  
void MAPSPredictor::Birth()
{
	QueryPerformanceFrequency(&Frecuency);
	QueryPerformanceCounter(&StartingTime);
    // Reports this information to the RTMaps console. You can remove this line if you know when Birth() is called in the component lifecycle.
    ReportInfo("Passing through Birth() method");
}

void MAPSPredictor::Core() 
{
	str.Clear();
	readInputs();

	if (!ready)
	{
		if (completedL[0] && completedL[1] && completedC[0] && completedC[1])
		{
			ready = true;
		}
	}
	else {
		//If not has been predicted previusly 
		if (!predicted || updated[0] || updated[1]) {
			predecir();
		}
		QueryPerformanceCounter(&EndingTime);
		ElapsedTime = (double)(EndingTime.QuadPart - StartingTime.QuadPart) * 1000 / Frecuency.QuadPart;
		if (ElapsedTime>framerate)
		{
			StartingTime = EndingTime;
			WriteOutputs();
		}
	}
	//ReportInfo(str);
}

void MAPSPredictor::Death()
{
    
}

void MAPSPredictor::readInputs()
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
		ArrayLaserObjects = static_cast<AUTO_Objects*>(ioeltin->Data());
		LaserObjects[0] = LaserObjects[1];
		LaserObjects[1] = *ArrayLaserObjects;
		updated[0] = true;
		if (!ready) {
			completedL[0] = completedL[1];
			completedL[1] = true;
		}
		if (firsttime) {
			timestamp = ArrayLaserObjects->timestamp;
			firsttime = false;
		}
		break;
	case 1:
		ArrayCameraObjects = static_cast<AUTO_Objects*>(ioeltin->Data());
		CameraObjects[0] = CameraObjects[1];
		CameraObjects[1] = *ArrayCameraObjects;
		updated[1] = true;
		if (!ready) {
			completedC[0] = completedC[1];
			completedC[1] = true;
		}
		if (firsttime) {
			timestamp = ArrayCameraObjects->timestamp;
			firsttime = false;
		}
		break;

	default:
		break;
	}
}

void MAPSPredictor::WriteOutputs()
{
	predicted = false;

	_ioOutput = StartWriting(Output("LaserObjects"));
	AUTO_Objects &list = *static_cast<AUTO_Objects*>(_ioOutput->Data());
	list = LaserObjectsOutput;
	StopWriting(_ioOutput);

	_ioOutput = StartWriting(Output("CameraObjects"));
	AUTO_Objects &list2 = *static_cast<AUTO_Objects*>(_ioOutput->Data());
	list2 = CameraObjectsOutput;
	StopWriting(_ioOutput);
}

void MAPSPredictor::predecir()
{
	while (timestamp < max(LaserObjects[1].timestamp, CameraObjects[1].timestamp))
	{
		timestamp += framerate;
	}
	//Predecir porsicion de obstaculos 
	if (updated[0]) 
	{
		LaserObjectsOutput = LaserObjects[1];
		for (size_t i = 0; i < LaserObjectsOutput.number_of_objects; i++)
		{
			int pos = findPosition(LaserObjects[0],LaserObjectsOutput.object[i].id);
			if (pos != -1)
			{
				//Calcular el vector de avance
				Point3D vector;
				vector.x = LaserObjects[1].object[i].x_rel - LaserObjects[0].object[pos].x_rel;
				vector.y = LaserObjects[1].object[i].y_rel - LaserObjects[0].object[pos].y_rel;
				vector.z = LaserObjects[1].object[i].z_rel - LaserObjects[0].object[pos].z_rel;
				//mover el obstaculo segun el vector de avance
				moveObstacle(&LaserObjectsOutput.object[i], vector, abs(LaserObjects[1].timestamp - LaserObjects[0].timestamp), LaserObjectsOutput.timestamp);
			}
		}
		
		updated[0] = false;
	}
	if (updated[1])
	{
		CameraObjectsOutput = CameraObjects[1];
		for (size_t i = 0; i < CameraObjectsOutput.number_of_objects; i++)
		{
			int pos = findPosition(CameraObjects[0], CameraObjectsOutput.object[i].id);
			if (pos != -1)
			{
				//Calcular el vector de avance
				Point3D vector;
				vector.x = CameraObjects[1].object[i].x_rel - CameraObjects[0].object[pos].x_rel;
				vector.y = CameraObjects[1].object[i].y_rel - CameraObjects[0].object[pos].y_rel;
				vector.z = CameraObjects[1].object[i].z_rel - CameraObjects[0].object[pos].z_rel;
				//mover el obstaculo segun el vector de avance
				moveObstacle(&CameraObjectsOutput.object[i], vector, abs(CameraObjects[1].timestamp - CameraObjects[0].timestamp), CameraObjectsOutput.timestamp);
			}
		}

		updated[1] = false;
	}
	predicted = true;	
}

int MAPSPredictor::findPosition(AUTO_Objects objects, int id)
{
	for (size_t i = 0; i < objects.number_of_objects; i++)
	{
		if (objects.object[i].id == id)
		{
			return i;
		}
	}
	return -1;
}

void MAPSPredictor::moveObstacle(AUTO_Object * obstacle, Point3D vector, int Distancetime, int timestamp)
{
	vector.x = (float32_t)(((double)vector.x / (double)Distancetime) *(this->timestamp-timestamp));
	vector.y = (float32_t)(((double)vector.y / (double)Distancetime) *(this->timestamp-timestamp));
	vector.z = (float32_t)(((double)vector.z / (double)Distancetime) *(this->timestamp-timestamp));

	obstacle->x_rel += vector.x;
	obstacle->y_rel += vector.y;
	obstacle->z_rel += vector.z;

	for (size_t i = 0; i < 4; i++)
	{
		obstacle->bounding_box_x_rel[i] += vector.x;
		obstacle->bounding_box_y_rel[i] += vector.y;
	}

	obstacle->distance = (float32_t)sqrt(pow(obstacle->x_rel, 2) + pow(obstacle->y_rel, 2) + pow(obstacle->z_rel, 2));
		
}