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

//Initialization: Birth() will be called once at diagram execution startup.			  
void MAPSMatching::Birth()
{
	
}

void MAPSMatching::Core()
{
	clear_Matched();
	ArrayLaserObjects = nullptr;
	ArrayCameraObjects = nullptr;
	str.Clear();

	//Read input objects
	readInputs();

	str << '\n' << "Box Matchig " << ArrayLaserObjects->number_of_objects << " " << ArrayCameraObjects->number_of_objects;

	//Buscamos objetos en el gating window de cada obstaculo
	//We look for objects inside the gating window of each object
	findMatches(ArrayLaserObjects, ArrayCameraObjects);
	printResults();
	WriteOutputs();
	ReportInfo(str);
}

void MAPSMatching::Death()
{

}

void MAPSMatching::readInputs()
{
	while (!DataAvailableInFIFO(Input("LaserObject")) || !DataAvailableInFIFO(Input("CameraObject"))) {}
	str << '\n' << "Objects detected";
	//Leer laser
	if (DataAvailableInFIFO(Input("LaserObject")))
	{
		elt = StartReading(Input("LaserObject"));
		ArrayLaserObjects = static_cast<AUTO_Objects*>(elt->Data());
		StopReading(Input("LaserObject"));
		output_LaserAmpliatedBox = *ArrayLaserObjects;
	}
	//Leer camara
	if (DataAvailableInFIFO(Input("CameraObject")))
	{
		elt = StartReading(Input("CameraObject"));
		ArrayCameraObjects = static_cast<AUTO_Objects*>(elt->Data());
		StopReading(Input("CameraObject"));
		output_CameraAmpliatedBox = *ArrayCameraObjects;
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
	LaserMatched.number_objects = ArrayLaserObjects->number_of_objects;
	CameraMatched.number_objects = ArrayCameraObjects->number_of_objects;
	for (int i = 0; i < ArrayLaserObjects->number_of_objects; i++)
	{
		for (int j = 0; j < ArrayCameraObjects->number_of_objects; j++)
		{
			if (ArrayLaserObjects->object[i].object_class == ArrayCameraObjects->object[j].object_class)
			{
				if (BoxMatching(ArrayLaserObjects->object[i], ArrayCameraObjects->object[j], &output_LaserAmpliatedBox.object[i], &output_CameraAmpliatedBox.object[j]))
				{
					LaserMatched.Matrix_matched[i][LaserMatched.number_matched[i]] = ArrayCameraObjects->object[j].id;
					LaserMatched.number_matched[i]++;
					CameraMatched.Matrix_matched[j][CameraMatched.number_matched[j]] = ArrayLaserObjects->object[i].id;
					CameraMatched.number_matched[j]++;
				}
			}
		}
	}
	/*
	//Comparar cada obstaculo del camara con cada uno de la laser
	for (int i = 0; i < ArrayCameraObjects->number_of_objects; i++)
	{
		for (int j = 0; j < ArrayLaserObjects->number_of_objects; j++)
		{
			if (BoxMatching(ArrayLaserObjects->object[j], ArrayCameraObjects->object[i]))
			{
				CameraMatched.Matrix_matched[i][CameraMatched.number_matched[i]] = ArrayLaserObjects->object[j].id;
				CameraMatched.number_matched[i]++;
			}
		}
	}
	*/
}

bool MAPSMatching::BoxMatching(AUTO_Object Object1, AUTO_Object Object2, AUTO_Object* Output1, AUTO_Object* Output2)
{
	BOUNDIG_BOX original_ampliated_Object1, original_ampliated_Object2;
	BOUNDIG_BOX ampliated_Lrotated_Object1, ampliated_Lrotated_Object2;
	BOUNDIG_BOX ampliated_Rrotated_Object1, ampliated_Rrotated_Object2;
	BOUNDIG_BOX final_BowndingBox_Object1, final_BowndingBox_Object2;
	//TODO::CAMBIAR
	calculateBoundingBox(Object1, &original_ampliated_Object1, &ampliated_Lrotated_Object1, &ampliated_Rrotated_Object1);
	final_BowndingBox_Object1 = finalBox(original_ampliated_Object1, ampliated_Lrotated_Object1, ampliated_Rrotated_Object1);
	copyBBox(final_BowndingBox_Object1, Output1);
	//copyBBox(ampliated_Lrotated_Object1, Output1);

	calculateBoundingBox(Object2, &original_ampliated_Object2, &ampliated_Lrotated_Object2, &ampliated_Rrotated_Object2);
	final_BowndingBox_Object2 = finalBox(original_ampliated_Object2, ampliated_Lrotated_Object2, ampliated_Rrotated_Object2);
	copyBBox(final_BowndingBox_Object2, Output2);
	//copyBBox(ampliated_Lrotated_Object2, Output2);

	double x_max, x_min, y_max, y_min;

	for (int i = 0; i < 4; i++) {
		x_max = max(final_BowndingBox_Object1.point[i].x, x_max);
		x_min = min(final_BowndingBox_Object1.point[i].x, x_min);
		y_max = max(final_BowndingBox_Object1.point[i].y, y_max);
		y_min = min(final_BowndingBox_Object1.point[i].y, y_min);
	}

	for (size_t i = 0; i < 4; i++)
	{
		if (final_BowndingBox_Object2.point[i].x > x_min && final_BowndingBox_Object2.point[i].x < x_max)
		{
			if (final_BowndingBox_Object2.point[i].y > y_min && final_BowndingBox_Object2.point[i].y < y_max)
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
	for (size_t i = 0; i < 4; i++)
	{
		Output_ampliated->bounding_box_x_rel[i] = BBox.point[i].x;
		Output_ampliated->bounding_box_y_rel[i] = BBox.point[i].y;
	}
}

void MAPSMatching::calculateBoundingBox(AUTO_Object Object, BOUNDIG_BOX* original_ampliated, BOUNDIG_BOX* ampliated_Lrotated, BOUNDIG_BOX* ampliated_Rrotated)
{
#pragma region Calculate Original_ampliated
#pragma region Copiar a local
	original_ampliated->point[0].x = Object.bounding_box_x_rel[0];
	original_ampliated->point[0].y = Object.bounding_box_y_rel[0];
	original_ampliated->point[1].x = Object.bounding_box_x_rel[1];
	original_ampliated->point[1].y = Object.bounding_box_y_rel[1];
	original_ampliated->point[2].x = Object.bounding_box_x_rel[2];
	original_ampliated->point[2].y = Object.bounding_box_y_rel[2];
	original_ampliated->point[3].x = Object.bounding_box_x_rel[3];
	original_ampliated->point[3].y = Object.bounding_box_y_rel[3];
#pragma endregion

#pragma region rotacion a 0
	rotarBoundingBox(original_ampliated, -Object.yaw_rel);
#pragma endregion

#pragma region ampliacion
	ampliarBowndingBox(original_ampliated, Object.x_sigma, Object.y_sigma);
#pragma endregion

#pragma region devolver a angulo
	rotarBoundingBox(original_ampliated, Object.yaw_rel);
#pragma endregion
#pragma endregion

	*ampliated_Lrotated = *original_ampliated;
	*ampliated_Rrotated = *original_ampliated;
	//Centrar Bowndingbox
	trasladarBowndingBox(ampliated_Lrotated, -Object.x_rel, -Object.y_rel);
	//Rotar BowndingBox
	rotarBoundingBox(ampliated_Lrotated, -Object.yaw_sigma);
	//Devolver a su posicion
	trasladarBowndingBox(ampliated_Lrotated, Object.x_rel, Object.y_rel);
	trasladarBowndingBox(ampliated_Rrotated, -Object.x_rel, -Object.y_rel);
	rotarBoundingBox(ampliated_Rrotated, Object.yaw_sigma);
	trasladarBowndingBox(ampliated_Rrotated, Object.x_rel, Object.y_rel);
}

void MAPSMatching::trasladarBowndingBox(BOUNDIG_BOX* entrada, double x, double y)
{
	entrada->point[0].x = entrada->point[0].x + x;
	entrada->point[0].y = entrada->point[0].y + y;

	entrada->point[1].x = entrada->point[1].x + x;
	entrada->point[1].y = entrada->point[1].y + y;

	entrada->point[2].x = entrada->point[2].x + x;
	entrada->point[2].y = entrada->point[2].y + y;

	entrada->point[3].x = entrada->point[3].x + x;
	entrada->point[3].y = entrada->point[3].y + y;
}

void MAPSMatching::ampliarBowndingBox(BOUNDIG_BOX* entrada, double x, double y)
{

	entrada->point[0].x = entrada->point[0].x + x;
	entrada->point[0].y = entrada->point[0].y - y;

	entrada->point[1].x = entrada->point[1].x + x;
	entrada->point[1].y = entrada->point[1].y + y;

	entrada->point[2].x = entrada->point[2].x - x;
	entrada->point[2].y = entrada->point[2].y + y;

	entrada->point[3].x = entrada->point[3].x - x;
	entrada->point[3].y = entrada->point[3].y - y;

}

BOUNDIG_BOX MAPSMatching::finalBox(BOUNDIG_BOX original, BOUNDIG_BOX Lrotated, BOUNDIG_BOX Rrotated)
{
	//TODO::ALGO MAL
	BOUNDIG_BOX finalBox;
	double x_min, x_max, y_min, y_max;
	double x_min_original, x_max_original, y_min_original, y_max_original;
	double x_min_Lrotated, x_max_Lrotated, y_min_Lrotated, y_max_Lrotated;
	double x_min_Rrotated, x_max_Rrotated, y_min_Rrotated, y_max_Rrotated;
	
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
	rotarPunto(&entrada->point[0], angulo);
	rotarPunto(&entrada->point[1], angulo);
	rotarPunto(&entrada->point[2], angulo);
	rotarPunto(&entrada->point[3], angulo);
}

void MAPSMatching::rotarPunto(B_POINT *punto, double angulo)
{
	punto->x = (punto->x * cos(angulo)) - (punto->y * sin(angulo));
	punto->y = (punto->x * sin(angulo)) + (punto->y * cos(angulo));
}

void MAPSMatching::clear_Matched()
{
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