////////////////////////////////
// RTMaps SDK Component
////////////////////////////////

////////////////////////////////
// Purpose of this module :
////////////////////////////////

#include "maps_GraficResult.h"	// Includes the header of this component

const MAPSTypeFilterBase ValeoStructure = MAPS_FILTER_USER_STRUCTURE(AUTO_Objects);
// Use the macros to declare the inputs
MAPS_BEGIN_INPUTS_DEFINITION(MAPSGraficResult)
	MAPS_INPUT("LaserObject", ValeoStructure, MAPS::FifoReader)
	MAPS_INPUT("CameraObject", ValeoStructure, MAPS::FifoReader)
    //MAPS_INPUT("iName",MAPS::FilterInteger32,MAPS::FifoReader)
MAPS_END_INPUTS_DEFINITION

// Use the macros to declare the outputs
MAPS_BEGIN_OUTPUTS_DEFINITION(MAPSGraficResult)
    //MAPS_OUTPUT("oName",MAPS::Integer32,NULL,NULL,1)
MAPS_END_OUTPUTS_DEFINITION

// Use the macros to declare the properties
MAPS_BEGIN_PROPERTIES_DEFINITION(MAPSGraficResult)
    //MAPS_PROPERTY("pName",128,false,false)
MAPS_END_PROPERTIES_DEFINITION

// Use the macros to declare the actions
MAPS_BEGIN_ACTIONS_DEFINITION(MAPSGraficResult)
    //MAPS_ACTION("aName",MAPSGraficResult::ActionName)
MAPS_END_ACTIONS_DEFINITION

// Use the macros to declare this component (GraficResult) behaviour
MAPS_COMPONENT_DEFINITION(MAPSGraficResult,"GraficResult","1.0",128,
			  MAPS::Threaded,MAPS::Threaded,
			  -1, // Nb of inputs. Leave -1 to use the number of declared input definitions
			  -1, // Nb of outputs. Leave -1 to use the number of declared output definitions
			  -1, // Nb of properties. Leave -1 to use the number of declared property definitions
			  -1) // Nb of actions. Leave -1 to use the number of declared action definitions


void MAPSGraficResult::Birth()
{
	direction = direction + std::to_string(idLaser) + "_" + std::to_string(idCamera) + "\\";
	_mkdir(direction.c_str());

	fLaser.open(direction + "LaserP_" + std::to_string(idLaser) + "_" + std::to_string(idCamera) + ".m", ofstream::out);
	fCamera.open(direction + "CameraP_" + std::to_string(idLaser) + "_" + std::to_string(idCamera) + ".m", ofstream::out);
	fEstimation.open(direction + "Estimacion_" + std::to_string(idLaser) + "_" + std::to_string(idCamera) + ".m", ofstream::out);

	fLaser << "%timestamp,x_rel,y_rel,x_sigma,y_sigma,speed,speed_x_rel,speed_y_rel,speed_x_sigma,speed_y_sigma" << '\n' << "LaserPre=[";
	fCamera << "%timestamp,x_rel,y_rel,x_sigma,y_sigma,speed,speed_x_rel,speed_y_rel,speed_x_sigma,speed_y_sigma" << '\n' << "CameraPre=[";
	fEstimation << "%timestamp,x_rel,y_rel,x_sigma,y_sigma,speed,speed_x_rel,speed_y_rel,speed_x_sigma,speed_y_sigma" << '\n' << "Estimate=[";
}

void MAPSGraficResult::Core()
{
	readInputs();
	if (!readed[0] || !readed[1])
	{
		return;
	}
	for (int i = 0; i < numInputs; i++)
	{
		readed[i] = false;
	}

	WriteLaser();
	WriteCamera();
	WriteResult();
}

//De-initialization: Death() will be called once at diagram execution shutdown.
void MAPSGraficResult::Death()
{
	fLaser << "];";
	fCamera << "];";
	fEstimation << "];";

	fLaser.close();
	fCamera.close();
	fEstimation.close();
}

void MAPSGraficResult::readInputs()
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
		ArrayLaserObjects_input = static_cast<AUTO_Objects*>(ioeltin->Data());
		ArrayLaserObjects = *ArrayLaserObjects_input;
		readed[0] = true;
		break;
	case 1:
		ArrayCameraObjects_input = static_cast<AUTO_Objects*>(ioeltin->Data());
		ArrayCameraObjects = *ArrayCameraObjects_input;
		readed[1] = true;
		break;
	default:
		break;
	}
}

void MAPSGraficResult::WriteLaser()
{
	for (int i = 0; i < ArrayLaserObjects.number_of_objects; i++)
	{
		if (ArrayLaserObjects.object[i].id == idLaser)
		{
			if (!firstL)
			{
				fLaser << ";" << '\n' << ArrayLaserObjects.timestamp << "," << ArrayLaserObjects.object[i].x_rel << "," << ArrayLaserObjects.object[i].y_rel << "," << ArrayLaserObjects.object[i].x_sigma
					<< "," << ArrayLaserObjects.object[i].y_sigma << "," << ArrayLaserObjects.object[i].speed_rel << "," << ArrayLaserObjects.object[i].speed_x_rel << "," << ArrayLaserObjects.object[i].speed_y_rel
					<< "," << ArrayLaserObjects.object[i].speed_x_sigma << "," << ArrayLaserObjects.object[i].speed_y_sigma;
			}
			else
			{
				fLaser << '\n' << ArrayLaserObjects.timestamp << "," << ArrayLaserObjects.object[i].x_rel << "," << ArrayLaserObjects.object[i].y_rel << "," << ArrayLaserObjects.object[i].x_sigma
					<< "," << ArrayLaserObjects.object[i].y_sigma << "," << ArrayLaserObjects.object[i].speed_rel << "," << ArrayLaserObjects.object[i].speed_x_rel << "," << ArrayLaserObjects.object[i].speed_y_rel
					<< "," << ArrayLaserObjects.object[i].speed_x_sigma << "," << ArrayLaserObjects.object[i].speed_y_sigma;
				firstL = false;
			}
			break;
		}
	}
}

void MAPSGraficResult::WriteCamera()
{
	for (int i = 0; i < ArrayCameraObjects.number_of_objects; i++)
	{
		if (ArrayCameraObjects.object[i].id == idCamera)
		{
			if (!firstC)
			{
				fCamera << ";" << '\n' << ArrayCameraObjects.timestamp << "," << ArrayCameraObjects.object[i].x_rel << "," << ArrayCameraObjects.object[i].y_rel << "," << ArrayCameraObjects.object[i].x_sigma
					<< "," << ArrayCameraObjects.object[i].y_sigma << "," << ArrayCameraObjects.object[i].speed_rel << "," << ArrayCameraObjects.object[i].speed_x_rel << "," << ArrayCameraObjects.object[i].speed_y_rel
					<< "," << ArrayCameraObjects.object[i].speed_x_sigma << "," << ArrayCameraObjects.object[i].speed_y_sigma;
			}
			else
			{
				fCamera << '\n' << ArrayCameraObjects.timestamp << "," << ArrayCameraObjects.object[i].x_rel << "," << ArrayCameraObjects.object[i].y_rel << "," << ArrayCameraObjects.object[i].x_sigma
					<< "," << ArrayCameraObjects.object[i].y_sigma << "," << ArrayCameraObjects.object[i].speed_rel << "," << ArrayCameraObjects.object[i].speed_x_rel << "," << ArrayCameraObjects.object[i].speed_y_rel
					<< "," << ArrayCameraObjects.object[i].speed_x_sigma << "," << ArrayCameraObjects.object[i].speed_y_sigma;
				firstC = false;
			}
			break;
		}
	}
}

void MAPSGraficResult::WriteResult()
{
	float32_t X_Estimation, Y_Estimation, X_SigmaE, Y_SigmaE;
	float32_t X_Laser, Y_Laser, X_SigmaL, Y_SigmaL;
	float32_t X_Camera, Y_Camera, X_SigmaC, Y_SigmaC;
	bool is[2] = { false,false };

	for (int i = 0; i < ArrayLaserObjects.number_of_objects; i++)
	{
		if (ArrayLaserObjects.object[i].id == idLaser)
		{
			//fLaser << ArrayLaserObjects.object[i].timestamp << "," << ArrayLaserObjects.object[i].x_rel << "," << ArrayLaserObjects.object[i].y_rel << "," << ArrayLaserObjects.object[i].x_sigma << "," << ArrayLaserObjects.object[i].y_sigma << ";" << '\n';
			X_Laser = ArrayLaserObjects.object[i].x_rel;
			Y_Laser = ArrayLaserObjects.object[i].y_rel;
			X_SigmaL = ArrayLaserObjects.object[i].x_sigma;
			Y_SigmaL = ArrayLaserObjects.object[i].y_sigma;
			is[0] = true;
			break;
		}
	}
	for (int i = 0; i < ArrayCameraObjects.number_of_objects; i++)
	{
		if (ArrayCameraObjects.object[i].id == idCamera)
		{
			//fCamera << ArrayCameraObjects.object[i].timestamp << "," << ArrayCameraObjects.object[i].x_rel << "," << ArrayCameraObjects.object[i].y_rel << "," << ArrayCameraObjects.object[i].x_sigma << "," << ArrayCameraObjects.object[i].y_sigma << ";" << '\n';
			X_Camera = ArrayCameraObjects.object[i].x_rel;
			Y_Camera = ArrayCameraObjects.object[i].y_rel;
			X_SigmaC = ArrayCameraObjects.object[i].x_sigma;
			Y_SigmaC = ArrayCameraObjects.object[i].y_sigma;
			is[1] = true;
			break;
		}
	}
	if (is[0] && is[1]) {
		X_Estimation = calcParam(X_Laser, X_SigmaL, X_Camera, X_SigmaC);
		Y_Estimation = calcParam(Y_Laser, Y_SigmaL, Y_Camera, Y_SigmaC);
		X_SigmaE = calcSigma(X_SigmaL, X_SigmaC);
		Y_SigmaE = calcSigma(Y_SigmaL, Y_SigmaC);
		if (!firstE)
		{
			fEstimation << ";" << '\n' << ArrayLaserObjects.timestamp << "," << ArrayCameraObjects.timestamp << "," << X_Estimation << "," << Y_Estimation << "," << X_SigmaE << "," << Y_SigmaE;
		}
		else
		{
			fEstimation << '\n' << ArrayLaserObjects.timestamp << "," << ArrayCameraObjects.timestamp << "," << X_Estimation << "," << Y_Estimation << "," << X_SigmaE << "," << Y_SigmaE;
			firstE = false;
		}
	}
	is[0] = false;
	is[1] = false;
}

float32_t MAPSGraficResult::calcParam(float32_t paramL, float32_t sigmaL, float32_t paramC, float32_t sigmaC)
{
	float32_t param = ((sigmaC / (sigmaL + sigmaC))*paramL) + ((sigmaL / (sigmaL + sigmaC)) *paramC);
	return param;
}

float32_t MAPSGraficResult::calcSigma(float32_t sigmaL, float32_t sigmaC)
{
	float32_t sigma = (sigmaL*sigmaC) / (sigmaL + sigmaC);
	return sigma;
}
