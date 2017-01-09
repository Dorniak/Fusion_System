////////////////////////////////
// RTMaps SDK Component header
////////////////////////////////

#ifndef _Maps_Bayesian_Fusion_H
#define _Maps_Bayesian_Fusion_H

// Includes maps sdk library header
#include "maps.hpp"

// Declares a new MAPSComponent child class
class MAPSBayesian_Fusion : public MAPSComponent 
{
	// Use standard header definition macro
	MAPS_COMPONENT_STANDARD_HEADER_CODE(MAPSBayesian_Fusion)
private :
	// Place here your specific methods and attributes
	void ProcessData();
	void ReadInputs();
	void WriteOutputs();
	void Predictor();
	void Matching();
	void Join();

};

#endif
