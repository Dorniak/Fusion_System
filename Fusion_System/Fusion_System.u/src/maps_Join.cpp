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
	for (int i = 0; i < 4; i++) {
		readed[i] = false;
	}
	numInputs = 4;
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

	/*
	str << '\n';
	str << "Inputs:";
	str << '\n' << "Laser: " << Laser_Objects.number_of_objects << " Camera: " << Camera_Objects.number_of_objects;
	str << '\n' << "Outputs:";
	str << '\n' << "Associated objects: " << joined.size();
	str << '\n' << "Non Laser Associated objects: " << nonLaserJoined.size();
	str << '\n' << "Non Camera Associated objects: " << nonCameraJoined.size();
	*/
	PrintAssociations();
	PrintPossibleAssociations();
	//ReportInfo(str);
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
		ArrayLaserObjects = *input_Laser_Objects;
		readed[0] = true;
		break;
	case 1:
		input_Camera_Objects = static_cast<AUTO_Objects*>(ioeltin->Data());
		ArrayCameraObjects = *input_Camera_Objects;
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
			score = calculateScore(Laser_Matched.id[i], Laser_Matched.Matrix_matched[i][j][0]);
			if (score >= 0) 
			{
				addAssociation(i, Laser_Matched.Matrix_matched[i][j][0], score);
			}
		}
	}
	selectAssociations();
	//At the end of this for we will have a list of asociated objects
	cleanAssociatedList();
	//Now we can find the non calculated Laser objects
	for (int j = 0; j < ArrayLaserObjects.number_of_objects; j++)
	{
		if (!findAssociatedLaser(ArrayLaserObjects.object[j].id))
		{
			nonLaserJoined.push_back(ArrayLaserObjects.object[j].id);
		}
	}
	//Now we can find the non calculated Caemra objects
	for (int k = 0; k < ArrayCameraObjects.number_of_objects; k++)
	{
		if (!findAssociatedCamera(ArrayCameraObjects.object[k].id))
		{
			nonCameraJoined.push_back(ArrayCameraObjects.object[k].id);
		}
	}
}

void MAPSJoin::WriteOutputs()
{
	_ioOutput = StartWriting(Output("LaserObjects"));
	AUTO_Objects &LaserObj = *static_cast<AUTO_Objects*>(_ioOutput->Data());
	LaserObj = ArrayLaserObjects;
	StopWriting(_ioOutput);

	_ioOutput = StartWriting(Output("CameraObjects"));
	AUTO_Objects &CameraObj = *static_cast<AUTO_Objects*>(_ioOutput->Data());
	CameraObj = ArrayCameraObjects;
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

void MAPSJoin::PrintAssociations()
{
	str << '\n' << "Associations" << '\n';
	for (int i = 0; i < joined.number_objects; i++)
	{
		str << "L: " << joined.vector[i][0] << " C: " << joined.vector[i][1] << " S: " << joined.vector[i][2] << '\n';
	}
}

void MAPSJoin::PrintPossibleAssociations()
{
	str << '\n' << "Possible Associations" << '\n';
	//MatrixOfAssociations
	for (int i = 0; i < ArrayLaserObjects.number_of_objects; i++)
	{
		str << "Laser ID: " << Laser_Matched.id[i] << '\n';
		for (int j = 0; j < Laser_Matched.number_matched[i]; j++)
		{
			str << "C: " << MatrixOfAssociations[i][j][0] << " S: " << MatrixOfAssociations[i][j][1] << '\n';
		}
	}
}

int MAPSJoin::calculateScore(int id_Laser, int id_Camera)
{
	float score;//[0,100]
	float score_type(0), score_pos(0), score_size(0), score_speed(0), score_overlap(0);//[0,100]
	float score_centered(0);

	//Calculate the scores
	AUTO_Object object_Laser, object_Camera;
	object_Laser = ArrayLaserObjects.object[findPositionObject(id_Laser, &ArrayLaserObjects)];
	object_Camera = ArrayCameraObjects.object[findPositionObject(id_Camera, &ArrayCameraObjects)];
	if (compatibles(object_Laser.object_class, object_Camera.object_class))
	{
		score_type = calcScoreType(&object_Laser, &object_Camera);
		score_pos = calcScorePos(&object_Laser, &object_Camera);
		score_size = calcScoreSize(&object_Laser, &object_Camera);
		score_speed = calcScoreSpeed(&object_Laser, &object_Camera);
		score_overlap = getOverlapScore(&object_Laser, &object_Camera);
		score_centered = calcScoreCenter(&object_Laser, &object_Camera);
		//TODO:Revisar los -1 en las posibles asociaciones

		//Ready
		//-Type
		//-Position
		//-Speed
		//-Overlap
		//-Centered

		//Left
		//-size

		score = (float)(score_type*weight_type + score_pos*weight_pos + score_size*weight_size + score_speed*weight_speed + score_overlap*weight_over + score_centered*weight_center);

		score = floor(score);
		return (int)score;
	}
	else return 0;//Son tipos no compatibles
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
	cleanAssociationMatrix();
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
	int id, score, k, j;
	//Recorres la lista de objetos del laser
	for (int i = 0; i < ArrayLaserObjects.number_of_objects; i++)
	{
		j = 0;
		//Recorres la lista de un objeto 
		while (MatrixOfAssociations[i][j][0] != -1 && j < AUTO_MAX_NUM_OBJECTS) {
			id = MatrixOfAssociations[i][j][0];
			score = MatrixOfAssociations[i][j][1];
			k = j;
			//Elevas el objeto hasta su posicion
			while (k > 0 && MatrixOfAssociations[i][k - 1][1] < score) {
				MatrixOfAssociations[i][k][0] = MatrixOfAssociations[i][k - 1][0];
				MatrixOfAssociations[i][k][1] = MatrixOfAssociations[i][k - 1][1];
				k--;
			}
			MatrixOfAssociations[i][k][0] = id;
			MatrixOfAssociations[i][k][1] = score;

			j++;
		}
	}
}

void MAPSJoin::selectAssociations()
{
	int posAmb;
	shortMatrixAssociations();
	for (int i = 0; i < ArrayLaserObjects.number_of_objects; i++)
	{
		//No hay que copiar el id del laser por que ya estaba antes
		joined.vector[i][1] = MatrixOfAssociations[i][0][0];//Copiar el id de la camara 
		joined.vector[i][2] = MatrixOfAssociations[i][0][1];//Copiar el score
	}
	do
	{
		while (findAmbiguities())
		{
			for (int i = 0; i < joined.size(); i++)
			{
				posAmb = findAmbiguities(i);
				if (posAmb != -1) {
					if (joined.vector[i][2] >= joined.vector[posAmb][2])
					{
						selectNextAssociation(posAmb);
					}
					else
					{
						selectNextAssociation(i);
					}
				}
			}
		}
	} while (lastCheck());
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

bool MAPSJoin::findAmbiguities()
{
	for (int i = 0; i < joined.size(); i++)
	{
		if (joined.vector[i][1] == -1)
		{
			continue;
		}
		for (int j = i + 1; j < joined.size(); j++)
		{
			if (joined.vector[j][1] != -1 && joined.vector[i][1] == joined.vector[j][1])
			{
				return true;
			}
		}
	}

	return false;
}

int MAPSJoin::findAmbiguities(int pos)
{
	int id = joined.vector[pos][1];//id de camara
	if (id != -1) 
	{
		for (int i = pos + 1; i < joined.size(); i++)
		{
			if (joined.vector[i][1] != -1 && id == joined.vector[i][1])
			{
				return i;
			}
		}
	}

	return -1;
}

void MAPSJoin::selectNextAssociation(int pos)
{
	//Seleccionar la siguiente asociacion posible y copiarla al vector de asociaciones
	int id_actual;
	id_actual = joined.vector[pos][1];
	//MatrixOfAssociations
	int i(0);
	while (i < AUTO_MAX_NUM_OBJECTS && (MatrixOfAssociations[pos][i][0] != id_actual && MatrixOfAssociations[pos][i][0] != -1))
	{
		i++;
	}
	//Significa que soy el ultimo
	if (i >= AUTO_MAX_NUM_OBJECTS - 1 || MatrixOfAssociations[pos][i + 1][0] == -1)
	{
		joined.vector[pos][1] = -1;
		joined.vector[pos][2] = 0;
	}//Significa que no eres el ultimo en el array y hay mas posibilidades
	else
	{
		joined.vector[pos][1] = MatrixOfAssociations[pos][i + 1][0];//Id de la camara
		joined.vector[pos][2] = MatrixOfAssociations[pos][i + 1][1];//score
	}
}

bool MAPSJoin::lastCheck()
{
	int j;
	bool changes = false;
	//si ha habido algun cambio se devuelve true
	for (int i = 0; i < Laser_Matched.number_objects; i++)
	{
		j = 0;
		//Buscar mi posicion
		while (MatrixOfAssociations[i][j][0] != joined.vector[i][1])//Buscar tu id en tu lista de posibilidades
		{
			j++;
		}
		for (int k = 0; k < j; k++)
		{
			if (!IsAssociated(MatrixOfAssociations[i][k][0]))
			{//Se ha dado el caso de que con la resolucion de ambiguedades se ha quedado una asociacion con mayor score libre
				joined.vector[i][1] = MatrixOfAssociations[i][k][0];
				joined.vector[i][2] = MatrixOfAssociations[i][k][1];
				changes = true;
			}
		}
	}

	if (changes)
	{
		return true;
	}
	else return false;
}

bool MAPSJoin::IsAssociated(int id)
{
	for (int i = 0; i < joined.size(); i++)
	{
		if (joined.vector[i][1] == id)
		{
			return true;//Ese id ya esta asociado
		}
	}

	return false;//Ese id aun no esta asociado
}

float MAPSJoin::calcScoreType(AUTO_Object * Object_Laser, AUTO_Object * Object_Camera)
{
	//Definimos 3 opciones
	//Que los dos objetos tengan un tipo definido y sea el mismo 100%
	//Que los dos objetos tengan tipos incompatibles 0%
	//Que los dos objetos tengan distintos tipos pero compatibles
	if (Object_Laser->object_class == Object_Camera->object_class && Object_Laser->object_class != 0)
	{
		return Object_Laser->class_confidence;
	}
	else if (compatibles(Object_Laser->object_class, Object_Camera->object_class))
	{
		return 50;
	}

	else return 0;
}

float MAPSJoin::calcScorePos(AUTO_Object * Object_Laser, AUTO_Object * Object_Camera)
{
	Point2D Pos_Laser, Sigma_Laser, Pos_Camera, Sigma_Camera;
	BOUNDIG_BOX Position_Laser, Position_Camera;
	float32_t I_U;

	//Lo guardamos en forma de punto
	Pos_Laser = Point2D(Object_Laser->x_rel, Object_Laser->y_rel);
	Sigma_Laser = Point2D(Object_Laser->x_sigma, Object_Laser->y_sigma);
	Pos_Camera = Point2D(Object_Camera->x_rel, Object_Camera->y_rel);
	Sigma_Camera = Point2D(Object_Camera->x_sigma, Object_Camera->y_sigma);
	//Calculamos las BBox
	Position_Laser = BOUNDIG_BOX(Pos_Laser, Sigma_Laser);
	Position_Camera = BOUNDIG_BOX(Pos_Camera, Sigma_Camera);

	I_U = (Position_Laser.intersection(Position_Camera).area()) / Position_Laser.Union_area(Position_Camera);
	I_U *= 100;

	return (float)I_U;
}

float MAPSJoin::calcScoreSize(AUTO_Object * Object_Laser, AUTO_Object * Object_Camera)
{
	return 0.0f;
}

float MAPSJoin::calcScoreSpeed(AUTO_Object * Object_Laser, AUTO_Object * Object_Camera)
{
	Point2D Speed_Laser, Sigma_Laser, Speed_Camera;
	BOUNDIG_BOX Speed_Laser_Box;

	//Vector Speed Laser
	Speed_Laser = Point2D(Object_Laser->x_rel, Object_Laser->y_rel);
	Sigma_Laser = Point2D(Object_Laser->x_sigma, Object_Laser->y_sigma);

	//Vector speed Camera
	//Speed sigma is not working for the camera
	Speed_Camera = Point2D(Object_Camera->x_rel, Object_Camera->y_rel);

	Speed_Laser_Box = BOUNDIG_BOX(Speed_Laser, Sigma_Laser);

	if (Speed_Laser_Box.isInside(Speed_Camera))
	{
		return 100;
	}
	else return 0;
}

float MAPSJoin::calcScoreCenter(AUTO_Object * Object_Laser, AUTO_Object * Object_Camera)
{
	Point2D centerL,centerC,sigmaL;
	BOUNDIG_BOX BBoxLAmpli;
	float32_t distanceMax, dist;

	centerL = Point2D(Object_Laser->x_rel, Object_Laser->y_rel);
	centerC = Point2D(Object_Camera->x_rel, Object_Camera->y_rel);
	sigmaL = Point2D(Object_Laser->x_sigma, Object_Laser->y_sigma);

	BBoxLAmpli = BOUNDIG_BOX(Object_Laser);
	BBoxLAmpli.ampliate(sigmaL);

	distanceMax = centerL.dist(BBoxLAmpli.point[0]);//Da igual que punto
	dist = centerL.dist(centerC);

	if (dist > distanceMax)
	{
		return 0;
	}
	else if (dist == 0)
	{
		return 100;
	}
	else
	{
		return (1 / (distanceMax / dist)) * 100;//[0,100]
	}
}

float MAPSJoin::getOverlapScore(AUTO_Object * Object_Laser, AUTO_Object * Object_Camera)
{
	return (float)Laser_Matched.Matrix_matched[findID(Object_Laser->id, &Laser_Matched)][findID(Object_Laser->id, Object_Camera->id, &Laser_Matched)][1];
}

int MAPSJoin::findID(int id_object, MATCH_OBJECTS * vector)
{
	if (id_object == -1 || vector->number_objects>AUTO_MAX_NUM_OBJECTS)
	{
		return -1;//Only for control but actually should be implossible 
	}
	for (int i = 0; i < vector->number_objects; i++)
	{
		if (vector->id[i] == -1)
		{
			return -1;//Only for control but actually should be implossible 
		}
		else if (vector->id[i] == id_object)
		{
			return i;
		}
	}

	return -1;
}

int MAPSJoin::findID(int id_object, int id_target, MATCH_OBJECTS * vector)
{
	int pos = findID(id_object, vector);

	if (pos == -1 || id_target == -1)
	{
		return -1;//Only for control but actually should be implossible 
	}

	for (int j = 0; j < vector->number_matched[pos]; j++)
	{
		if (j > AUTO_MAX_NUM_OBJECTS)
		{
			continue;//Only for control but actually should be implossible 
		}
		if (vector->Matrix_matched[pos][j][0] == id_target)
		{
			return j;
		}
	}

	return -1;
}

bool MAPSJoin::compatibles(AUTO_OBJECT_CLASS classL, AUTO_OBJECT_CLASS classC)
{
	if (classL == 0 || classC == 0)
	{
		return true;
	}
	switch (classL)
	{
	case 0:
		return true;
		break;
	case 1:
		if (classC == 1 || classC == 2)
		{
			return true;
		}
		else
		{
			return false;
		}
		break;
	case 2:
		if (classC == 1 || classC == 2)
		{
			return true;
		}
		else
		{
			return false;
		}
		break;
	case 3:
		if (classC == 3 || classC == 4)
		{
			return true;
		}
		else
		{
			return false;
		}
		break;
	case 4:
		if (classC == 3 || classC == 4)
		{
			return true;
		}
		else 
		{
			return false;
		}
		break;
	default:
		return false;
		break;
	}
}