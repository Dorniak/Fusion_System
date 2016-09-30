/**
 * @copyright (c) Valeo Driving Assistance Research.
 * <br>All rights reserved. Subject to limited distribution and restricted disclosure only.
 * @author paulo.resende@valeo.com
 * @file auto_common.h
 * @brief Common data types and macros
 * @version 1.0
 * @date 18-02-2014
 * @bug	No known bugs
 * @warning	No warnings
 */
 
#ifndef __AUTO_COMMON_H__
#define __AUTO_COMMON_H__

#ifdef __cplusplus
extern "C" {
#endif

/*---------------------------------------------------------------------------
** Includes
*/
#include <string.h> // memset

/*---------------------------------------------------------------------------
** Defines and Macros
*/
#ifdef _MSC_VER
  #define INLINE __forceinline /**< Use __forceinline (VC++ specific). */
#else
  #define INLINE inline       /**< Use standard inline. */
#endif
#ifndef DBL_MAX
	#define DBL_MAX 1.7976931348623158e+308 /**< Double maximum positive value. */
#endif
#ifndef DBL_MIN
	#define DBL_MIN 2.2250738585072014e-308 /**< Double minimum positive value. */
#endif
#ifndef FLT_MAX
	#define FLT_MAX 3.402823466e+38F /**< Float maximum positive value. */
#endif
#ifndef FLT_MIN
	#define FLT_MIN 1.175494351e-38F /**< Float minimum positive value. */
#endif
#ifndef TRUE
	#define TRUE 1
#endif
#ifndef FALSE
	#define FALSE 0
#endif
/** Approximation of the mathematical constant \a Pi. */
#define AUTO_PI						3.1415926535897932384626433832795
/** Returns the minimum between \a a and \a b. */
#define AUTO_MIN(a, b)				((a) > (b) ? (b) : (a))
/** Returns the maximum between \a a and \a b. */
#define AUTO_MAX(a, b)				((a) < (b) ? (b) : (a))
/** Returns the maximum between \a min and the minumum between \a a and \a max. */
#define AUTO_MIN_MAX(min, max, a)	(AUTO_MAX((min), AUTO_MIN((a), (max))))
/** Returns the absolute value of \a a. */
#define AUTO_ABS(a)					((a) >= 0  ? (a) : -(a))
/** Returns the sign value of \a a. */
#define AUTO_SIGN(a)				((a) > 0 ? 1 : -1)
/** Returns the integral value that is nearest to \a a, with halfway cases rounded away from zero. */
#define AUTO_ROUND(a)				(int)((a) > 0 ? ((a) + 0.5) : ((a) - 0.5))
/** Converts decimal degrees to radians. */
#define AUTO_DEG2RAD				(AUTO_PI/180.0)
/** Converts radians to decimal degrees. */
#define AUTO_RAD2DEG				(180.0/AUTO_PI)
#define AUTO_ODD(a)					((a)&1)
#define AUTO_EVEN(a)				(!((a)&1))
/** Earth's mean radius in meters. */
#define AUTO_EARTH_RADIUS 			6371000.0

/*---------------------------------------------------------------------------
** Typedefs
*/
/**
 * @defgroup DataTypes Data types definition
 * @{
 */

/** Boolean */
typedef unsigned char			bool_t;
/** plain 8 bit character */
typedef char					char_t;
/** Unsigned 8 bit integer */
typedef unsigned char 			uint8_t;
/** Unsigned 16 bit integer */
typedef unsigned short 			uint16_t;
/** Unsigned 32 bit integer */
typedef unsigned int 			uint32_t;
/** Unsigned 64 bit integer */
typedef unsigned long long		uint64_t;
/** Signed 8 bit integer */
typedef signed char				int8_t;
/** Signed 16 bit integer */
typedef signed short 			int16_t;
/** Signed 32 bit integer */
typedef signed int 				int32_t;
/** Signed 64 bit integer */
typedef signed long long 		int64_t;
/** 32 bit floating-point */
typedef float					float32_t;
/** 64 bit floating-point */
typedef double					float64_t;

/** @} */

/**
 * @enum AUTO_SURFACE_CLASS
 */
typedef enum {
	AUTO_SCL_UNKNOWN = 0, /**< Unknown. */
	AUTO_SCL_TARMAC, /**< Tarmac. */
	AUTO_SCL_CONCRETE, /**< Concrete. */
	AUTO_SCL_PAVING, /**< Paving. */
	AUTO_SCL_GRAVEL, /**< Gravel. */
	AUTO_SCL_SOIL, /**< Soil. */
	AUTO_SCL_SAND, /**< Sand. */
	AUTO_SCL_WATER /**< Water. */
} AUTO_SURFACE_CLASS;

/**
 * @enum AUTO_TURN_SIGNAL
 */
typedef enum {
	AUTO_TSI_UNKNOWN = 0, /**< Unknown. */ 
	AUTO_TSI_OFF, /**< Off. */ 		
	AUTO_TSI_RIGHT, /**< Right turn signal is on. */ 
	AUTO_TSI_LEFT, /**< Left turn signal is on. */ 
	AUTO_TSI_HAZARD /**< Left and right turn signal are on. */ 
} AUTO_TURN_SIGNAL;

/** @enum AUTO_STATUS */
typedef enum {
	AUTO_STA_OFF = 0, /**< Off. */ 
	AUTO_STA_ON, /**< On. */ 
	AUTO_STA_FAILURE, /**< Failure. */ 
	AUTO_STA_UNKNOWN /**< Unknown. */ 
} AUTO_STATUS;

/**
 * @struct AUTO_GnssData
 * @brief Global navigation satellite system data
 */
typedef struct {
	float64_t 	latitude; /**< Latitude [deg]. */ 
	float64_t 	longitude; /**< Longitude [deg]. */
	float32_t 	altitude; /**< Orthometric height, mean-sea-level reference when available or 0 [m]. */
	float32_t 	heading; /**< Heading to North [deg]. */
	float32_t 	latitude_sigma; /**< Standard deviation of latitude [m]. */
	float32_t 	longitude_sigma; /**< Standard deviation of latitude [m]. */
	float32_t 	altitude_sigma; /**< Standard deviation of altitude [m]. */
	float32_t 	heading_sigma; /**< Standard deviation of heading [deg]. */
	uint32_t	timestamp; /**< Timestamp [ms]. */
} AUTO_GnssData;

/*---------------------------------------------------------------------------
** Functions
*/

#ifdef __cplusplus
}
#endif

#endif /* __AUTO_COMMON_H__ */
/*---------------------------------------------------------------------------
** End of File
*/