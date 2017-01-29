////////////////////////////////
// RTMaps SDK Component header
////////////////////////////////

#ifndef _Maps_Bayesian_Fusion_H
#define _Maps_Bayesian_Fusion_H

// Includes maps sdk library header
#include "maps.hpp"
#include "auto_objects.h"
#include <math.h>
#include <vector>
#include <array>
#include "Structures.h"
#include "Parameters.h"


typedef struct {
	float32_t x;
	float32_t y;
	float32_t z;
} Point3D_32t;

using namespace std;
// Declares a new MAPSComponent child class
class MAPSBayesian_Fusion : public MAPSComponent 
{
	// Use standard header definition macro
	MAPS_COMPONENT_STANDARD_HEADER_CODE(MAPSBayesian_Fusion)
private :
	/*----------------------------------------------------------------------------------------------------*/
	//Common variables
	/*----------------------------------------------------------------------------------------------------*/
	MAPSIOElt *_ioOutput;//Output structure
	MAPSStreamedString str;//String for debug

	AUTO_Objects* ArrayLaserObjects_input;//Input structure for Laser Objects
	AUTO_Objects* ArrayCameraObjects_input;//Input structure for Camera Objects

	AUTO_Objects ArrayLaserObjects;//Internal structure for Laser Objects
	AUTO_Objects ArrayCameraObjects;//Internal structure for Camera Objects

	AUTO_Objects LaserObjects[2];//Structure for predictor Laser
	AUTO_Objects CameraObjects[2];//Structure for predictor Camera

	/*----------------------------------------------------------------------------------------------------*/
	//Predictor
	/*----------------------------------------------------------------------------------------------------*/
	//Variables Predictor
	int framerate;
	LARGE_INTEGER Frecuency, StartingTime, EndingTime;//Variables for measure the time
	double ElapsedTime;//Elapsed time
	uint32_t timestamp;//Next common timestamp(Software sincronization
	bool predicted;//Control already predicted
	bool firsttime;//Control for the first loop
	bool updated[2];//Control already predicted objects
	bool ready;//Ready for start the analisys
	bool completedL[2];//Structure Laser filling control
	bool completedC[2];//Structure Camera filling control
	//Functions Predictor
	/// <summary>
	/// Predict the position of all the obstacles in the next frame rate
	/// </summary>
	void predecir();
	/// <summary>
	/// Calculate the position of one obstacle in the next frame rate
	/// </summary>
	/// <param name="obstacle">Obstacle to move</param>
	/// <param name="timestamp">Next common timestamp</param>
	void moveObstacle(AUTO_Object * obstacle, int timestamp);
	/// <summary>
	/// Find the position of the objects(id) in the array objects
	/// </summary>
	/// <param name="objects">Array of objects</param>
	/// <param name="id">Id obstacle to find</param>
	/// <returns>Position in the array</returns>
	int findPosition(AUTO_Objects objects, int id);
	

	/*----------------------------------------------------------------------------------------------------*/
	//Matching
	/*----------------------------------------------------------------------------------------------------*/
	//Variables Matching
	MATCH_OBJECTS LaserMatched;//List of Camera Objects that have been matched with each Laser Object
	MATCH_OBJECTS CameraMatched;//List of Laser Objects that have been matched with each Camera Object
	AUTO_Objects output_LaserAmpliatedBox;//Array of objects with Ampliated Bounding Box of each Laser object
	AUTO_Objects output_CameraAmpliatedBox;//Array of objects with Ampliated Bounding Box of each Camera object
	//Functions matching
	/// <summary>
	/// Compute if 2 objects could be the same
	/// </summary>
	/// <param name="Object1">Laser object</param>
	/// <param name="Object2">Camera object</param>
	/// <returns>True if the objects could be the same false in other way</returns>
	bool BoxMatching(AUTO_Object * Object1, AUTO_Object * Object2);
	/// <summary>
	/// Go over the arrays comparing all the objects
	/// </summary>
	/// <param name="ArrayLaser">Array of Laser objects</param>
	/// <param name="ArrayCamera">Array of Camera objects</param>
	void findMatches(AUTO_Objects * ArrayLaser, AUTO_Objects * ArrayCamera);
	/// <summary>
	/// Copy the calculate Estended Bounding Box to the output array and for use it later
	/// </summary>
	/// <param name="BBox">Extended Bounding Box of one object</param>
	/// <param name="Output_ampliated">Output array with the extended Bounding Boxes</param>
	void copyBBox(BOUNDIG_BOX BBox, AUTO_Object * Output_ampliated);
	/// <summary>
	/// Compute the Extended Bounding Box of one object 
	/// </summary>
	/// <param name="Object">AUTO_Object</param>
	void calculateBoundingBox(AUTO_Object * Object);
	/// <summary>
	/// Operator to move the Bounding box
	/// </summary>
	/// <param name="entrada">Bounding box to move</param>
	/// <param name="x">meters in x edje</param>
	/// <param name="y">meters in y edje</param>
	void trasladarBowndingBox(BOUNDIG_BOX * entrada, double x, double y);
	/// <summary>
	/// Enlarge the Extended Bounding Box
	/// </summary>
	/// <param name="entrada">Bounding Box</param>
	/// <param name="x">meters in x edje</param>
	/// <param name="y">meters in y edje</param>
	void ampliarBowndingBox(BOUNDIG_BOX * entrada, double x, double y);
	/// <summary>
	/// Clean the structures
	/// </summary>
	void clear_Matched();
	/// <summary>
	/// Compute the overlap area between two objects and save it
	/// </summary>
	/// <param name="objeto1">Laser object</param>
	/// <param name="objeto2">Camera Object</param>
	void overlap(AUTO_Object * objeto1, AUTO_Object * objeto2);
	/// <summary>
	/// Find the row where is the id in the matrix of MATCH_OBJECTS
	/// </summary>
	/// <param name="id_object">id to find</param>
	/// <param name="vector">Matrix</param>
	/// <returns></returns>
	int findID(int id_object, MATCH_OBJECTS * vector);
	/// <summary>
	/// Find the column of the row where is the id_object
	/// </summary>
	/// <param name="id_object">Row</param>
	/// <param name="id_target">Column</param>
	/// <param name="vector">MATCH_OBJECTS</param>
	/// <returns></returns>
	int findID(int id_object, int id_target, MATCH_OBJECTS * vector);
	/// <summary>
	/// Calculate the Union divided by intersection
	/// </summary>
	/// <param name="objet1">Laser object</param>
	/// <param name="objet2">Camera object</param>
	/// <param name="BBoxInter">Bounding box of the intersection</param>
	/// <returns> Union divided by intersection</returns>
	int calcIU(AUTO_Object * objet1, AUTO_Object * objet2, BOUNDIG_BOX * BBoxInter);
	/// <summary>
	/// Compute the Bounding box that contain:
	/// </summary>
	/// <param name="original">Bounding box original extended</param>
	/// <param name="Lrotated">Bounting box rotated to the left</param>
	/// <param name="Rrotated">Bounting box rotated to the right</param>
	/// <returns>Final extended bounding box</returns>
	BOUNDIG_BOX finalBox(BOUNDIG_BOX original, BOUNDIG_BOX Lrotated, BOUNDIG_BOX Rrotated);

	/*----------------------------------------------------------------------------------------------------*/
	//Join
	/*----------------------------------------------------------------------------------------------------*/
	//Variables Join
	AssociatedObjs joined;//Matrix with the associated ids
	NonAssociated nonLaserJoined;//Ids of the Laser objects that have not been associated
	NonAssociated nonCameraJoined;//Ids of the Camera objects that have not been associated
	int score;//Variable for save temporaly the score of one association
	int MatrixOfAssociations[AUTO_MAX_NUM_OBJECTS][AUTO_MAX_NUM_OBJECTS][2];//Matrix for save temporaly each posible association and their score
	//Functions Join
	/// <summary>
	/// Compute the score of one association
	/// </summary>
	/// <param name="id_Laser">Laser object</param>
	/// <param name="id_Camera">Camera object</param>
	/// <returns>integer [0,100]</returns>
	int calculateScore(int id_Laser, int id_Camera);
	/// <summary>
	/// Find object with identificator id in the array of objects
	/// </summary>
	/// <param name="id">id of object to find</param>
	/// <param name="objects">Array of objects</param>
	/// <returns>Position</returns>
	int findPositionObject(int id, AUTO_Objects *objects);
	/// <summary>
	/// Find if there are several associations of one object
	/// </summary>
	/// <param name="pos">position in the array of joined</param>
	/// <returns>return the id of the ambiguity</returns>
	int findAmbiguities(int pos);
	/// <summary>
	/// Find if this laser object has been associated
	/// </summary>
	/// <param name="id_Laser">id of Laser object</param>
	/// <returns></returns>
	bool findAssociatedLaser(int id_Laser);
	/// <summary>
	/// Find if this camera object has been associated
	/// </summary>
	/// <param name="id_Camera">id of Camera object</param>
	/// <returns></returns>
	bool findAssociatedCamera(int id_Camera);
	/// <summary>
	/// Find if there are any ambiguity
	/// </summary>
	/// <returns></returns>
	bool findAmbiguities();
	/// <summary>
	/// Check if in the movement of objects have been created a new best possible association
	/// </summary>
	/// <returns></returns>
	bool lastCheck();
	/// <summary>
	/// Check if this object has been associated
	/// </summary>
	/// <param name="id">id of object</param>
	/// <returns></returns>
	bool IsAssociated(int id);
	/// <summary>
	/// Check if the class of this objects are compatible
	/// </summary>
	/// <param name="classL">Class of Laser object</param>
	/// <param name="classC">Class of camera object</param>
	/// <returns></returns>
	bool compatibles(AUTO_OBJECT_CLASS classL, AUTO_OBJECT_CLASS classC);
	void PrintAssociations();
	void PrintPossibleAssociations();
	/// <summary>
	/// Clean structures of associations
	/// </summary>
	void cleanAssociatedList();
	/// <summary>
	/// Clean all temporal structures
	/// </summary>
	void cleanStructures();
	/// <summary>
	/// Add new posible association to the matrix
	/// </summary>
	/// <param name="posLaser">position of Laser object in the matrix</param>
	/// <param name="idCam">Id of camera object</param>
	/// <param name="score">score of the asociation</param>
	void addAssociation(int posLaser, int idCam, int score);
	/// <summary>
	/// Sort matrix of associations by score
	/// </summary>
	void shortMatrixAssociations();
	/// <summary>
	/// Select this associatlion like the best one
	/// </summary>
	void selectAssociations();
	/// <summary>
	/// Clean the association matrix structure
	/// </summary>
	void cleanAssociationMatrix();
	/// <summary>
	/// Select the next possible association like the best one 
	/// </summary>
	/// <param name="pos">Position of this association</param>
	void selectNextAssociation(int pos);
	/// <summary>
	/// Calc score for the coincidence in the type of the objects
	/// </summary>
	float calcScoreType(AUTO_Object * Object_Laser, AUTO_Object * Object_Camera);//[0,100]
	/// <summary>
	/// Calc score for the coincidence in the position of the objects
	/// </summary>
	float calcScorePos(AUTO_Object * Object_Laser, AUTO_Object * Object_Camera);//[0,100]
	/// <summary>
	/// Calc score for the coincidence in the size of the objects
	/// </summary>
	float calcScoreSize(AUTO_Object * Object_Laser, AUTO_Object * Object_Camera);//[0,100]
	/// <summary>
	/// Calc score for the coincidence in the speed of the objects
	/// </summary>
	float calcScoreSpeed(AUTO_Object * Object_Laser, AUTO_Object * Object_Camera);//[0,100]
	/// <summary>
	/// Calc score for the coincidence in the center of the objects
	/// </summary>
	float calcScoreCenter(AUTO_Object * Object_Laser, AUTO_Object * Object_Camera);//[0,100]
	/// <summary>
	/// Calc score for the coincidence in the overlap between the objetcs
	/// </summary>
	float getOverlapScore(AUTO_Object * Object_Laser, AUTO_Object * Object_Camera);//[0,100]
	

	/*----------------------------------------------------------------------------------------------------*/
	//Estimation
	/*----------------------------------------------------------------------------------------------------*/
	//Variables Join
	AUTO_Objects Estimation;//Array of estimated objects
	vector<array<int, 3>>LCAssociations;//Ids of camera objects non asociated
	int lastID;//Last id used
	//Functions Join
	/// <summary>
	/// Estimation function
	/// </summary>
	void Estimate();
	/// <summary>
	/// Function to short the vector of associations
	/// </summary>
	/// <param name="vect">Vector of asociations</param>
	void shortVectorLCA(vector<array<int, 3>> vect);
	/// <summary>
	/// Function to shrot the output array by id
	/// </summary>
	/// <param name="objects">Objects array</param>
	void shortObjects(AUTO_Objects * objects);
	/// <summary>
	/// Function to estimate the new parameter from th eoriginal objects
	/// </summary>
	/// <param name="paramA">Parameter from the camera</param>
	/// <param name="sigmaA">Sigma of the previous parameter</param>
	/// <param name="paramB">Parameter from the Laser</param>
	/// <param name="sigmaB">Sigma of the previous parameter</param>
	/// <param name="paramResult">Result parameter</param>
	/// <param name="sigmaResult">Result sigma</param>
	void EstimateParameter(float32_t paramA, float32_t sigmaA, float32_t paramB, float32_t sigmaB, float32_t * paramResult, float32_t * sigmaResult);
	/// <summary>
	/// Function to estimate the new parameter from th eoriginal objects with the new formula
	/// </summary>
	/// /// <param name="paramA">Parameter from the camera</param>
	/// <param name="sigmaA">Sigma of the previous parameter</param>
	/// <param name="paramB">Parameter from the Laser</param>
	/// <param name="sigmaB">Sigma of the previous parameter</param>
	/// /// <param name="distanceB">Distance of the Camera object</param>
	/// <param name="paramResult">Result parameter</param>
	/// <param name="sigmaResult">Result sigma</param>
	void EstimateParameterNew(float32_t paramA, float32_t sigmaA, float32_t paramB, float32_t sigmaB, float32_t distanceB, float32_t * paramResult, float32_t * sigmaResult);
	/// <summary>
	/// Generate id for a Laser object
	/// </summary>
	/// <param name="id">Laser id</param>
	/// <returns>Result id</returns>
	int generateIdLas(int id);
	/// <summary>
	/// Generate id fra a Camera object
	/// </summary>
	/// <param name="id">Camera id</param>
	/// <returns>Result id</returns>
	int generateIdCam(int id);
	/// <summary>
	/// Generate id for a association
	/// </summary>
	/// <param name="idL">Laser id</param>
	/// <param name="idC">Camera id</param>
	/// <returns>Result id</returns>
	int generateIdLC(int idL, int idC);
	/// <summary>
	/// find the position of an association,Return position
	/// </summary>
	int findAssoc(int idL, int idC);
	/// <summary>
	/// Find a Laser ibject in the array of objects
	/// </summary>
	/// <param name="id">Laser id</param>
	/// <returns>Object</returns>
	AUTO_Object findLaserObj(int id);
	/// <summary>
	/// Find a Camera ibject in the array of objects
	/// </summary>
	/// <param name="id">Camera id</param>
	/// <returns>Object</returns>
	AUTO_Object findCameraObj(int id);
	/// <summary>
	/// Compute the estimation object from an association
	/// </summary>
	/// <param name="objL">Laser object</param>
	/// <param name="objC">CAmera object</param>
	/// <returns>Estimated object</returns>
	AUTO_Object calculateObj(AUTO_Object * objL, AUTO_Object * objC);
	/// <summary>
	/// Calc the estimation parameter 
	/// </summary>
	/// <param name="paramL">Laser parameter</param>
	/// <param name="sigmaL">Laser sigma</param>
	/// <param name="paramC">Camera parameter</param>
	/// <param name="sigmaC">Camera sigma</param>
	/// <returns>Estimated parameter</returns>
	float32_t calcParam(float32_t paramL, float32_t sigmaL, float32_t paramC, float32_t sigmaC);
	/// <summary>
	/// Calc parameter with the new formula
	/// </summary>
	/// <param name="paramL">Laser parameter</param>
	/// <param name="sigmaL">Laser sigma</param>
	/// <param name="paramC">Camera parameter</param>
	/// <param name="sigmaC">Camera sigma</param>
	/// <param name="distanceC">Camera object distance</param>
	/// <returns>Estimated parameter</returns>
	float32_t calcParamNew(float32_t paramL, float32_t sigmaL, float32_t paramC, float32_t sigmaC, float32_t distanceC);
	/// <summary>
	/// Calc the estimated sigma
	/// </summary>
	/// <param name="sigmaL">Laser sigma</param>
	/// <param name="sigmaC">Camera sigma</param>
	/// <returns>Estimated sigma</returns>
	float32_t calcSigma(float32_t sigmaL, float32_t sigmaC);

	/*----------------------------------------------------------------------------------------------------*/
	//Others
	/*----------------------------------------------------------------------------------------------------*/
	void ProcessData();
	void ReadInputs();
	void WriteOutputs();
	void Predictor();
	void Matching();
	void Join();
	void calcEstimation();

};

#endif
