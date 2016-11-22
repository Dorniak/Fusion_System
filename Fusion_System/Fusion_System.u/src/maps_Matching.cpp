////////////////////////////////
// RTMaps SDK Component
////////////////////////////////

////////////////////////////////
// Purpose of this module :
////////////////////////////////

#include "maps_Matching.h"	// Includes the header of this component

const MAPSTypeFilterBase ValeoStructure = MAPS_FILTER_USER_STRUCTURE(AUTO_Objects); 

// Use the macros to declare the inputs
MAPS_BEGIN_INPUTS_DEFINITION(MAPSMatching)
    MAPS_INPUT("LaserObject", ValeoStructure, MAPS::FifoReader)
	MAPS_INPUT("CameraObject", ValeoStructure, MAPS::FifoReader)
MAPS_END_INPUTS_DEFINITION

// Use the macros to declare the outputs
MAPS_BEGIN_OUTPUTS_DEFINITION(MAPSMatching)
    MAPS_OUTPUT_USER_STRUCTURE("BBoxLaser", AUTO_Objects)
	MAPS_OUTPUT_USER_STRUCTURE("BBoxCamera", AUTO_Objects)
	MAPS_OUTPUT_USER_STRUCTURE("LaserObjects", AUTO_Objects)
	MAPS_OUTPUT_USER_STRUCTURE("CameraObjects", AUTO_Objects)
	MAPS_OUTPUT_USER_STRUCTURE("MatchedLaser", MATCH_OBJECTS)
	MAPS_OUTPUT_USER_STRUCTURE("MatchedCamera", MATCH_OBJECTS)
MAPS_END_OUTPUTS_DEFINITION

// Use the macros to declare the properties
MAPS_BEGIN_PROPERTIES_DEFINITION(MAPSMatching)
    //MAPS_PROPERTY("pName",128,false,false)
MAPS_END_PROPERTIES_DEFINITION

// Use the macros to declare the actions
MAPS_BEGIN_ACTIONS_DEFINITION(MAPSMatching)
    //MAPS_ACTION("aName",MAPSMatching::ActionName)
MAPS_END_ACTIONS_DEFINITION

// Use the macros to declare this component (Matching) behaviour
MAPS_COMPONENT_DEFINITION(MAPSMatching,"Matching","1.0",128,
			  MAPS::Threaded,MAPS::Threaded,
			  -1, // Nb of inputs. Leave -1 to use the number of declared input definitions
			  -1, // Nb of outputs. Leave -1 to use the number of declared output definitions
			  -1, // Nb of properties. Leave -1 to use the number of declared property definitions
			  -1) // Nb of actions. Leave -1 to use the number of declared action definitions

void MAPSMatching::Birth()
{
	
}

void MAPSMatching::Core()
{
	//Read input objects
	readInputs();
	if ( !readed[0] || !readed [1])
	{
		return;
	}
	for (int i = 0; i < numInputs; i++)
	{
		readed[i] = false;
	}
	str.Clear();
	str << '\n' << "Box Matchig " << ArrayLaserObjects.number_of_objects << " " << ArrayCameraObjects.number_of_objects;
	//We look for objects inside the gating window of each object
	clear_Matched();
	findMatches(&ArrayLaserObjects, &ArrayCameraObjects);
	printResults();
	WriteOutputs();
	ReportInfo(str);
}

void MAPSMatching::Death()
{

}

void MAPSMatching::readInputs()
{
	MAPSInput* inputs[2] = { &Input("LaserObject"), &Input("CameraObject") };
	int inputThatAnswered;
	MAPSIOElt* ioeltin = StartReading(2, inputs, &inputThatAnswered);
	if (ioeltin == NULL) {
		return ;
	}
	switch (inputThatAnswered)
	{
	case 0:
		LaserInput = static_cast<AUTO_Objects*>(ioeltin->Data());
		ArrayLaserObjects = *LaserInput;
		readed[0] = true;
		break;
	case 1:
		CameraInput = static_cast<AUTO_Objects*>(ioeltin->Data());
		ArrayCameraObjects = *CameraInput;
		readed[1] = true;
		break;
	default:
		break;
	}
}

void MAPSMatching::WriteOutputs()
{
	_ioOutput = StartWriting(Output("BBoxLaser"));
	AUTO_Objects &list5 = *static_cast<AUTO_Objects*>(_ioOutput->Data());
	list5 = output_LaserAmpliatedBox;
	StopWriting(_ioOutput);

	_ioOutput = StartWriting(Output("BBoxCamera"));
	AUTO_Objects &list6 = *static_cast<AUTO_Objects*>(_ioOutput->Data());
	list6 = output_CameraAmpliatedBox;
	StopWriting(_ioOutput);

	_ioOutput = StartWriting(Output("LaserObjects"));
	AUTO_Objects &list = *static_cast<AUTO_Objects*>(_ioOutput->Data());
	list = ArrayLaserObjects;
	StopWriting(_ioOutput);

	_ioOutput = StartWriting(Output("CameraObjects"));
	AUTO_Objects &list2 = *static_cast<AUTO_Objects*>(_ioOutput->Data());
	list2 = ArrayCameraObjects;
	StopWriting(_ioOutput);

	_ioOutput = StartWriting(Output("MatchedLaser"));
	MATCH_OBJECTS &list3 = *static_cast<MATCH_OBJECTS*>(_ioOutput->Data());
	list3 = LaserMatched;
	StopWriting(_ioOutput);

	_ioOutput = StartWriting(Output("MatchedCamera"));
	MATCH_OBJECTS &list4 = *static_cast<MATCH_OBJECTS*>(_ioOutput->Data());
	list4 = CameraMatched;
	StopWriting(_ioOutput);
}

void MAPSMatching::printResults()
{
	str << '\n' << "Imprimir coincidencias " << '\n';
	str << '\n' << "Laser" << '\n';
	for (int printer = 0; printer < LaserMatched.number_objects; printer++)
	{
		str << "ID: " << LaserMatched.id[printer]<< " ==>";
		for (int printer2 = 0; printer2 < LaserMatched.number_matched[printer]; printer2++)
		{
			str << " " << LaserMatched.Matrix_matched[printer][printer2][0] << "(" << LaserMatched.Matrix_matched[printer][printer2][1] << ")";
		}
		str << '\n';
	}
	str << '\n';
	str << '\n' << "Camara" << '\n';
	for (int printer = 0; printer < CameraMatched.number_objects; printer++)
	{
		str << "ID: " << CameraMatched.id[printer] << " ==> ";
		for (int printer2 = 0; printer2 < CameraMatched.number_matched[printer]; printer2++)
		{
			str << " " << CameraMatched.Matrix_matched[printer][printer2][0] << "(" << CameraMatched.Matrix_matched[printer][printer2][1] << ")";
		}
		str << '\n';
	}

	str << '\n' << "fin de impresion";
}

void MAPSMatching::findMatches(AUTO_Objects * ArrayLaser, AUTO_Objects * ArrayCamera)
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

bool MAPSMatching::BoxMatching(AUTO_Object * Object1, AUTO_Object * Object2)
{
	//TODO::Modificar esto con las nuevas funciones de BBox
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

	//Calcular si el centro esta dentro de la bownding box
	if (Object2->x_rel > x_min && Object2->x_rel < x_max)
	{
		if (Object2->y_rel > y_min && Object2->y_rel < y_max)
		{
			return true;
		}
	}
	
	//Calcularlo al reves por que hay casos extraños no contemplados antes

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

	//Calcular si el centro esta dentro de la bownding box
	if (Object1->x_rel > x_min2 && Object1->x_rel < x_max2)
	{
		if (Object1->y_rel > y_min2 && Object1->y_rel < y_max2)
		{
			return true;
		}
	}
	
	return false;
}

void MAPSMatching::copyBBox(BOUNDIG_BOX BBox, AUTO_Object* Output_ampliated) 
{
	//This is only to see the extended bounding box in the bird view
	for (size_t i = 0; i < 4; i++)
	{
		Output_ampliated->bounding_box_x_rel[i] = BBox.point[i].x;
		Output_ampliated->bounding_box_y_rel[i] = BBox.point[i].y;
	}
}

void MAPSMatching::calculateBoundingBox(AUTO_Object * Object)
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

void MAPSMatching::trasladarBowndingBox(BOUNDIG_BOX* entrada, double x, double y)
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

void MAPSMatching::ampliarBowndingBox(BOUNDIG_BOX* entrada, double x, double y)
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

BOUNDIG_BOX MAPSMatching::finalBox(BOUNDIG_BOX original, BOUNDIG_BOX Lrotated, BOUNDIG_BOX Rrotated)
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

void MAPSMatching::clear_Matched()
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
		LaserMatched.number_matched[i] = 0;//Quitar las asociaciones
		CameraMatched.number_matched[i] = 0;//Quitar las asociaciones
		LaserMatched.id[i] = -1;//Eliminar todos los id
		CameraMatched.id[i] = -1;//Eliminar todos los id
	}
	LaserMatched.number_objects = 0;//Eliminar numero de objetos
	CameraMatched.number_objects = 0;//Eliminar numero de objetos
}

void MAPSMatching::overlap(AUTO_Object * objeto1, AUTO_Object * objeto2)//Call with objects with extended BBox
{
	BOUNDIG_BOX intersection, BBox1, BBox2;
	BBox1 = BOUNDIG_BOX(objeto1);
	BBox2 = BOUNDIG_BOX(objeto2);
//	float32_t x_min, x_max, y_min, y_max;

	intersection = BBox1.intersection(BBox2);
	/*
	x_max = min(max(max(BBox1.point[0].x, BBox1.point[1].x), max(BBox1.point[2].x, BBox1.point[3].x)), max(max(BBox2.point[0].x, BBox2.point[1].x), max(BBox2.point[2].x, BBox2.point[3].x)));
	y_max = min(max(max(BBox1.point[0].y, BBox1.point[1].y), max(BBox1.point[2].y, BBox1.point[3].y)), max(max(BBox2.point[0].y, BBox2.point[1].y), max(BBox2.point[2].y, BBox2.point[3].y)));
	x_min = max(min(min(BBox1.point[0].x, BBox1.point[1].x), min(BBox1.point[2].x, BBox1.point[3].x)), min(min(BBox2.point[0].x, BBox2.point[1].x), min(BBox2.point[2].x, BBox2.point[3].x)));
	y_min = max(min(min(BBox1.point[0].y, BBox1.point[1].y), min(BBox1.point[2].y, BBox1.point[3].y)), min(min(BBox2.point[0].y, BBox2.point[1].y), min(BBox2.point[2].y, BBox2.point[3].y)));

	intersection.point[0].x = x_max;
	intersection.point[0].y = y_min;

	intersection.point[1].x = x_max;
	intersection.point[1].y = y_max;

	intersection.point[2].x = x_min;
	intersection.point[2].y = y_max;

	intersection.point[3].x = x_min;
	intersection.point[3].y = y_min;
	*/

	LaserMatched.Matrix_matched[findID(objeto1->id, &LaserMatched)][findID(objeto1->id, objeto2->id, &LaserMatched)][1] = calcIU(objeto1, objeto2, &intersection);
	CameraMatched.Matrix_matched[findID(objeto2->id, &CameraMatched)][findID(objeto2->id, objeto1->id, &CameraMatched)][1] = LaserMatched.Matrix_matched[findID(objeto1->id, &LaserMatched)][findID(objeto1->id, objeto2->id, &LaserMatched)][1];	
}

float MAPSMatching::compareArea(BOUNDIG_BOX BBox, BOUNDIG_BOX BBoxOriginal)
{
	double base, altura, area1, area2;
	float32_t y_max = max(max(BBox.point[0].y, BBox.point[1].y), max(BBox.point[2].y, BBox.point[3].y));
	float32_t y_min = min(min(BBox.point[0].y, BBox.point[1].y), min(BBox.point[2].y, BBox.point[3].y));
	float32_t x_max = max(max(BBox.point[0].x, BBox.point[1].x), max(BBox.point[2].x, BBox.point[3].x));
	float32_t x_min = min(min(BBox.point[0].x, BBox.point[1].x), min(BBox.point[2].x, BBox.point[3].x));

	base = fabs(y_max- y_min);//y max - y min
	altura = fabs(x_max - x_min);//x max - x min
	area1 = base * altura;

	y_max = max(max(BBoxOriginal.point[0].y, BBoxOriginal.point[1].y), max(BBoxOriginal.point[2].y, BBoxOriginal.point[3].y));
	y_min = min(min(BBoxOriginal.point[0].y, BBoxOriginal.point[1].y), min(BBoxOriginal.point[2].y, BBoxOriginal.point[3].y));
	x_max = max(max(BBoxOriginal.point[0].x, BBoxOriginal.point[1].x), max(BBoxOriginal.point[2].x, BBoxOriginal.point[3].x));
	x_min = min(min(BBoxOriginal.point[0].x, BBoxOriginal.point[1].x), min(BBoxOriginal.point[2].x, BBoxOriginal.point[3].x));

	base = fabs(y_max - y_min);//y max - y min
	altura = fabs(x_max - x_min);//x max - x min
	area2 = base * altura;

	return (float)((area1 * 100) / area2);//Porcentaje de coincidencia
}

int MAPSMatching::findID(int id_object, MATCH_OBJECTS * vector)
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

int MAPSMatching::findID(int id_object, int id_target, MATCH_OBJECTS * vector)
{
	int pos = findID(id_object, vector);

	if (pos == -1 || id_target == -1 )
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

int MAPSMatching::calcIU(AUTO_Object * objet1, AUTO_Object * objet2, BOUNDIG_BOX * BBoxInter)
{
	double area1, area2, areaI, areaU, areaFinal;
	BOUNDIG_BOX obj1, obj2;

	obj1 = BOUNDIG_BOX(objet1);
	obj2 = BOUNDIG_BOX(objet2);
	//TODO::Esto esta mal no se puede usar el width y el length por que son de la BBox original
	area1 = obj1.area();
	area2 = obj2.area();
	areaI = BBoxInter->area();
	areaU = area1 + area2 - areaI;
	areaFinal = (areaI / areaU)*100;//[0,100]
	areaFinal = round(areaFinal);

	return (int)areaFinal;
}

double MAPSMatching::calcArea(BOUNDIG_BOX * BBox)
{
	float32_t distanceA, distanceB, distanceC;
	float32_t width, length;

	distanceA = BBox->point[0].dist(BBox->point[1]);
	distanceB = BBox->point[0].dist(BBox->point[2]);
	distanceC = BBox->point[0].dist(BBox->point[3]);

	width = min(min(distanceA, distanceB), distanceC);
	if (distanceA == width)
	{
		length = min(distanceB, distanceC);
	}
	else if (distanceB == width)
	{
		length = min(distanceA, distanceC);
	}
	else
	{
		length = min(distanceB, distanceC);
	}

	return width*length;
}
