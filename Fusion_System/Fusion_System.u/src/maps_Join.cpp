////////////////////////////////
// RTMaps SDK Component
////////////////////////////////

////////////////////////////////
// Purpose of this module :
////////////////////////////////

#include "maps_Join.h"	// Includes the header of this component

const MAPSTypeFilterBase ValeoStructure = MAPS_FILTER_USER_STRUCTURE(AUTO_Objects);
const MAPSTypeFilterBase matchedVector = MAPS_FILTER_USER_STRUCTURE(MATCH_OBJECTS);

// Use the macros to declare the inputs
MAPS_BEGIN_INPUTS_DEFINITION(MAPSJoin)
	MAPS_INPUT("LaserObject", ValeoStructure, MAPS::FifoReader)
	MAPS_INPUT("CameraObject", ValeoStructure, MAPS::FifoReader)
	MAPS_INPUT("MatchedLaser", matchedVector, MAPS::FifoReader)
	MAPS_INPUT("MatchedCamera", matchedVector, MAPS::FifoReader)
MAPS_END_INPUTS_DEFINITION

// Use the macros to declare the outputs
MAPS_BEGIN_OUTPUTS_DEFINITION(MAPSJoin)
	MAPS_OUTPUT_USER_STRUCTURE("LaserObjects", AUTO_Objects)
	MAPS_OUTPUT_USER_STRUCTURE("CameraObjects", AUTO_Objects)
	MAPS_OUTPUT_USER_STRUCTURE("AObjects", AssociatedObjs)
	MAPS_OUTPUT_USER_STRUCTURE("NALaser", NonAssociated)
	MAPS_OUTPUT_USER_STRUCTURE("NACamera", NonAssociated)
MAPS_END_OUTPUTS_DEFINITION

// Use the macros to declare the properties
MAPS_BEGIN_PROPERTIES_DEFINITION(MAPSJoin)
    //MAPS_PROPERTY("pName",128,false,false)
MAPS_END_PROPERTIES_DEFINITION

// Use the macros to declare the actions
MAPS_BEGIN_ACTIONS_DEFINITION(MAPSJoin)
    //MAPS_ACTION("aName",MAPSJoin::ActionName)
MAPS_END_ACTIONS_DEFINITION

// Use the macros to declare this component (Join) behaviour
MAPS_COMPONENT_DEFINITION(MAPSJoin,"Join","1.0",128,
			  MAPS::Threaded,MAPS::Threaded,
			  -1, // Nb of inputs. Leave -1 to use the number of declared input definitions
			  -1, // Nb of outputs. Leave -1 to use the number of declared output definitions
			  -1, // Nb of properties. Leave -1 to use the number of declared property definitions
			  -1) // Nb of actions. Leave -1 to use the number of declared action definitions

void MAPSJoin::Birth()
{
    // Reports this information to the RTMaps console. You can remove this line if you know when Birth() is called in the component lifecycle.
    ReportInfo("Passing through Birth() method");
}
void MAPSJoin::Core()
{
#pragma region Reading
	ReadInputs();
	if (!readed[0] || !readed[1] || !readed[2] || !readed[3])
	{
		return;
	}
	for (int i = 0; i < numInputs; i++)
	{
		readed[i] = false;
	}
#pragma endregion

	cleanStructures();

#pragma region Processing
	ProcessData();
#pragma endregion

#pragma region Writing
	WriteOutputs();
#pragma endregion

	
	str << '\n';
	str << "Inputs:";
	str << '\n' << "Laser: " << Laser_Objects.number_of_objects << " Camera: " << Camera_Objects.number_of_objects;
	str << '\n' << "Outputs:";
	str << '\n' << "Associated objects: " << joined.size();
	str << '\n' << "Non Laser Associated objects: " << nonLaserJoined.size();
	str << '\n' << "Non Camera Associated objects: " << nonCameraJoined.size();
	ReportInfo(str);
	str.Clear();

}
void MAPSJoin::Death()
{
    // Reports this information to the RTMaps console. You can remove this line if you know when Death() is called in the component lifecycle.
    ReportInfo("Passing through Death() method");
}

void MAPSJoin::ReadInputs()
{
	MAPSInput* inputs[4] = { &Input("LaserObject"), &Input("CameraObject"),&Input("MatchedLaser"), &Input("MatchedCamera") };
	int inputThatAnswered;
	MAPSIOElt* ioeltin = StartReading(4, inputs, &inputThatAnswered);
	if (ioeltin == NULL) {
		return;
	}
	switch (inputThatAnswered)
	{
	case 0:
		input_Laser_Objects = static_cast<AUTO_Objects*>(ioeltin->Data());
		Laser_Objects = *input_Laser_Objects;
		readed[0] = true;
		break;
	case 1:
		input_Camera_Objects = static_cast<AUTO_Objects*>(ioeltin->Data());
		Camera_Objects = *input_Camera_Objects;
		readed[1] = true;
		break;
	case 2:
		input_Laser_Matched = static_cast<MATCH_OBJECTS*>(ioeltin->Data());
		Laser_Matched = *input_Laser_Matched;
		readed[2] = true;
		break;
	case 3:
		input_Camera_Matched = static_cast<MATCH_OBJECTS*>(ioeltin->Data());
		Camera_Matched = *input_Camera_Matched;
		readed[3] = true;
		break;
	default:
		break;
	}
}

void MAPSJoin::ProcessData()
{
	//Here I have the list of obstacles and the list of the obstacles in the gating window of each one
	//I have to compare all the parameters that I can to get the coincident level that the obstacles have
	//and select one nonone for each one

	//Useful parameters
	/*
	-Position
	-Size
	-Width
	-Speed
	-Direction vector
	-Relative speed of the vehicle
	-Kind of obstacle
	*/

	//I have to create 3 list

	//-List of asociated obstacles
	/*
		Format:
		2 columns
		n rows; n = number of Laser obstacles
	*/

	//-List of non asociated Laser obstacles
	/*
	Format:
	1 row n columns
	For each position we have the id of one Laser object
	*/

	//-List of non asociated Camera obstacles
	/*
	Format:
	1 row n columns
	For each position we have the id of one Camera object
	*/
	joined.resize(Laser_Matched.number_objects);
	for (int i = 0; i < Laser_Matched.number_objects; i++)
	{
		//Clean list of joined
		joined.vector[i][0] = Laser_Matched.id[i];
		joined.vector[i][1] = -1;
		joined.vector[i][2] = 0;
		for (int j = 0; j < Laser_Matched.number_matched[i]; j++)
		{
			score = 0;
			score = calculateScore(Laser_Matched.id[i], Laser_Matched.Matrix_matched[i][j]);
			if (score > 0) 
			{
				addAssociation(i, Laser_Matched.Matrix_matched[i][j], score);



				/*
				if (score == joined.vector[i][3])
				{
					//TODO::We have a problem
				}
				else if (score > joined.vector[i][3])
				{
					joined.vector[i][1] = Laser_Matched.Matrix_matched[i][j];
					joined.vector[i][2] = score;
				}
				*/
			}
		}
	}
	selectAssociations();
	//At the end of this for we will have a list of asociated objects
	cleanAssociatedList();
	//Now we can find the non calculated Laser objects
	for (int j = 0; j < Laser_Objects.number_of_objects; j++)
	{
		if (!findAssociatedLaser(Laser_Objects.object[j].id))
		{
			nonLaserJoined.push_back(Laser_Objects.object[j].id);
		}
	}
	//Now we can find the non calculated Caemra objects
	for (int k = 0; k < Camera_Objects.number_of_objects; k++)
	{
		if (!findAssociatedCamera(Camera_Objects.object[k].id))
		{
			nonCameraJoined.push_back(Camera_Objects.object[k].id);
		}
	}
}

void MAPSJoin::WriteOutputs()
{
	_ioOutput = StartWriting(Output("LaserObjects"));
	AUTO_Objects &LaserObj = *static_cast<AUTO_Objects*>(_ioOutput->Data());
	LaserObj = Laser_Objects;
	StopWriting(_ioOutput);

	_ioOutput = StartWriting(Output("CameraObjects"));
	AUTO_Objects &CameraObj = *static_cast<AUTO_Objects*>(_ioOutput->Data());
	CameraObj = Camera_Objects;
	StopWriting(_ioOutput);

	_ioOutput = StartWriting(Output("AObjects"));
	AssociatedObjs &AssociatedObj = *static_cast<AssociatedObjs*>(_ioOutput->Data());
	AssociatedObj = joined;
	StopWriting(_ioOutput);

	_ioOutput = StartWriting(Output("NALaser"));
	NonAssociated &NALObj = *static_cast<NonAssociated*>(_ioOutput->Data());
	NALObj = nonLaserJoined;
	StopWriting(_ioOutput);

	_ioOutput = StartWriting(Output("NACamera"));
	NonAssociated &NACObj = *static_cast<NonAssociated*>(_ioOutput->Data());
	NACObj = nonCameraJoined;
	StopWriting(_ioOutput);
}

int MAPSJoin::calculateScore(int id_Laser, int id_Camera)
{

	//TODO::Calculate score
	return 30;
}

int MAPSJoin::findPositionObject(int id, AUTO_Objects * objects)
{
	for (int i = 0; i < objects->number_of_objects; i++)
	{
		if (objects->object[i].id == id)
		{
			return i;
		}
	}
	return -1;
}

bool MAPSJoin::findAssociatedLaser(int id_Laser)
{
	for (int i = 0; i < joined.size(); i++)
	{
		if (joined.vector[i][0] == id_Laser)
		{
			return true;
		}
	}
	return false;
}

bool MAPSJoin::findAssociatedCamera(int id_Camera)
{
	for (int i = 0; i < joined.size(); i++)
	{
		if (joined.vector[i][1] == id_Camera)
		{
			return true;
		}
	}
	return false;
}

void MAPSJoin::cleanAssociatedList()
{
	for (int i = 0; i < joined.size(); i++)
	{
		if (joined.vector[i][1] == -1)
		{
			joined.erase((int)i);
			i--;
		}
		else if(joined.vector[i][2] < MIN_SCORE)
		{
			joined.erase((int)i);
			i--;
		}
	}
}

void MAPSJoin::cleanStructures()
{
	joined.clear();
	nonLaserJoined.clear();
	nonCameraJoined.clear();
}

void MAPSJoin::addAssociation(int posLaser, int idCam, int score)
{
	int i = 0;
	while (MatrixOfAssociations[posLaser][i][0] != -1) { i++; }
	MatrixOfAssociations[posLaser][i][0] = idCam;
	MatrixOfAssociations[posLaser][i][1] = score;
}

void MAPSJoin::shortMatrixAssociations()
{
	//Ordenar la matriz de asociaciones de mayor a menor por score
	int id, score,k;
	for (int i = 0; i < AUTO_MAX_NUM_OBJECTS; i++)
	{
		for (int j = 0; j < AUTO_MAX_NUM_OBJECTS; j++)
		{
			id = MatrixOfAssociations[i][j][0];
			score = MatrixOfAssociations[i][j][1];
			k = j;
			while (k > 0 && MatrixOfAssociations[i][k - 1][1] < score) {
				MatrixOfAssociations[i][k][0] = MatrixOfAssociations[i][k - 1][0];
				MatrixOfAssociations[i][k][1] = MatrixOfAssociations[i][k - 1][1];
				k--;
			}
			MatrixOfAssociations[i][k][0] = id;
			MatrixOfAssociations[i][k][1] = score;
		}
	}
}

void MAPSJoin::selectAssociations()
{
	//TODO:Buscar la manera de hacer asociaciones sin ambiguedades
	shortMatrixAssociations();

}

void MAPSJoin::cleanAssociationMatrix()
{
	for (int i = 0; i < AUTO_MAX_NUM_OBJECTS; i++)
	{
		for (int j = 0; j < AUTO_MAX_NUM_OBJECTS; j++)
		{
			MatrixOfAssociations[i][j][0] = -1;
			MatrixOfAssociations[i][j][1] = 0;
		}
	}
}
