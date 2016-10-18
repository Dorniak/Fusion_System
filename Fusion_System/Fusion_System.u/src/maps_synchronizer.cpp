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
	MAPS_INPUT("BBoxLaser", ValeoStructure, MAPS::FifoReader)
	MAPS_INPUT("BBoxCamera", ValeoStructure, MAPS::FifoReader)
	MAPS_INPUT("LaserObject", ValeoStructure, MAPS::FifoReader)
	MAPS_INPUT("CameraObject", ValeoStructure, MAPS::FifoReader)
	MAPS_INPUT("Final", ValeoStructure, MAPS::FifoReader)
	MAPS_END_INPUTS_DEFINITION

// Use the macros to declare the outputs
MAPS_BEGIN_OUTPUTS_DEFINITION(MAPSsynchronizer)
MAPS_OUTPUT_USER_STRUCTURE("LaserObjects_O", AUTO_Objects)
MAPS_OUTPUT_USER_STRUCTURE("CameraObjects_O", AUTO_Objects)
MAPS_OUTPUT_USER_STRUCTURE("Output_estimation_O", AUTO_Objects)
MAPS_OUTPUT_USER_STRUCTURE("BBoxLaser_O", AUTO_Objects)
MAPS_OUTPUT_USER_STRUCTURE("BBoxCamera_O", AUTO_Objects)
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

	LBBox3 = LBBox2;
	LBBox2 = LBBox;
	
	CBBox3 = CBBox2;
	CBBox2 = CBBox;

	LObjects3 = LObjects2;
	LObjects2 = LObjects;

	CObjects3 = CObjects2;
	CObjects2 = CObjects;

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
	while (!DataAvailableInFIFO(Input("BBoxLaser")) || !DataAvailableInFIFO(Input("BBoxCamera")) || !DataAvailableInFIFO(Input("LaserObject")) || !DataAvailableInFIFO(Input("CameraObject")) ) {}
	Rest(1000);
	//Leer objetos del BBoxlaser
	if (DataAvailableInFIFO(Input("BBoxLaser")))
	{
		elt = StartReading(Input("BBoxLaser"));
		LBBox = static_cast<AUTO_Objects*>(elt->Data());
		StopReading(Input("BBoxLaser"));
	}

	//Leer objetos de la BBoxcamara
	if (DataAvailableInFIFO(Input("BBoxCamera")))
	{
		elt = StartReading(Input("BBoxCamera"));
		CBBox = static_cast<AUTO_Objects*>(elt->Data());
		StopReading(Input("BBoxCamera"));
	}

	//Leer objetos del laser
	if (DataAvailableInFIFO(Input("LaserObject")))
	{
		elt = StartReading(Input("LaserObject"));
		LObjects = static_cast<AUTO_Objects*>(elt->Data());
		StopReading(Input("LaserObject"));
	}

	//Leer objetos de la camara
	if (DataAvailableInFIFO(Input("CameraObject")))
	{
		elt = StartReading(Input("CameraObject"));
		CObjects = static_cast<AUTO_Objects*>(elt->Data());
		StopReading(Input("CameraObject"));
	}

	//Leer objetos de la camara
	if (DataAvailableInFIFO(Input("Final")))
	{
		elt = StartReading(Input("Final"));
		Result = static_cast<AUTO_Objects*>(elt->Data());
		StopReading(Input("Final"));
	}
}

void MAPSsynchronizer::writeOutputs()
{
	if (LBBox3 != nullptr) {
		_ioOutput = StartWriting(Output("BBoxLaser_O"));
		AUTO_Objects &list = *static_cast<AUTO_Objects*>(_ioOutput->Data());
		list = *LBBox3;
		StopWriting(_ioOutput);
	}
	if (CBBox3 != nullptr) {
		_ioOutput = StartWriting(Output("BBoxCamera_O"));
		AUTO_Objects &list1 = *static_cast<AUTO_Objects*>(_ioOutput->Data());
		list1 = *CBBox3;
		StopWriting(_ioOutput);
	}
	if (LObjects3 != nullptr) {
		_ioOutput = StartWriting(Output("LaserObjects_O"));
		AUTO_Objects &list2 = *static_cast<AUTO_Objects*>(_ioOutput->Data());
		list2 = *LObjects3;
		StopWriting(_ioOutput);
	}
	if (CObjects3 != nullptr) {
		_ioOutput = StartWriting(Output("CameraObjects_O"));
		AUTO_Objects &list3 = *static_cast<AUTO_Objects*>(_ioOutput->Data());
		list3 = *CObjects3;
		StopWriting(_ioOutput);
	}
	if (Result != nullptr) {
		_ioOutput = StartWriting(Output("Output_estimation_O"));
		AUTO_Objects &list4 = *static_cast<AUTO_Objects*>(_ioOutput->Data());
		list4 = *Result;
		StopWriting(_ioOutput);
	}
}
