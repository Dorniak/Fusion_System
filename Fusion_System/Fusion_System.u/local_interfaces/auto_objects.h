/**
 * @copyright (c) Valeo Driving Assistance Research.
 * <br>All rights reserved. Subject to limited distribution and restricted disclosure only.
 * @author paulo.resende@valeo.com
 * @file auto_objects.h
 * @brief Describes the state and the features of the objects.
 * @version 1.0
 * @date 18-02-2014
 * @bug	No known bugs
 * @warning	No warnings
 */

#ifndef __AUTO_OBJECTS_H__
#define __AUTO_OBJECTS_H__

#ifdef __cplusplus
extern "C" {
#endif

/*---------------------------------------------------------------------------
** Includes
*/
#include "auto_common.h"

/*---------------------------------------------------------------------------
** Defines and Macros
*/
/** Maximum number of relevant objects. */
#define AUTO_MAX_NUM_OBJECTS 200

/*---------------------------------------------------------------------------
** Typedefs
*/
/**
 * @enum AUTO_OBJECT_CLASS
 */
typedef enum {
	AUTO_OCL_UNKNOWN = 0, /**< Unknown. */
	AUTO_OCL_PEDESTRIAN, /**< Pedestrian. */
	AUTO_OCL_BIKE, /**< Bike. */
	AUTO_OCL_CAR, /**< Car. */
	AUTO_OCL_TRUCK, /**< Truck. */
	AUTO_OCL_ANIMAL, /**< Animal. */
	AUTO_OCL_BARRIER, /**< Barrier. Static obstacle. */
	AUTO_OCL_VIRTUAL /**< Virtual. Not a physical obstacle. */
} AUTO_OBJECT_CLASS;

/**
 * @enum AUTO_OBJECT_SUB_CLASS
 */
typedef enum {
	AUTO_OSC_UNKNOWN = 0, /**< Unknown. */
	
	AUTO_OSC_SMALL,
	AUTO_OSC_MEDIUM,
	AUTO_OSC_BIG,
	
	AUTO_OSC_PEDESTRIAN_ADULT,
	AUTO_OSC_PEDESTRIAN_CHILD,	
	AUTO_OSC_PEDESTRIAN_WHEELCHAIR,
	
	AUTO_OSC_BIKE_BICYCLE,
	AUTO_OSC_BIKE_MOTORCYCLE,
	AUTO_OSC_BIKE_TRICYCLE,
	
	// Euro NCAP Structural Category
	AUTO_OSC_CAR_PASSENGER, /**< Passenger car. */
	AUTO_OSC_CAR_MPV, /**< Multi-Purpose Vehicle, Minivan, Cargo van or Passenger van. */
	AUTO_OSC_CAR_OFF_ROADER, /**< Sport Utility Vehicle (SUV). */
	AUTO_OSC_CAR_PICKUP, /**< Pickup. */

	// FHWA Classification Chart
	AUTO_OSC_TRUCK_BUS,
	AUTO_OSC_TRUCK_SINGLE_UNIT,
	AUTO_OSC_TRUCK_SINGLE_TRAILER,
	AUTO_OSC_TRUCK_MULTI_TRAILER,
	
	AUTO_OSC_BARRIER_GATE,
	AUTO_OSC_BARRIER_TOLLGATE,
	AUTO_OSC_BARRIER_GUARDRAIL,
	AUTO_OSC_BARRIER_BOLLARD,
	
	AUTO_OSC_VIRTUAL_STOP,
	AUTO_OSC_VIRTUAL_TRAFFIC_LIGHT,
	AUTO_OSC_VIRTUAL_YIELD
} AUTO_OBJECT_SUB_CLASS;

#ifdef WIN32
#pragma pack(push)  /* Push current alignment to stack */
#pragma pack(1)     /* Set alignment to 1 byte boundary */
#endif

/**
 * @struct AUTO_Object
 * @brief Object state and features
 */
typedef struct {
	uint16_t 				id; /**< Object id. */	
	AUTO_GnssData 			gnss; /**< Global pose WGS84. */
	AUTO_OBJECT_SUB_CLASS	object_sub_class; /**< Object sub-class. */
	AUTO_OBJECT_CLASS 		object_class; /**< Object class. */
	uint16_t				class_age; /**< Object class age. Number of times this object has been classified as current class. */
	float32_t				class_confidence; /**< Object class confidence [0.f, 1.f]. */
	uint16_t 				track_age; /**< Object track age. Number of times this object has been tracked without detection. Set to 0 when detected again. */
	float32_t 				track_confidence; /**< Object track confidence [0.f, 1.f]. */
	float32_t 				length; /**< Object length [m]. */
	float32_t 				width; /**< Object width [m]. */
	float32_t 				height; /**< Object height [m]. */
	float32_t 				x; /**< Absolute x regarding an arbitrary global pose [m]. */
	float32_t 				y; /**< Absolute y regarding an arbitrary global pose [m]. */
	float32_t 				z; /**< Absolute z regarding an arbitrary global pose [m]. */
	float32_t 				yaw; /**< Absolute yaw angle regarding an arbitrary global pose [rad]. */
	float32_t				yaw_rate; /**< Yaw rate [rad/s]. */
	float32_t 				speed; /**< Absolute ground speed [m/s]. */
	float32_t 				acceleration; /**< Absolute acceleration [m/s^2]. */
	float32_t 				bounding_box_x_rel[4]; /**< Relative bounding box x_rel coordinates [m]. */
	float32_t 				bounding_box_y_rel[4]; /**< Relative bounding box y_rel coordinates [m]. */
	float32_t 				x_rel; /**< Relative to x of the ego vehicle [m]. */
	float32_t 				y_rel; /**< Relative to y of the ego vehicle [m]. */
	float32_t 				z_rel; /**< Relative to z of the ego vehicle [m]. */
	float32_t 				yaw_rel; /**< Yaw angle relative to the yaw of the ego vehicle [rad]. */
	float32_t 				speed_x_rel; /**< Relative velocity (vector) in x_rel (without ego motion compensation) [m/s]. */
	float32_t 				speed_y_rel; /**< Relative velocity (vector) in y_rel (without ego motion compensation) [m/s]. */
	float32_t 				acceleration_x_rel; /**< Relative acceleration in x_rel (without ego motion compensation) [m/s^2]. */
	float32_t 				acceleration_y_rel; /**< Relative acceleration in y_rel (without ego motion compensation) [m/s^2]. */
	float32_t				distance; /**< Distance (euclidean or geodesic) between object and vehicle (-1 if far away) [m]. */
	float32_t				speed_rel; /**< Relative speed (euclidean or geodesic) between object and vehicle [m/s]. */
	float32_t				acceleration_rel; /**< Relative acceleration (euclidean or geodesic) between object and vehicle [m/s^2]. */
	float32_t 				length_sigma; /**< Standard deviation of length [m]. */
	float32_t 				width_sigma; /**< Standard deviation of width [m]. */
	float32_t 				height_sigma; /**< Standard deviation of height [m]. */
	float32_t 				x_sigma; /**< Standard deviation of x and x_rel [m]. */
	float32_t 				y_sigma; /**< Standard deviation of y and y_rel [m]. */
	float32_t 				z_sigma; /**< Standard deviation of z and z_rel [m]. */
	float32_t 				yaw_sigma; /**< Standard deviation of yaw and yaw_rel [rad]. */
	float32_t 				speed_x_sigma; /**< Standard deviation of speed_x_rel [m/s]. */
	float32_t 				speed_y_sigma; /**< Standard deviation of speed_y_rel [m/s]. */
	float32_t 				acceleration_x_sigma; /**< Standard deviation of acceleration_x_rel [m/s^2]. */
	float32_t 				acceleration_y_sigma; /**< Standard deviation of acceleration_y_rel [m/s^2]. */	
	AUTO_TURN_SIGNAL		turn_signal; /**< Turn signal. */
	bool_t					brake_signal; /**< Brake signal. */
	uint32_t 				timestamp; /**< Timestamp [ms]. */
	AUTO_STATUS				status; /**< Status. */
} AUTO_Object;

/**
 * @struct AUTO_Objects
 * @brief Objects
 */
typedef struct {
	uint16_t	primary_object_index; /**< Primary object index identifies the most relevant object in the ego vehicle trajectory. */
	AUTO_Object	object[AUTO_MAX_NUM_OBJECTS]; /**< Objects. */
	uint16_t	number_of_objects; /**< Number of objects. */
	uint32_t 	timestamp; /**< Timestamp [ms]. */
	AUTO_STATUS	status; /**< Status. */
} AUTO_Objects;

#ifdef WIN32
#pragma pack(pop)
#endif

/*---------------------------------------------------------------------------
** Functions
*/
INLINE void
initObject(
	AUTO_Object *object
){
	memset(object, 0, sizeof(AUTO_Object));
	object->distance = -1.f;
	return;
}

INLINE void
initObjects(
	AUTO_Objects *objects
){
    uint8_t i = 0;
	objects->primary_object_index = AUTO_MAX_NUM_OBJECTS;
	for (i = 0; i < AUTO_MAX_NUM_OBJECTS; i++) {
		initObject(&objects->object[i]);
	}
	objects->number_of_objects = 0;
	objects->timestamp = 0;
	objects->status = AUTO_STA_UNKNOWN;
	return;
}

#ifdef __cplusplus
}
#endif

#endif /* __AUTO_OBJECTS_H__ */
/*---------------------------------------------------------------------------
** End of File
*/