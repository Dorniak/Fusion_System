////////////////////////////////
// RTMaps SDK Component header
////////////////////////////////

#ifndef _Maps_Matching_H
#define _Maps_Matching_H

// Includes maps sdk library header
#include "maps.hpp"
#include "auto_objects.h"
#include <vector>

// Declares a new MAPSComponent child class
class MAPSMatching : public MAPSComponent 
{
	// Use standard header definition macro
	MAPS_COMPONENT_STANDARD_HEADER_CODE(MAPSMatching)
private :
	// Place here your specific methods and attributes
	//
	MAPSStreamedString str;
	AUTO_Objects* ArrayLaserObjects;
	AUTO_Objects* ArrayCameraObjects;
	std::vector<std::vector<int>> LMatched;
	std::vector<std::vector<int>> CMatched;
	void WriteOutputs();
	void findMatches(AUTO_Objects* ArrayLaserObjects, AUTO_Objects* ArrayCameraObjects);
	bool BoxMatching(AUTO_Object Object1, AUTO_Object Object2);

};

#endif
