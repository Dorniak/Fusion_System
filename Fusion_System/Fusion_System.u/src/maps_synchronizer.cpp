////////////////////////////////
// RTMaps SDK Component
////////////////////////////////

////////////////////////////////
// Purpose of this module :
////////////////////////////////

#include "maps_synchronizer.h"	// Includes the header of this component

const MAPSTypeFilterBase ValeoStructure = MAPS_FILTER_USER_STRUCTURE(AUTO_Objects);
// Use the macros to declare the inputs
MAPS_BEGIN_INPUTS_DEFINITION(MAPSsynchronizer)
	MAPS_INPUT("Object_I", ValeoStructure, MAPS::FifoReader)
	MAPS_END_INPUTS_DEFINITION

// Use the macros to declare the outputs
MAPS_BEGIN_OUTPUTS_DEFINITION(MAPSsynchronizer)
MAPS_OUTPUT_USER_STRUCTURE("Object_O", AUTO_Objects)
MAPS_END_OUTPUTS_DEFINITION

// Use the macros to declare the properties
MAPS_BEGIN_PROPERTIES_DEFINITION(MAPSsynchronizer)
    //MAPS_PROPERTY("pName",128,false,false)
MAPS_END_PROPERTIES_DEFINITION

// Use the macros to declare the actions
MAPS_BEGIN_ACTIONS_DEFINITION(MAPSsynchronizer)
    //MAPS_ACTION("aName",MAPSsynchronizer::ActionName)
MAPS_END_ACTIONS_DEFINITION

// Use the macros to declare this component (synchronizer) behaviour
MAPS_COMPONENT_DEFINITION(MAPSsynchronizer,"synchronizer","1.0",128,
			  MAPS::Threaded,MAPS::Threaded,
			  -1, // Nb of inputs. Leave -1 to use the number of declared input definitions
			  -1, // Nb of outputs. Leave -1 to use the number of declared output definitions
			  -1, // Nb of properties. Leave -1 to use the number of declared property definitions
			  -1) // Nb of actions. Leave -1 to use the number of declared action definitions

//Initialization: Birth() will be called once at diagram execution startup.			  
void MAPSsynchronizer::Birth()
{
    // Reports this information to the RTMaps console. You can remove this line if you know when Birth() is called in the component lifecycle.
    ReportInfo("Passing through Birth() method");
}

void MAPSsynchronizer::Core() 
{
	readInputs();
	writeOutputs();
   // ReportInfo("Passing through Core() method");
    //Rest(500000);
}

//De-initialization: Death() will be called once at diagram execution shutdown.
void MAPSsynchronizer::Death()
{
    ReportInfo("Passing through Death() method");
}

void MAPSsynchronizer::readInputs()
{
	elt = StartReading(Input("Object_I"));
	Object_I = static_cast<AUTO_Objects*>(elt->Data());
	StopReading(Input("Object_I"));
}

void MAPSsynchronizer::writeOutputs()
{
	_ioOutput = StartWriting(Output("Object_O"));
	AUTO_Objects &list = *static_cast<AUTO_Objects*>(_ioOutput->Data());
	list = *Object_I;
	StopWriting(_ioOutput);
}
