////////////////////////////////
// RTMaps SDK Component
////////////////////////////////

////////////////////////////////
// Purpose of this module :
////////////////////////////////

#include "maps_Bayesian_Fusion.h"	// Includes the header of this component

const MAPSTypeFilterBase ValeoStructure = MAPS_FILTER_USER_STRUCTURE(AUTO_Objects);
// Use the macros to declare the inputs
MAPS_BEGIN_INPUTS_DEFINITION(MAPSBayesian_Fusion)
    //MAPS_INPUT("iName",MAPS::FilterInteger32,MAPS::FifoReader)
	MAPS_INPUT("LaserObject", ValeoStructure, MAPS::FifoReader)
	MAPS_INPUT("CameraObject", ValeoStructure, MAPS::FifoReader)
MAPS_END_INPUTS_DEFINITION

// Use the macros to declare the outputs
MAPS_BEGIN_OUTPUTS_DEFINITION(MAPSBayesian_Fusion)
    //MAPS_OUTPUT("oName",MAPS::Integer32,NULL,NULL,1)
	MAPS_OUTPUT_USER_STRUCTURE("Output_estimation", AUTO_Objects)
MAPS_END_OUTPUTS_DEFINITION

// Use the macros to declare the properties
MAPS_BEGIN_PROPERTIES_DEFINITION(MAPSBayesian_Fusion)
	MAPS_PROPERTY("framerate", 100, false, true)
    //MAPS_PROPERTY("pName",128,false,false)
MAPS_END_PROPERTIES_DEFINITION

// Use the macros to declare the actions
MAPS_BEGIN_ACTIONS_DEFINITION(MAPSBayesian_Fusion)
    //MAPS_ACTION("aName",MAPSBayesian_Fusion::ActionName)
MAPS_END_ACTIONS_DEFINITION

// Use the macros to declare this component (Bayesian_Fusion) behaviour
MAPS_COMPONENT_DEFINITION(MAPSBayesian_Fusion,"Bayesian_Fusion","1.0",128,
			  MAPS::Threaded,MAPS::Threaded,
			  -1, // Nb of inputs. Leave -1 to use the number of declared input definitions
			  -1, // Nb of outputs. Leave -1 to use the number of declared output definitions
			  -1, // Nb of properties. Leave -1 to use the number of declared property definitions
			  -1) // Nb of actions. Leave -1 to use the number of declared action definitions

//Initialization: Birth() will be called once at diagram execution startup.			  
void MAPSBayesian_Fusion::Birth()
{
	QueryPerformanceFrequency(&Frecuency);
	QueryPerformanceCounter(&StartingTime);
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
void MAPSBayesian_Fusion::Core() 
{
   Predictor();
}

//De-initialization: Death() will be called once at diagram execution shutdown.
void MAPSBayesian_Fusion::Death()
{
    // Reports this information to the RTMaps console. You can remove this line if you know when Death() is called in the component lifecycle.
    ReportInfo("Passing through Death() method");
}

void MAPSBayesian_Fusion::predecir()
{
	if (abs((int)(timestamp - min(LaserObjects[1].timestamp, CameraObjects[1].timestamp))) > framerate * 10)
	{
		timestamp = min(LaserObjects[1].timestamp, CameraObjects[1].timestamp);
	}
	while (timestamp < max(LaserObjects[1].timestamp, CameraObjects[1].timestamp))
	{
		timestamp += framerate;
	}
	//Predict the position of the obstacles
	if (updated[0])
	{
		ArrayLaserObjects = LaserObjects[1];
		for (size_t i = 0; i < ArrayLaserObjects.number_of_objects; i++)
		{
			int pos = findPosition(LaserObjects[0], ArrayLaserObjects.object[i].id);
			if (pos != -1)
			{
				//Move the obstacle
				moveObstacle(&ArrayLaserObjects.object[i], ArrayLaserObjects.timestamp);
			}
		}

		updated[0] = false;
	}
	if (updated[1])
	{
		ArrayCameraObjects = CameraObjects[1];
		for (size_t i = 0; i < ArrayCameraObjects.number_of_objects; i++)
		{
			int pos = findPosition(CameraObjects[0], ArrayCameraObjects.object[i].id);
			if (pos != -1)
			{
				//Move the obstacle
				moveObstacle(&ArrayCameraObjects.object[i], ArrayCameraObjects.timestamp);
			}
		}

		updated[1] = false;
	}
	predicted = true;
}

int MAPSBayesian_Fusion::findPosition(AUTO_Objects objects, int id)
{
	//Calculate the position of this object in the array
	for (size_t i = 0; i < objects.number_of_objects; i++)
	{
		if (objects.object[i].id == id)
		{
			return i;
		}
	}
	return -1;
}

void MAPSBayesian_Fusion::moveObstacle(AUTO_Object * obstacle, int timestamp)
{
	Point3D_32t vector;
	//Move the position and the bounding box to the predicted position

	vector.x = (float32_t)((obstacle->speed_x_rel / 1000) * (this->timestamp - timestamp));
	vector.y = (float32_t)((obstacle->speed_y_rel / 1000) * (this->timestamp - timestamp));

	obstacle->x_rel += vector.x;
	obstacle->y_rel += vector.y;

	for (size_t i = 0; i < 4; i++)
	{
		obstacle->bounding_box_x_rel[i] += vector.x;
		obstacle->bounding_box_y_rel[i] += vector.y;
	}

	obstacle->distance = (float32_t)sqrt(pow(obstacle->x_rel, 2) + pow(obstacle->y_rel, 2) + pow(obstacle->z_rel, 2));
}

void MAPSBayesian_Fusion::findMatches(AUTO_Objects * ArrayLaser, AUTO_Objects * ArrayCamera)
{
	//We look for coincidences between bounding box of 2 objects
	//We fill the array of id Laser objects
	LaserMatched.number_objects = ArrayLaser->number_of_objects;
	for (int i = 0; i < ArrayLaser->number_of_objects; i++)
	{
		LaserMatched.id[i] = ArrayLaser->object[i].id;
	}
	//We fill the array of id Camera objects
	CameraMatched.number_objects = ArrayCamera->number_of_objects;
	for (int i = 0; i < ArrayCamera->number_of_objects; i++)
	{
		CameraMatched.id[i] = ArrayCamera->object[i].id;
	}
	//Save and print the extended bowndingbox
	output_LaserAmpliatedBox = *ArrayLaser;
	output_CameraAmpliatedBox = *ArrayCamera;
	//Calculate the extended BBox and save it in the output_XAmpliatedBox
	for (int a = 0; a < output_LaserAmpliatedBox.number_of_objects; a++)
	{
		calculateBoundingBox(&output_LaserAmpliatedBox.object[a]);
	}
	for (int b = 0; b < output_CameraAmpliatedBox.number_of_objects; b++)
	{
		calculateBoundingBox(&output_CameraAmpliatedBox.object[b]);
	}

	for (int i = 0; i < ArrayLaser->number_of_objects; i++)
	{
		for (int j = 0; j < ArrayCamera->number_of_objects; j++)
		{
			if (BoxMatching(&output_LaserAmpliatedBox.object[i], &output_CameraAmpliatedBox.object[j]))
			{
				LaserMatched.Matrix_matched[i][LaserMatched.number_matched[i]][0] = ArrayCamera->object[j].id;
				LaserMatched.number_matched[i]++;
				CameraMatched.Matrix_matched[j][CameraMatched.number_matched[j]][0] = ArrayLaser->object[i].id;
				CameraMatched.number_matched[j]++;
				overlap(&output_LaserAmpliatedBox.object[i], &output_CameraAmpliatedBox.object[j]);
			}
		}
	}
}

bool MAPSBayesian_Fusion::BoxMatching(AUTO_Object * Object1, AUTO_Object * Object2)
{
	double x_max(-DBL_MAX), x_min(DBL_MAX), y_max(-DBL_MAX), y_min(DBL_MAX);

	for (int i = 0; i < 4; i++) {
		x_max = max(Object1->bounding_box_x_rel[i], x_max);
		x_min = min(Object1->bounding_box_x_rel[i], x_min);
		y_max = max(Object1->bounding_box_y_rel[i], y_max);
		y_min = min(Object1->bounding_box_y_rel[i], y_min);
	}
	for (int i = 0; i < 4; i++)
	{
		if (Object2->bounding_box_x_rel[i] > x_min && Object2->bounding_box_x_rel[i] < x_max)
		{
			if (Object2->bounding_box_y_rel[i] > y_min && Object2->bounding_box_y_rel[i] < y_max)
			{
				return true;
			}
		}
	}

	//Calculate if the center is insaid of the bounding box
	if (Object2->x_rel > x_min && Object2->x_rel < x_max)
	{
		if (Object2->y_rel > y_min && Object2->y_rel < y_max)
		{
			return true;
		}
	}

	//Upsaid down

	double x_max2(-DBL_MAX), x_min2(DBL_MAX), y_max2(-DBL_MAX), y_min2(DBL_MAX);

	for (int i = 0; i < 4; i++) {
		x_max2 = max(Object2->bounding_box_x_rel[i], x_max2);
		x_min2 = min(Object2->bounding_box_x_rel[i], x_min2);
		y_max2 = max(Object2->bounding_box_y_rel[i], y_max2);
		y_min2 = min(Object2->bounding_box_y_rel[i], y_min2);
	}
	for (int i = 0; i < 4; i++)
	{
		if (Object1->bounding_box_x_rel[i] > x_min2 && Object1->bounding_box_x_rel[i] < x_max2)
		{
			if (Object1->bounding_box_y_rel[i] > y_min2 && Object1->bounding_box_y_rel[i] < y_max2)
			{
				return true;
			}
		}
	}

	//Calculate if the center is insaid of the bounding box
	if (Object1->x_rel > x_min2 && Object1->x_rel < x_max2)
	{
		if (Object1->y_rel > y_min2 && Object1->y_rel < y_max2)
		{
			return true;
		}
	}

	return false;
}

void MAPSBayesian_Fusion::copyBBox(BOUNDIG_BOX BBox, AUTO_Object * Output_ampliated)
{
	//This is only to see the extended bounding box in the bird view
	for (size_t i = 0; i < 4; i++)
	{
		Output_ampliated->bounding_box_x_rel[i] = BBox.point[i].x;
		Output_ampliated->bounding_box_y_rel[i] = BBox.point[i].y;
	}
}

void MAPSBayesian_Fusion::calculateBoundingBox(AUTO_Object * Object)
{
	BOUNDIG_BOX original_ampliated;
	BOUNDIG_BOX ampliated_Lrotated;
	BOUNDIG_BOX ampliated_Rrotated;
	BOUNDIG_BOX final_BowndingBox;

#pragma region Calculate Original_ampliated
#pragma region Copiar a local
	original_ampliated = BOUNDIG_BOX(Object);
#pragma endregion

#pragma region ampliacion
	ampliarBowndingBox(&original_ampliated, Object->x_sigma, Object->y_sigma);
#pragma endregion
#pragma endregion

	ampliated_Lrotated = original_ampliated;
	ampliated_Rrotated = original_ampliated;
	//Center Bowndingbox to 0
	trasladarBowndingBox(&ampliated_Lrotated, -Object->x_rel, -Object->y_rel);
	//Rotate BowndingBox
	ampliated_Lrotated.rote(-Object->yaw_sigma);
	//Return to the real position
	trasladarBowndingBox(&ampliated_Lrotated, Object->x_rel, Object->y_rel);
	trasladarBowndingBox(&ampliated_Rrotated, -Object->x_rel, -Object->y_rel);
	ampliated_Rrotated.rote(Object->yaw_sigma);
	trasladarBowndingBox(&ampliated_Rrotated, Object->x_rel, Object->y_rel);

	final_BowndingBox = finalBox(original_ampliated, ampliated_Lrotated, ampliated_Rrotated);
	copyBBox(final_BowndingBox, Object);
}

void MAPSBayesian_Fusion::trasladarBowndingBox(BOUNDIG_BOX * entrada, double x, double y)
{
	//Move bounding box
	entrada->point[0].x = entrada->point[0].x + (float32_t)x;
	entrada->point[0].y = entrada->point[0].y + (float32_t)y;

	entrada->point[1].x = entrada->point[1].x + (float32_t)x;
	entrada->point[1].y = entrada->point[1].y + (float32_t)y;

	entrada->point[2].x = entrada->point[2].x + (float32_t)x;
	entrada->point[2].y = entrada->point[2].y + (float32_t)y;

	entrada->point[3].x = entrada->point[3].x + (float32_t)x;
	entrada->point[3].y = entrada->point[3].y + (float32_t)y;
}

void MAPSBayesian_Fusion::ampliarBowndingBox(BOUNDIG_BOX * entrada, double x, double y)
{
	//Ampliate bownding box
	entrada->point[0].x = entrada->point[0].x + (float32_t)x;
	entrada->point[0].y = entrada->point[0].y - (float32_t)y;

	entrada->point[1].x = entrada->point[1].x + (float32_t)x;
	entrada->point[1].y = entrada->point[1].y + (float32_t)y;

	entrada->point[2].x = entrada->point[2].x - (float32_t)x;
	entrada->point[2].y = entrada->point[2].y + (float32_t)y;

	entrada->point[3].x = entrada->point[3].x - (float32_t)x;
	entrada->point[3].y = entrada->point[3].y - (float32_t)y;
}

BOUNDIG_BOX MAPSBayesian_Fusion::finalBox(BOUNDIG_BOX original, BOUNDIG_BOX Lrotated, BOUNDIG_BOX Rrotated)
{
	//Calculate the final extended bounding box
	BOUNDIG_BOX finalBox;
	float32_t x_min, x_max, y_min, y_max;
	float32_t x_min_original, x_max_original, y_min_original, y_max_original;
	float32_t x_min_Lrotated, x_max_Lrotated, y_min_Lrotated, y_max_Lrotated;
	float32_t x_min_Rrotated, x_max_Rrotated, y_min_Rrotated, y_max_Rrotated;

	x_min_original = min(min(original.point[0].x, original.point[1].x), min(original.point[2].x, original.point[3].x));
	x_min_Lrotated = min(min(Lrotated.point[0].x, Lrotated.point[1].x), min(Lrotated.point[2].x, Lrotated.point[3].x));
	x_min_Rrotated = min(min(Rrotated.point[0].x, Rrotated.point[1].x), min(Rrotated.point[2].x, Rrotated.point[3].x));

	x_max_original = max(max(original.point[0].x, original.point[1].x), max(original.point[2].x, original.point[3].x));
	x_max_Lrotated = max(max(Lrotated.point[0].x, Lrotated.point[1].x), max(Lrotated.point[2].x, Lrotated.point[3].x));
	x_max_Rrotated = max(max(Rrotated.point[0].x, Rrotated.point[1].x), max(Rrotated.point[2].x, Rrotated.point[3].x));

	y_min_original = min(min(original.point[0].y, original.point[1].y), min(original.point[2].y, original.point[3].y));
	y_min_Lrotated = min(min(Lrotated.point[0].y, Lrotated.point[1].y), min(Lrotated.point[2].y, Lrotated.point[3].y));
	y_min_Rrotated = min(min(Rrotated.point[0].y, Rrotated.point[1].y), min(Rrotated.point[2].y, Rrotated.point[3].y));

	y_max_original = max(max(original.point[0].y, original.point[1].y), max(original.point[2].y, original.point[3].y));
	y_max_Lrotated = max(max(Lrotated.point[0].y, Lrotated.point[1].y), max(Lrotated.point[2].y, Lrotated.point[3].y));
	y_max_Rrotated = max(max(Rrotated.point[0].y, Rrotated.point[1].y), max(Rrotated.point[2].y, Rrotated.point[3].y));

	x_min = min(min(x_min_original, x_min_Lrotated), x_min_Rrotated);
	x_max = max(max(x_max_original, x_max_Lrotated), x_max_Rrotated);
	y_min = min(min(y_min_original, y_min_Lrotated), y_min_Rrotated);
	y_max = max(max(y_max_original, y_max_Lrotated), y_max_Rrotated);

	finalBox.point[0].x = x_max;
	finalBox.point[0].y = y_min;

	finalBox.point[1].x = x_max;
	finalBox.point[1].y = y_max;

	finalBox.point[2].x = x_min;
	finalBox.point[2].y = y_max;

	finalBox.point[3].x = x_min;
	finalBox.point[3].y = y_min;

	return finalBox;
}

void MAPSBayesian_Fusion::clear_Matched()
{
	//Clear the arrays of the matched objects
	for (int i = 0; i < AUTO_MAX_NUM_OBJECTS; i++)
	{
		for (int j = 0; j < AUTO_MAX_NUM_OBJECTS; j++)
		{
			LaserMatched.Matrix_matched[i][j][0] = -1;//id camara
			CameraMatched.Matrix_matched[i][j][0] = -1;//id laser
			LaserMatched.Matrix_matched[i][j][1] = 0;//score
			CameraMatched.Matrix_matched[i][j][1] = 0;//score
		}
		LaserMatched.number_matched[i] = 0;//Delete associations
		CameraMatched.number_matched[i] = 0;//Delete associations
		LaserMatched.id[i] = -1;//Delete ids
		CameraMatched.id[i] = -1;//Delete ids
	}
	LaserMatched.number_objects = 0;//Delete number of objects
	CameraMatched.number_objects = 0;//Delete number of objects
}

void MAPSBayesian_Fusion::overlap(AUTO_Object * objeto1, AUTO_Object * objeto2)
{
	BOUNDIG_BOX intersection, BBox1, BBox2;
	BBox1 = BOUNDIG_BOX(objeto1);
	BBox2 = BOUNDIG_BOX(objeto2);

	intersection = BBox1.intersection(BBox2);

	LaserMatched.Matrix_matched[findID(objeto1->id, &LaserMatched)][findID(objeto1->id, objeto2->id, &LaserMatched)][1] = calcIU(objeto1, objeto2, &intersection);
	CameraMatched.Matrix_matched[findID(objeto2->id, &CameraMatched)][findID(objeto2->id, objeto1->id, &CameraMatched)][1] = LaserMatched.Matrix_matched[findID(objeto1->id, &LaserMatched)][findID(objeto1->id, objeto2->id, &LaserMatched)][1];
}

int MAPSBayesian_Fusion::findID(int id_object, MATCH_OBJECTS * vector)
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

int MAPSBayesian_Fusion::findID(int id_object, int id_target, MATCH_OBJECTS * vector)
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

int MAPSBayesian_Fusion::calcIU(AUTO_Object * objet1, AUTO_Object * objet2, BOUNDIG_BOX * BBoxInter)
{
	double area1, area2, areaI, areaU, areaFinal;
	BOUNDIG_BOX obj1, obj2;

	obj1 = BOUNDIG_BOX(objet1);
	obj2 = BOUNDIG_BOX(objet2);
	area1 = obj1.area();
	area2 = obj2.area();
	areaI = BBoxInter->area();
	areaU = area1 + area2 - areaI;
	areaFinal = (areaI / areaU) * 100;//[0,100]
	areaFinal = round(areaFinal);

	return (int)areaFinal;
}

void MAPSBayesian_Fusion::PrintAssociations()
{
	str << '\n' << "Associations" << '\n';
	for (int i = 0; i < joined.number_objects; i++)
	{
		str << "L: " << joined.vector[i][0] << " C: " << joined.vector[i][1] << " S: " << joined.vector[i][2] << '\n';
	}
}

void MAPSBayesian_Fusion::PrintPossibleAssociations()
{
	str << '\n' << "Possible Associations" << '\n';
	//MatrixOfAssociations
	for (int i = 0; i < ArrayLaserObjects.number_of_objects; i++)
	{
		str << "Laser ID: " << LaserMatched.id[i] << '\n';
		for (int j = 0; j < LaserMatched.number_matched[i]; j++)
		{
			str << "C: " << MatrixOfAssociations[i][j][0] << " S: " << MatrixOfAssociations[i][j][1] << '\n';
		}
	}
}

int MAPSBayesian_Fusion::calculateScore(int id_Laser, int id_Camera)
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
		//Ready
		//-Type
		//-Position
		//-Speed
		//-Overlap
		//-Centered

		//Left
		//-size

		score = (float)(score_type*weight_type + score_pos*weight_pos + score_size*weight_size + score_speed*weight_speed + score_overlap*weight_over + score_centered*weight_center);

		score = round(score);
		return (int)score;
	}
	else return 0;//They are not compatibles
}

int MAPSBayesian_Fusion::findPositionObject(int id, AUTO_Objects * objects)
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

bool MAPSBayesian_Fusion::findAssociatedLaser(int id_Laser)
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

bool MAPSBayesian_Fusion::findAssociatedCamera(int id_Camera)
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

void MAPSBayesian_Fusion::cleanAssociatedList()
{
	for (int i = 0; i < joined.size(); i++)
	{
		if (joined.vector[i][1] == -1)
		{
			joined.erase((int)i);
			i--;
		}
		else if (joined.vector[i][2] < MIN_SCORE)
		{
			joined.erase((int)i);
			i--;
		}
	}
}

void MAPSBayesian_Fusion::cleanStructures()
{
	joined.clear();
	nonLaserJoined.clear();
	nonCameraJoined.clear();
	cleanAssociationMatrix();
}

void MAPSBayesian_Fusion::addAssociation(int posLaser, int idCam, int score)
{
	int i = 0;
	while (MatrixOfAssociations[posLaser][i][0] != -1) { i++; }
	MatrixOfAssociations[posLaser][i][0] = idCam;
	MatrixOfAssociations[posLaser][i][1] = score;
}

void MAPSBayesian_Fusion::shortMatrixAssociations()
{
	//Short matrix by score
	int id, score, k, j;
	//Go over the list of Laser objects
	for (int i = 0; i < ArrayLaserObjects.number_of_objects; i++)
	{
		j = 0;
		//Go over the list of one object
		while (MatrixOfAssociations[i][j][0] != -1 && j < AUTO_MAX_NUM_OBJECTS) {
			id = MatrixOfAssociations[i][j][0];
			score = MatrixOfAssociations[i][j][1];
			k = j;
			//Grow up the objects to their position
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

void MAPSBayesian_Fusion::selectAssociations()
{
	int posAmb;
	shortMatrixAssociations();
	for (int i = 0; i < ArrayLaserObjects.number_of_objects; i++)
	{
		//Is not necesary to copy the Laser id because is already here
		joined.vector[i][1] = MatrixOfAssociations[i][0][0];//Copy camera id
		joined.vector[i][2] = MatrixOfAssociations[i][0][1];//Copy score id
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

void MAPSBayesian_Fusion::cleanAssociationMatrix()
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

bool MAPSBayesian_Fusion::findAmbiguities()
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

int MAPSBayesian_Fusion::findAmbiguities(int pos)
{
	int id = joined.vector[pos][1];//Camera id
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

void MAPSBayesian_Fusion::selectNextAssociation(int pos)
{
	//Select the next possible association and copy it to the associations vector
	int id_actual;
	id_actual = joined.vector[pos][1];
	//MatrixOfAssociations
	int i(0);
	while (i < AUTO_MAX_NUM_OBJECTS && (MatrixOfAssociations[pos][i][0] != id_actual && MatrixOfAssociations[pos][i][0] != -1))
	{
		i++;
	}
	//Means that I am the last
	if (i >= AUTO_MAX_NUM_OBJECTS - 1 || MatrixOfAssociations[pos][i + 1][0] == -1)
	{
		joined.vector[pos][1] = -1;
		joined.vector[pos][2] = 0;
	}//I am not the last there are more posibilities
	else
	{
		joined.vector[pos][1] = MatrixOfAssociations[pos][i + 1][0];//Camera id
		joined.vector[pos][2] = MatrixOfAssociations[pos][i + 1][1];//score
	}
}

bool MAPSBayesian_Fusion::lastCheck()
{
	int j;
	bool changes = false;
	//if have been any change return true
	for (int i = 0; i < LaserMatched.number_objects; i++)
	{
		j = 0;
		//Find my position
		while (MatrixOfAssociations[i][j][0] != joined.vector[i][1])//Find your id in the list
		{
			j++;
		}
		for (int k = 0; k < j; k++)
		{
			if (!IsAssociated(MatrixOfAssociations[i][k][0]))
			{//Case of with the movement there are a better association free
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

bool MAPSBayesian_Fusion::IsAssociated(int id)
{
	for (int i = 0; i < joined.size(); i++)
	{
		if (joined.vector[i][1] == id)
		{
			return true;//This id is already associated
		}
	}

	return false;//This id is not already associated
}

float MAPSBayesian_Fusion::calcScoreType(AUTO_Object * Object_Laser, AUTO_Object * Object_Camera)
{
	//3 options
	//Both are the same type
	//They are incompatibles
	//Different types but compatibles
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

float MAPSBayesian_Fusion::calcScorePos(AUTO_Object * Object_Laser, AUTO_Object * Object_Camera)
{
	Point2D Pos_Laser, Sigma_Laser, Pos_Camera, Sigma_Camera;
	BOUNDIG_BOX Position_Laser, Position_Camera;
	float32_t I_U;

	//Save in point structure
	Pos_Laser = Point2D(Object_Laser->x_rel, Object_Laser->y_rel);
	Sigma_Laser = Point2D(Object_Laser->x_sigma, Object_Laser->y_sigma);
	Pos_Camera = Point2D(Object_Camera->x_rel, Object_Camera->y_rel);
	Sigma_Camera = Point2D(Object_Camera->x_sigma, Object_Camera->y_sigma);
	//Calc the bounding box
	Position_Laser = BOUNDIG_BOX(Pos_Laser, Sigma_Laser);
	Position_Camera = BOUNDIG_BOX(Pos_Camera, Sigma_Camera);

	I_U = (Position_Laser.intersection(Position_Camera).area()) / Position_Laser.Union_area(Position_Camera);
	I_U *= 100;

	return (float)I_U;
}

float MAPSBayesian_Fusion::calcScoreSize(AUTO_Object * Object_Laser, AUTO_Object * Object_Camera)
{
	return 0.0f;
}

float MAPSBayesian_Fusion::calcScoreSpeed(AUTO_Object * Object_Laser, AUTO_Object * Object_Camera)
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

float MAPSBayesian_Fusion::calcScoreCenter(AUTO_Object * Object_Laser, AUTO_Object * Object_Camera)
{
	Point2D centerL, centerC, sigmaL;
	BOUNDIG_BOX BBoxLAmpli;
	float32_t distanceMax, dist;

	centerL = Point2D(Object_Laser->x_rel, Object_Laser->y_rel);
	centerC = Point2D(Object_Camera->x_rel, Object_Camera->y_rel);
	sigmaL = Point2D(Object_Laser->x_sigma, Object_Laser->y_sigma);

	BBoxLAmpli = BOUNDIG_BOX(Object_Laser);
	BBoxLAmpli.ampliate(sigmaL);

	distanceMax = centerL.dist(BBoxLAmpli.point[0]);
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

float MAPSBayesian_Fusion::getOverlapScore(AUTO_Object * Object_Laser, AUTO_Object * Object_Camera)
{
	return (float)LaserMatched.Matrix_matched[findID(Object_Laser->id, &LaserMatched)][findID(Object_Laser->id, Object_Camera->id, &LaserMatched)][1];
}

bool MAPSBayesian_Fusion::compatibles(AUTO_OBJECT_CLASS classL, AUTO_OBJECT_CLASS classC)
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

void MAPSBayesian_Fusion::Estimate()
{
	//Add non associated objects
	for (int i = 0; i < nonLaserJoined.size(); i++)
	{
		Estimation.object[Estimation.number_of_objects] = findLaserObj(nonLaserJoined.vector[i]);
		Estimation.object[Estimation.number_of_objects].id = generateIdLas(nonLaserJoined.vector[i]);
		Estimation.number_of_objects++;
	}

	for (int i = 0; i < nonCameraJoined.size(); i++)
	{
		Estimation.object[Estimation.number_of_objects] = findCameraObj(nonCameraJoined.vector[i]);
		Estimation.object[Estimation.number_of_objects].id = generateIdCam(nonCameraJoined.vector[i]);
		Estimation.number_of_objects++;
	}

	//if this association have been see before we associated this id
	//If not we have to generate an id
	//If we saw this objects before but in differents associations we have to clean this associations
	for (int i = 0; i < joined.size(); i++)
	{
		Estimation.object[Estimation.number_of_objects] = calculateObj(&findLaserObj(joined.vector[i][0]), &findCameraObj(joined.vector[i][1]));
		Estimation.object[Estimation.number_of_objects].id = generateIdLC(joined.vector[i][0], joined.vector[i][1]);
		Estimation.number_of_objects++;
	}
}

void MAPSBayesian_Fusion::shortVectorLCA(vector<array<int, 3>> vect)
{
	int idL, idC, newId;
	for (int i = 0; i < (int)vect.size(); i++)
	{
		idL = vect[i][0];
		idC = vect[i][1];
		newId = vect[i][2];

		for (int j = i; j > 0; j--)
		{
			if (vect[j - 1][0] > idL)
			{
				vect[j][0] = vect[j - 1][0];
				vect[j][1] = vect[j - 1][1];
				vect[j][2] = vect[j - 1][2];
			}
			else if (vect[j - 1][0] == idL)
			{
				if (vect[j - 1][1] > idC)
				{
					vect[j][0] = vect[j - 1][0];
					vect[j][1] = vect[j - 1][1];
					vect[j][2] = vect[j - 1][2];
				}
				else
				{
					vect[j][0] = idL;
					vect[j][1] = idC;
					vect[j][2] = newId;
					break;
				}
			}
			else
			{
				vect[j][0] = idL;
				vect[j][1] = idC;
				vect[j][2] = newId;
				break;
			}
		}
	}
}

void MAPSBayesian_Fusion::shortObjects(AUTO_Objects * objects)
{
	AUTO_Object objectaux;
	for (int i = 0; i < objects->number_of_objects; i++)
	{
		objectaux = objects->object[i];
		for (int j = i; j > 0; j--)
		{
			if (objects->object[j - 1].id > objectaux.id)
			{
				objects->object[j] = objects->object[j - 1];
			}
			else
			{
				objects->object[j] = objectaux;
			}
		}
	}
}

int MAPSBayesian_Fusion::generateIdLas(int id)
{
	std::array<int, 3> vect;
	int pos = findAssoc(id, -1);
	if (pos != -1) {
		return LCAssociations[pos][2];
	}
	else
	{
		vect[0] = id;
		vect[1] = -1;
		vect[2] = lastID + 1;
		lastID++;
		LCAssociations.push_back(vect);
		return vect[2];
	}
}

int MAPSBayesian_Fusion::generateIdCam(int id)
{
	std::array<int, 3> vect;
	int pos = findAssoc(-1, id);
	if (pos != -1) {
		return LCAssociations[pos][2];
	}
	else
	{
		vect[0] = -1;
		vect[1] = id;
		vect[2] = lastID + 1;
		lastID++;
		LCAssociations.push_back(vect);
		return vect[2];
	}
}

int MAPSBayesian_Fusion::generateIdLC(int idL, int idC)
{
	std::array<int, 3> vect;
	int pos = findAssoc(idL, idC);
	if (pos != -1) {
		return LCAssociations[pos][2];
	}
	else
	{
		vect[0] = idL;
		vect[1] = idC;
		vect[2] = lastID + 1;
		lastID++;
		LCAssociations.push_back(vect);
		return vect[2];
	}
}

int MAPSBayesian_Fusion::findAssoc(int idL, int idC)
{
	if (idL == -1)
	{
		for (int i = 0; i < (int)LCAssociations.size(); i++)
		{
			if (LCAssociations[i][1] == idC)
			{
				return i;
			}
		}
		return -1;
	}
	else if (idC == -1)
	{
		for (int i = 0; i < (int)LCAssociations.size(); i++)
		{
			if (LCAssociations[i][0] == idL)
			{
				return i;
			}
		}
		return -1;
	}
	else
	{
		for (int i = 0; i < (int)LCAssociations.size(); i++)
		{
			if (LCAssociations[i][0] == idL || LCAssociations[i][1] == idC)
			{
				//Update association
				LCAssociations[i][0] = idL;
				LCAssociations[i][1] = idC;
				return i;
			}
		}
		return -1;
	}
}

AUTO_Object MAPSBayesian_Fusion::findLaserObj(int id)
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

AUTO_Object MAPSBayesian_Fusion::findCameraObj(int id)
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

AUTO_Object MAPSBayesian_Fusion::calculateObj(AUTO_Object * objL, AUTO_Object * objC)
{
	AUTO_Object result;
	float32_t  paramA, sigmaA, paramB, sigmaB;
	float32_t paramResult, sigmaResult;
	//Final function
	initObject(&result);

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
	paramA = objL->length;
	sigmaA = objL->length_sigma;
	paramB = objC->length;
	sigmaB = objC->length_sigma;
	if (sigmaA != 0 && sigmaB != 0) {
		EstimateParameter(paramA, sigmaA, paramB, sigmaB, &paramResult, &sigmaResult);
		result.length = paramResult;
		result.length_sigma = sigmaResult;
	}
	else
	{
		if (sigmaA != 0)
		{
			result.length = paramA;
			result.length_sigma = sigmaA;
		}
		else if (sigmaB != 0)
		{
			result.length = paramB;
			result.length_sigma = sigmaB;
		}
	}
#pragma endregion
	//width
#pragma region width
	paramA = objL->width;
	sigmaA = objL->width_sigma;
	paramB = objC->width;
	sigmaB = objC->width_sigma;
	if (sigmaA != 0 && sigmaB != 0) {
		EstimateParameter(paramA, sigmaA, paramB, sigmaB, &paramResult, &sigmaResult);
		result.width = paramResult;
		result.width_sigma = sigmaResult;
	}
	else
	{
		if (sigmaA != 0)
		{
			result.width = paramA;
			result.width_sigma = sigmaA;
		}
		else if (sigmaB != 0)
		{
			result.width = paramB;
			result.width_sigma = sigmaB;
		}
	}
#pragma endregion
	//height
#pragma region height
	paramA = objL->height;
	sigmaA = objL->height_sigma;
	paramB = objC->height;
	sigmaB = objC->height_sigma;
	if (sigmaA != 0 && sigmaB != 0) {
		EstimateParameter(paramA, sigmaA, paramB, sigmaB, &paramResult, &sigmaResult);
		result.height = paramResult;
		result.height_sigma = sigmaResult;
	}
	else
	{
		if (sigmaA != 0)
		{
			result.height = paramA;
			result.height_sigma = sigmaA;
		}
		else if (sigmaB != 0)
		{
			result.height = paramB;
			result.height_sigma = sigmaB;
		}
	}
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
	for (int i = 0; i < 4; i++)
	{
		paramA = objL->bounding_box_x_rel[i];
		sigmaA = objL->x_sigma;
		paramB = objC->bounding_box_x_rel[i];
		sigmaB = objC->x_sigma;
		if (sigmaA != 0 && sigmaB != 0) {
			EstimateParameter(paramA, sigmaA, paramB, sigmaB, &paramResult, &sigmaResult);
			result.bounding_box_x_rel[i] = paramResult;
		}
		else
		{
			if (sigmaA != 0)
			{
				result.bounding_box_x_rel[i] = paramA;
			}
			else if (sigmaB != 0)
			{
				result.bounding_box_x_rel[i] = paramB;
			}
		}
	}
#pragma endregion
	//BBox y_rel
#pragma region BBox y_rel
	for (int i = 0; i < 4; i++)
	{
		paramA = objL->bounding_box_y_rel[i];
		sigmaA = objL->y_sigma;
		paramB = objC->bounding_box_y_rel[i];
		sigmaB = objC->y_sigma;
		if (sigmaA != 0 && sigmaB != 0) {
			EstimateParameter(paramA, sigmaA, paramB, sigmaB, &paramResult, &sigmaResult);
			result.bounding_box_y_rel[i] = paramResult;
		}
		else
		{
			if (sigmaA != 0)
			{
				result.bounding_box_y_rel[i] = paramA;
			}
			else if (sigmaB != 0)
			{
				result.bounding_box_y_rel[i] = paramB;
			}
		}
	}
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
	}
#pragma endregion
	//yaw_rel
#pragma region yaw_rel
	paramA = objL->yaw_rel;
	sigmaA = objL->yaw_sigma;
	paramB = objC->yaw_rel;
	sigmaB = objC->yaw_sigma;
	if (sigmaA != 0 && sigmaB != 0) {
		EstimateParameter(paramA, sigmaA, paramB, sigmaB, &paramResult, &sigmaResult);
		result.yaw_rel = paramResult;
		result.yaw_sigma = sigmaResult;
	}
	else
	{
		if (sigmaA != 0)
		{
			result.yaw_rel = paramA;
			result.yaw_sigma = sigmaA;
		}
		else if (sigmaB != 0)
		{
			result.yaw_rel = paramB;
			result.yaw_sigma = sigmaB;
		}
	}
#pragma endregion
	//speed_x_rel
#pragma region speed_x_rel
	paramA = objL->speed_x_rel;
	sigmaA = objL->speed_x_sigma;
	paramB = objC->speed_x_rel;
	sigmaB = objC->speed_x_sigma;
	if (sigmaA != 0 && sigmaB != 0) {
		EstimateParameter(paramA, sigmaA, paramB, sigmaB, &paramResult, &sigmaResult);
		result.speed_x_rel = paramResult;
		result.speed_x_sigma = sigmaResult;
	}
	else
	{
		if (sigmaA != 0)
		{
			result.speed_x_rel = paramA;
			result.speed_x_sigma = sigmaA;
		}
		else if (sigmaB != 0)
		{
			result.speed_x_rel = paramB;
			result.speed_x_sigma = sigmaB;
		}
	}
#pragma endregion
	//speed_y_rel
#pragma region speed_y_rel
	paramA = objL->speed_y_rel;
	sigmaA = objL->speed_y_sigma;
	paramB = objC->speed_y_rel;
	sigmaB = objC->speed_y_sigma;
	if (sigmaA != 0 && sigmaB != 0) {
		EstimateParameter(paramA, sigmaA, paramB, sigmaB, &paramResult, &sigmaResult);
		result.speed_y_rel = paramResult;
		result.speed_y_sigma = sigmaResult;
	}
	else
	{
		if (sigmaA != 0)
		{
			result.speed_y_rel = paramA;
			result.speed_y_sigma = sigmaA;
		}
		else if (sigmaB != 0)
		{
			result.speed_y_rel = paramB;
			result.speed_y_sigma = sigmaB;
		}
	}
#pragma endregion
	//acceleration_x_rel
#pragma region acceleration_x_rel
	paramA = objL->acceleration_x_rel;
	sigmaA = objL->acceleration_x_sigma;
	paramB = objC->acceleration_x_rel;
	sigmaB = objC->acceleration_x_sigma;
	if (sigmaA != 0 && sigmaB != 0) {
		EstimateParameter(paramA, sigmaA, paramB, sigmaB, &paramResult, &sigmaResult);
		result.acceleration_x_rel = paramResult;
		result.acceleration_x_sigma = sigmaResult;
	}
	else
	{
		if (sigmaA != 0)
		{
			result.acceleration_x_rel = paramA;
			result.acceleration_x_sigma = sigmaA;
		}
		else if (sigmaB != 0)
		{
			result.acceleration_x_rel = paramB;
			result.acceleration_x_sigma = sigmaB;
		}
	}
#pragma endregion
	//acceleration_y_rel
#pragma region acceleration_y_rel
	paramA = objL->acceleration_y_rel;
	sigmaA = objL->acceleration_y_sigma;
	paramB = objC->acceleration_y_rel;
	sigmaB = objC->acceleration_y_sigma;
	if (sigmaA != 0 && sigmaB != 0) {
		EstimateParameter(paramA, sigmaA, paramB, sigmaB, &paramResult, &sigmaResult);
		result.acceleration_y_rel = paramResult;
		result.acceleration_y_sigma = sigmaResult;
	}
	else
	{
		if (sigmaA != 0)
		{
			result.acceleration_y_rel = paramA;
			result.acceleration_y_sigma = sigmaA;
		}
		else if (sigmaB != 0)
		{
			result.acceleration_y_rel = paramB;
			result.acceleration_y_sigma = sigmaB;
		}
	}
#pragma endregion
	//distance
#pragma region distance
	Point2D pos = Point2D(result.x_rel, result.y_rel);
	result.distance = pos.module();
#pragma endregion
	//speed_rel
#pragma region speed_rel
	Point2D speed = Point2D(result.speed_x_rel, result.speed_y_rel);
	result.speed_rel = speed.module();
#pragma endregion
	//acceleration_rel
#pragma region acceleration_rel
	Point2D accel = Point2D(result.acceleration_x_rel, result.acceleration_y_rel);
	result.acceleration_rel = accel.module();
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
	return result;
}

void MAPSBayesian_Fusion::EstimateParameter(float32_t paramA, float32_t sigmaA, float32_t paramB, float32_t sigmaB, float32_t * paramResult, float32_t * sigmaResult)
{
	*paramResult = calcParam(paramA, sigmaA, paramB, sigmaB);
	*sigmaResult = calcSigma(sigmaA, sigmaB);
}

void MAPSBayesian_Fusion::EstimateParameterNew(float32_t paramA, float32_t sigmaA, float32_t paramB, float32_t sigmaB, float32_t distanceB, float32_t * paramResult, float32_t * sigmaResult)
{
	*paramResult = calcParamNew(paramA, sigmaA, paramB, sigmaB, distanceB);
	*sigmaResult = calcSigma(sigmaA, sigmaB);
}

float32_t MAPSBayesian_Fusion::calcParam(float32_t paramL, float32_t sigmaL, float32_t paramC, float32_t sigmaC)
{
	float32_t param = ((sigmaC / (sigmaL + sigmaC))*paramL) + ((sigmaL / (sigmaL + sigmaC)) *paramC);
	return param;
}

float32_t MAPSBayesian_Fusion::calcParamNew(float32_t paramL, float32_t sigmaL, float32_t paramC, float32_t sigmaC, float32_t distanceC)
{
	float32_t maxim = pow((float32_t)max(0.1*distanceC, 2), 2);
	float32_t param = (((sigmaC*maxim) / (sigmaL + sigmaC*maxim))*paramL) + ((sigmaL / (sigmaL + sigmaC*maxim)) *paramC);
	return param;
}

float32_t MAPSBayesian_Fusion::calcSigma(float32_t sigmaL, float32_t sigmaC)
{
	float32_t sigma = (sigmaL*sigmaC) / (sigmaL + sigmaC);
	return sigma;
}

void MAPSBayesian_Fusion::ProcessData()
{
	Matching();
	Join();
	calcEstimation();
	WriteOutputs();
}

void MAPSBayesian_Fusion::ReadInputs()
{
	//We read the inputs with the  reactive metod
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
		LaserObjects[0] = LaserObjects[1];
		LaserObjects[1] = *ArrayLaserObjects_input;
		updated[0] = true;
		if (!ready) {
			completedL[0] = completedL[1];
			completedL[1] = true;
		}
		if (firsttime) {
			timestamp = ArrayLaserObjects_input->timestamp;
			firsttime = false;
		}
		break;
	case 1:
		ArrayCameraObjects_input = static_cast<AUTO_Objects*>(ioeltin->Data());
		CameraObjects[0] = CameraObjects[1];
		CameraObjects[1] = *ArrayCameraObjects_input;
		updated[1] = true;
		if (!ready) {
			completedC[0] = completedC[1];
			completedC[1] = true;
		}
		if (firsttime) {
			timestamp = ArrayCameraObjects_input->timestamp;
			firsttime = false;
		}
		break;

	default:
		break;
	}
}

void MAPSBayesian_Fusion::WriteOutputs()
{
	_ioOutput = StartWriting(Output("Output_estimation"));
	AUTO_Objects &Est = *static_cast<AUTO_Objects*>(_ioOutput->Data());
	Est = Estimation;
	StopWriting(_ioOutput);
}

void MAPSBayesian_Fusion::Predictor()
{
	ReadInputs();
	framerate = (int)GetIntegerProperty("framerate");
	// We must check if we have minimun 2 frames for that we can do a prediction
	if (!ready)
	{
		if (completedL[0] && completedL[1] && completedC[0] && completedC[1])
		{
			ready = true;
		}
	}// When we have 2 frames the algorith start
	else {
		//Here we check if there are any change in the previus data if not is not necesary to recalculate.
		//If not has been predicted previusly 
		if (!predicted || updated[0] || updated[1]) {
			predecir();
		}
		// We use this function to alculate how  much time have been passed
		QueryPerformanceCounter(&EndingTime);
		ElapsedTime = (double)(EndingTime.QuadPart - StartingTime.QuadPart) * 1000 / Frecuency.QuadPart;
		if (ElapsedTime>framerate)
		{
			StartingTime = EndingTime;
			ProcessData();
		}
	}
}

void MAPSBayesian_Fusion::Matching()
{
	clear_Matched();
	findMatches(&ArrayLaserObjects, &ArrayCameraObjects);
}

void MAPSBayesian_Fusion::Join()
{
	cleanStructures();

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
	joined.resize(LaserMatched.number_objects);
	for (int i = 0; i < LaserMatched.number_objects; i++)
	{
		//Clean list of joined
		joined.vector[i][0] = LaserMatched.id[i];
		joined.vector[i][1] = -1;
		joined.vector[i][2] = 0;
		for (int j = 0; j < LaserMatched.number_matched[i]; j++)
		{
			score = 0;
			score = calculateScore(LaserMatched.id[i], LaserMatched.Matrix_matched[i][j][0]);
			if (score >= 0)
			{
				addAssociation(i, LaserMatched.Matrix_matched[i][j][0], score);
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

void MAPSBayesian_Fusion::calcEstimation()
{
	initObjects(&Estimation);
	Estimate();
	//short the list of estimations by id
	shortObjects(&Estimation);
	shortVectorLCA(LCAssociations);
}