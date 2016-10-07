/**
 * \file Data_EnvironmentState.h
 * \brief Describes the environment state
 * \details In particular, defines the structures for Roads, Lanes, Objects (dynamic obstacles), etc.
 */

#ifndef __DATA_ENVIRONMENTSTATE_H__
#define __DATA_ENVIRONMENTSTATE_H__

#ifdef _cplusplus
extern "C" {
#endif

#ifdef WIN32
#include "stdbool.h"
#else
#include <stdbool.h>
#endif

#define IMARA_MAX_NUM_LANES 5
#define IMARA_MAX_NUM_LANE_POINTS 40
#define IMARA_MAX_NUM_OBJECTS 60
#define IMARA_MAX_NUM_LASER_POINTS 3000

// ==================== Lanes ====================

enum IMARA_LaneUseClass
{
	ILUC_NORMAL,
	ILUC_EMERGENCY,
	ILUC_RESTRICTED,
	ILUC_WALL
};
typedef enum IMARA_LaneUseClass IMARA_LaneUseClass;

enum IMARA_LaneSurfaceClass
{
	ILSC_TARMAC,
	ILSC_CONCRETE,
	ILSC_PAVING,
	ILSC_GRAVEL,
	ILSC_SOIL,
	ILSC_SAND
};
typedef enum IMARA_LaneSurfaceClass IMARA_LaneSurfaceClass;

enum IMARA_LaneMarkingClass
{
	ILMC_DASHED,
	ILMC_CONTINUOUS,
	ILMC_KEEP_LEFT,
	ILMC_KEEP_RIGHT,
	ILMC_KEEP_STRAIGHT,
	ILMC_UNKNOWN
};
typedef enum IMARA_LaneMarkingClass IMARA_LaneMarkingClass;

enum IMARA_LaneDirection
{
	ILD_FORWARD,
	ILD_BACKWARD,
	ILD_BOTH
};
typedef enum IMARA_LaneDirection IMARA_LaneDirection;

struct IMARA_Lane // Structure for describing one lane
{
	IMARA_LaneUseClass lane_use[IMARA_MAX_NUM_LANE_POINTS];
	IMARA_LaneSurfaceClass lane_surface[IMARA_MAX_NUM_LANE_POINTS];
	IMARA_LaneMarkingClass lane_marking_left[IMARA_MAX_NUM_LANE_POINTS];
	IMARA_LaneMarkingClass lane_marking_right[IMARA_MAX_NUM_LANE_POINTS];
	IMARA_LaneMarkingClass lane_marking_center[IMARA_MAX_NUM_LANE_POINTS];
	IMARA_LaneDirection lane_direction[IMARA_MAX_NUM_LANE_POINTS];

	float x[IMARA_MAX_NUM_LANE_POINTS]; // Absolute x [m]
	float y[IMARA_MAX_NUM_LANE_POINTS]; // Absolute y [m]
	float x_rel[IMARA_MAX_NUM_LANE_POINTS]; // Relative to x of the vehicle [m]
	float y_rel[IMARA_MAX_NUM_LANE_POINTS]; // Relative to y of the vehicle [m]
	float width[IMARA_MAX_NUM_LANE_POINTS]; // Lane width [m]
	float min_speed[IMARA_MAX_NUM_LANE_POINTS]; // Minimum speed [m/s]
	float max_speed[IMARA_MAX_NUM_LANE_POINTS]; // Maximum speed [m/s]
	float curvature[IMARA_MAX_NUM_LANE_POINTS]; // Estimated curvature of the lane [1/m]
	
	float x_sigma[IMARA_MAX_NUM_LANE_POINTS]; // Standard deviation of the absolute x [m]
	float y_sigma[IMARA_MAX_NUM_LANE_POINTS]; // Standard deviation of the absolute y [m]
	float width_sigma[IMARA_MAX_NUM_LANE_POINTS];
	float curvature_sigma[IMARA_MAX_NUM_LANE_POINTS];
	
	int number_of_lane_points; // Number of lane points
};
typedef struct IMARA_Lane IMARA_Lane;

struct IMARA_Lanes
{
	IMARA_Lane lane[IMARA_MAX_NUM_LANES];
	int index_ego_lane;
	int number_of_lanes; // Number of lanes
	float timestamp; // [s]
	int status; // ok, failure
};
typedef struct IMARA_Lanes IMARA_Lanes;


// ==================== Objects ====================

enum IMARA_ObjectClass
{
	IOC_UNCLASSIFIED,
	IOC_PEDESTRIAN,
	IOC_BIKE,
	IOC_CAR,
	IOC_TRUCK
};
typedef enum IMARA_ObjectClass IMARA_ObjectClass;

struct IMARA_Object
{
	int id;							/*!< Object ID from the tracking*/
	int association_id;
	float x_rel;					/*!< x position Relative to he vehicle [m]*/
	float y_rel;					/*!< y position Relative to he vehicle [m]*/

	IMARA_ObjectClass classification;/*!<Classification information*/
	float classification_confidence;/*!<Classification confidence [0 to 1]*/
	int track_age;					/*!< Number of times this object has been tracked*/
	float track_confidence;			/*!< Track confidence [0-1]*/
	
	float x;						/*!< Absolute x position [m]*/
	float y;						/*!< Absolute y position [m]*/
	float heading;					/*!< Absolute heading[rad]*/
	float heading_rel;				/*!<Heading relative to the vehicle [rad]*/
	float speed_x;					/*!<Absolute velocity [m/s]*/
	float speed_y;					/*!<Absolute velocity [m/s]*/
	float speed_rel_x;				/*!<Relative velocity (without ego motion compensation) [m/s]*/
	float speed_rel_y;				/*!<Relative velocity (without ego motion compensation) [m/s]*/
	float acceleration_x;			/*!<X acceleration [m/s^2]*/
	float acceleration_y;			/*!<Y acceleration [m/s^2]*/
	float bounding_box_x[4];		/*!<Absolute x position of the bounding box [m]*/
	float bounding_box_y[4];		/*!<Absolute y position of the bounding box [m]*/
	float bounding_box_x_rel[4];	/*!<Relative x position of the bounding box [m]*/
	float bounding_box_y_rel[4];	/*!<Relative y position of the bounding box [m]*/
	float height;					/*!<Height of the object [m]*/
	float width;					/*!<Height of the object [m]*/
	float length;					/*!<Height of the object [m]*/
	
	float x_sigma;					/*!<Standard deviation of the absolute x position [m]*/
	float y_sigma;					/*!<Standard deviation of the absolute y position [m]*/ 
	float xy_sigma;					/*!<Standard deviation of the absolute y position [m]*/ 
	float speed_x_sigma;			/*!<Standard deviation of the x velocity*/ 
	float speed_y_sigma;			/*!<Standard deviation of the y velocity*/ 
	float speed_xy_sigma;			/*!<Standard deviation of the y velocity*/ 
	float acceleration_x_sigma;		/*!<Standard deviation of the y acceleration*/ 
	float acceleration_y_sigma;		/*!<Standard deviation of the y acceleration*/ 
	float acceleration_xy_sigma;		/*!<Standard deviation of the y acceleration*/ 
	int lane_id;					/*!<ID of the lane where the object is moving (-1 means not on a lane)*/
	bool lane_change ;				/*!<Indicates if the object is changing of lanes. In that case, the lane_id is the is the smallest Id of lane between both*/

	double covariance[4][4];
};
typedef struct IMARA_Object IMARA_Object;

struct IMARA_Objects // Structure describing the detected objects 
{
	IMARA_Object object[IMARA_MAX_NUM_OBJECTS];
	int number_of_objects; // Number of detected objects
	float timestamp; // [s]
	int status; // ok, failure
};
typedef struct IMARA_Objects IMARA_Objects;


// =============== Environment State =================

struct IMARA_EnvironmentState
{
	IMARA_Lanes lanes;
	IMARA_Objects objects;
	float timestamp;
	int status; // ok, failure
};
typedef struct IMARA_EnvironmentState IMARA_EnvironmentState;

#ifdef _cplusplus
}
#endif

#endif // __DATA_ENVIRONMENT_STATE_H__
