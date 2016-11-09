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
	str.Clear();
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
	str << '\n' << "Box Matchig " << ArrayLaserObjects->number_of_objects << " " << ArrayCameraObjects->number_of_objects;
	//We look for objects inside the gating window of each object
	findMatches(ArrayLaserObjects, ArrayCameraObjects);
	printResults();
	WriteOutputs();

	clear_Matched();
	//ReportInfo(str);
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
		ArrayLaserObjects = static_cast<AUTO_Objects*>(ioeltin->Data());
		readed[0] = true;
		break;
	case 1:
		ArrayCameraObjects = static_cast<AUTO_Objects*>(ioeltin->Data());
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
	list = *ArrayLaserObjects;
	StopWriting(_ioOutput);

	_ioOutput = StartWriting(Output("CameraObjects"));
	AUTO_Objects &list2 = *static_cast<AUTO_Objects*>(_ioOutput->Data());
	list2 = *ArrayCameraObjects;
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
		str << "ID: " << ArrayLaserObjects->object[printer].id << '\n';
		for (int printer2 = 0; printer2 < LaserMatched.number_matched[printer]; printer2++)
		{
			str << " " << LaserMatched.Matrix_matched[printer][printer2];
		}
		str << '\n';
	}
	str << '\n';
	str << '\n' << "Camara" << '\n';
	for (int printer = 0; printer < CameraMatched.number_objects; printer++)
	{
		str << "ID: " << ArrayCameraObjects->object[printer].id << '\n';
		for (int printer2 = 0; printer2 < CameraMatched.number_matched[printer]; printer2++)
		{
			str << " " << CameraMatched.Matrix_matched[printer][printer2];
		}
		str << '\n';
	}

	str << '\n' << "fin de impresion";
}

void MAPSMatching::findMatches(AUTO_Objects* ArrayLaserObjects, AUTO_Objects* ArrayCameraObjects)
{
	//We look for coincidences between bounding box of 2 objects
	LaserMatched.number_objects = ArrayLaserObjects->number_of_objects;
	for (int i = 0; i < ArrayLaserObjects->number_of_objects; i++)
	{
		LaserMatched.id[i] = ArrayLaserObjects->object[i].id;
	}
	CameraMatched.number_objects = ArrayCameraObjects->number_of_objects;
	for (int i = 0; i < ArrayCameraObjects->number_of_objects; i++)
	{
		CameraMatched.id[i] = ArrayCameraObjects->object[i].id;
	}
	output_LaserAmpliatedBox = *ArrayLaserObjects;
	output_CameraAmpliatedBox = *ArrayCameraObjects;

	for (int a = 0; a < output_LaserAmpliatedBox.number_of_objects; a++) 
	{
		calculateBoundingBox(&output_LaserAmpliatedBox.object[a]);
	}
	for (int b = 0; b < output_CameraAmpliatedBox.number_of_objects; b++)
	{
		calculateBoundingBox(&output_CameraAmpliatedBox.object[b]);
	}
	for (int i = 0; i < ArrayLaserObjects->number_of_objects; i++)
	{
		for (int j = 0; j < ArrayCameraObjects->number_of_objects; j++)
		{
			//TODO::
			//if (ArrayLaserObjects->object[i].object_class == ArrayCameraObjects->object[j].object_class)
			//{
				if (BoxMatching(output_LaserAmpliatedBox.object[i], output_CameraAmpliatedBox.object[j]))
				{
					LaserMatched.Matrix_matched[i][LaserMatched.number_matched[i]] = ArrayCameraObjects->object[j].id;
					LaserMatched.number_matched[i]++;
					CameraMatched.Matrix_matched[j][CameraMatched.number_matched[j]] = ArrayLaserObjects->object[i].id;
					CameraMatched.number_matched[j]++;
					overlap(output_LaserAmpliatedBox.object[i], output_CameraAmpliatedBox.object[j]);
				}
				output_LaserAmpliatedBox.number_of_objects++;
				output_CameraAmpliatedBox.number_of_objects++;
			//}
		}
	}
}

bool MAPSMatching::BoxMatching(AUTO_Object Object1, AUTO_Object Object2)
{
	double x_max(-DBL_MAX), x_min(DBL_MAX), y_max(-DBL_MAX), y_min(DBL_MAX);

	for (int i = 0; i < 4; i++) {
		x_max = max(Object1.bounding_box_x_rel[i], x_max);
		x_min = min(Object1.bounding_box_x_rel[i], x_min);
		y_max = max(Object1.bounding_box_y_rel[i], y_max);
		y_min = min(Object1.bounding_box_y_rel[i], y_min);
	}

	for (size_t i = 0; i < 4; i++)
	{
		if (Object1.bounding_box_x_rel[i] > x_min && Object1.bounding_box_x_rel[i] < x_max)
		{
			if (Object1.bounding_box_y_rel[i] > y_min && Object1.bounding_box_y_rel[i] < y_max)
			{
				return true;
			}
		}
	}

	//Calcular si el centro esta dentro de la bownding box
	if (Object2.x_rel > x_min && Object2.x_rel < x_max)
	{
		if (Object2.y_rel > y_min && Object2.y_rel < y_max)
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

void MAPSMatching::calculateBoundingBox(AUTO_Object *Object)
{
	BOUNDIG_BOX original_ampliated;
	BOUNDIG_BOX ampliated_Lrotated;
	BOUNDIG_BOX ampliated_Rrotated;
	BOUNDIG_BOX final_BowndingBox;

#pragma region Calculate Original_ampliated
#pragma region Copiar a local
	original_ampliated = calculateBBox(*Object);
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
	rotarBoundingBox(&ampliated_Lrotated, -Object->yaw_sigma);
	//Return to the real position
	trasladarBowndingBox(&ampliated_Lrotated, Object->x_rel, Object->y_rel);
	trasladarBowndingBox(&ampliated_Rrotated, -Object->x_rel, -Object->y_rel);
	rotarBoundingBox(&ampliated_Rrotated, Object->yaw_sigma);
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

void MAPSMatching::rotarBoundingBox(BOUNDIG_BOX* entrada, double angulo)
{
	//Rotate bounding box
	rotarPunto(&entrada->point[0], angulo);
	rotarPunto(&entrada->point[1], angulo);
	rotarPunto(&entrada->point[2], angulo);
	rotarPunto(&entrada->point[3], angulo);
}

void MAPSMatching::rotarPunto(B_POINT *punto, double angulo)
{
	//Rotate point in the plane 
	punto->x = (punto->x * (float32_t)cos(angulo)) - (punto->y * (float32_t)sin(angulo));
	punto->y = (punto->x * (float32_t)sin(angulo)) + (punto->y * (float32_t)cos(angulo));
}

void MAPSMatching::clear_Matched()
{
	//Clear the arrays of the matched objects
	for (int i = 0; i < AUTO_MAX_NUM_OBJECTS; i++)
	{
		for (int j = 0; j < AUTO_MAX_NUM_OBJECTS; j++)
		{
			LaserMatched.Matrix_matched[i][j] = 0;
			CameraMatched.Matrix_matched[i][j] = 0;
		}
		LaserMatched.number_matched[i] = 0;
		CameraMatched.number_matched[i] = 0;
	}
	LaserMatched.number_objects = 0;
	CameraMatched.number_objects = 0;
}

void MAPSMatching::overlap(AUTO_Object objeto1, AUTO_Object objeto2)
{

	BOUNDIG_BOX finalBox, BBox1, BBox2;
	BBox1 = calculateBBox(objeto1);
	BBox2 = calculateBBox(objeto2);
	float32_t x_min, x_max, y_min, y_max;
	x_max = min(max(max(BBox1.point[0].x, BBox1.point[1].x), max(BBox1.point[2].x, BBox1.point[3].x)), max(max(BBox2.point[0].x, BBox2.point[1].x), max(BBox2.point[2].x, BBox2.point[3].x)));
	y_max = min(max(max(BBox1.point[0].y, BBox1.point[1].y), max(BBox1.point[2].y, BBox1.point[3].y)), max(max(BBox2.point[0].y, BBox2.point[1].y), max(BBox2.point[2].y, BBox2.point[3].y)));
	x_min = max(min(min(BBox1.point[0].x, BBox1.point[1].x), min(BBox1.point[2].x, BBox1.point[3].x)), min(min(BBox2.point[0].x, BBox2.point[1].x), min(BBox2.point[2].x, BBox2.point[3].x)));
	y_min = max(min(min(BBox1.point[0].y, BBox1.point[1].y), min(BBox1.point[2].y, BBox1.point[3].y)), min(min(BBox2.point[0].y, BBox2.point[1].y), min(BBox2.point[2].y, BBox2.point[3].y)));

	finalBox.point[0].x = x_max;
	finalBox.point[0].y = y_min;

	finalBox.point[1].x = x_max;
	finalBox.point[1].y = y_max;

	finalBox.point[2].x = x_min;
	finalBox.point[2].y = y_max;

	finalBox.point[3].x = x_min;
	finalBox.point[3].y = y_min;


	LaserMatched.overlap[findID(objeto1.id, LaserMatched)][findID(objeto1.id, objeto2.id, LaserMatched)][0] = compareArea(finalBox, BBox1);//Compare area between intersection box and Laser object box
	LaserMatched.overlap[findID(objeto1.id, LaserMatched)][findID(objeto1.id, objeto2.id, LaserMatched)][1] = compareArea(finalBox, BBox2);//Compare area between intersection box and Camera object box

	CameraMatched.overlap[findID(objeto2.id, CameraMatched)][findID(objeto2.id, objeto1.id, CameraMatched)][0] = LaserMatched.overlap[objeto1.id][findID(objeto1.id, objeto2.id, LaserMatched)][1];
	CameraMatched.overlap[findID(objeto2.id, CameraMatched)][findID(objeto2.id, objeto1.id, CameraMatched)][1] = LaserMatched.overlap[objeto1.id][findID(objeto1.id, objeto2.id, LaserMatched)][0];

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
	return (area1 * 100) / area2;//Porcentaje de coincidencia
}

int MAPSMatching::findID(int id_object, MATCH_OBJECTS vector)
{
	for (int i = 0; i < vector.number_objects; i++)
	{
		if (ArrayLaserObjects->object[i].id == id_object)
		{
			return i;
		}
	}
	return -1;
}

int MAPSMatching::findID(int id_object, int id_target, MATCH_OBJECTS vector)
{
	int pos = findID(id_object, vector);

	for (int j = 0; j < vector.number_matched[pos]; j++)
	{
		if (vector.Matrix_matched[pos][j] == id_target)
		{
			return j;
		}
	}
	return -1;
}

BOUNDIG_BOX MAPSMatching::calculateBBox(AUTO_Object obj)
{
	BOUNDIG_BOX BBox;
	BBox.point[0].x = obj.bounding_box_x_rel[0];
	BBox.point[0].y = obj.bounding_box_y_rel[0];
	BBox.point[1].x = obj.bounding_box_x_rel[1];
	BBox.point[1].y = obj.bounding_box_y_rel[1];
	BBox.point[2].x = obj.bounding_box_x_rel[2];
	BBox.point[2].y = obj.bounding_box_y_rel[2];
	BBox.point[3].x = obj.bounding_box_x_rel[3];
	BBox.point[3].y = obj.bounding_box_y_rel[3];

	return BBox;
}

