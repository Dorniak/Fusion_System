////////////////////////////////
// RTMaps SDK Component header
////////////////////////////////

#ifndef _Maps_Gating_H
#define _Maps_Gating_H

#include <string>
#include <sstream>
#include <fstream>
#include <math.h>
#include <vector>
// Includes maps sdk library header
#include "maps.hpp"
#include "Match_Objects.h"
#include "auto_objects.h"
#include "Hipotesys.h"
#include "Properties.h"
#include "STATIC_VECTOR.h"

// Declares a new MAPSComponent child class
class MAPSGating : public MAPSComponent 
{
	// Use standard header definition macro
	MAPS_COMPONENT_STANDARD_HEADER_CODE(MAPSGating)
private :
	bool p_save_exp; /*!<boolean used to save experimental results*/
	bool p_save_ass_objs; /*!<boolean used to save associated objects*/
	bool p_save_np_objs; /*!<boolean used to save not perceived objects*/
	bool p_save_nc_objs; /*!<boolean used to save not communicating objects*/
	bool p_save_perception_objs; /*!<boolean used to perception objects*/
	bool p_save_communication_objs; /*!<boolean used to communication objects*/
	bool p_save_hypothesis; /*!<boolean used to save generated hypothesis*/
	bool p_save_gate_objects; /*!<boolean used to save objects located inside gating window*/

	std::ofstream m_ass_objs_file; /*!<file to save associated objects*/
	std::ofstream m_np_objs_file; /*!<file to save not perceived objects*/
	std::ofstream m_nc_objs_file; /*!<file to save not communicating objects*/
	std::ofstream m_perception_objs_file; /*!<file to save perception objects*/
	std::ofstream m_communication_objs_file; /*!<file to save communication objects*/
	std::ofstream m_hypothesis_no_pruning_file; /*!<file to save hypothesis before pruning*/
	std::ofstream m_hypothesis_pruning_file; /*!<file to save hypothesis after pruning*/
	std::ofstream m_gate_objs_file; /*!<file to save objects located inside gating window*/
	std::ofstream m_config_file; /*!<file to save experimental configuration, like important parameters*/

	bool p_debug_report; /*!<boolean used to display debugging information in the console*/
	float p_laser_range; /*!<laser range*/
	float p_laser_angular_resolution; /*!<laser angular resolution*/
	float p_min_nb_impacts; /*!<minimum number of impacts for obstacle dtection*/
	float p_min_width; /*!<minimum width for obstacle dtection*/
	float p_sigma_laser; /*!laser measurement noise value*/
	float p_tx_power; /*!transmission power*/
	float p_ht; /*!transmitter height*/
	float p_hr; /*!receiver height*/
	float p_lambda; /*!communication wavelength*/
	float p_noise_dbm; /*!communication noise level*/
	float p_gating; /*!size of gaiting window obtained from chi2 law applied with 95% of confidence value*/
	float p_hypothesis_pruning; /*!minum probability for hypothesis pruning*/
	float p_occlusion_ratio; /*!weight of the occlusion ratio*/
	float p_communication_prob; /*!fixed communication detection probability*/
	float p_perception_prob; /*!fixed perception detection probability*/

	//int p_color_b; /*!<blue value for displaying objects*/
	//int p_color_g; /*!<green value for displaying objects*/
	//int p_color_r; /*!<red value for displaying objects*/

	//int p_color_b_ass; /*!<blue value for displaying associated objects*/
	//int p_color_g_ass; /*!<green value for displaying associated objects*/
	//int p_color_r_ass; /*!<red value for displaying associated objects*/

	//int p_color_b_np; /*!<blue value for displaying not perceived objects*/
	//int p_color_g_np; /*!<green value for displaying not perceived objects*/
	//int p_color_r_np; /*!<red value for displaying not perceived objects*/

	MAPSIOElt* elt;
	MAPSStreamedString str;
	MATCH_OBJECTS* input_Laser_Matched;
	MATCH_OBJECTS* input_Camera_Matched;
	AUTO_Objects* m_objects_per2; /*!<perception objects*/
	AUTO_Objects* m_objects_com2; /*!<communication objects*/
	MATCH_OBJECTS Laser_Matched;
	MATCH_OBJECTS Camera_Matched;
	AUTO_Objects m_objects_per; /*!<perception objects*/	//Camara
	AUTO_Objects m_objects_com; /*!<communication objects*/ //Laser
	AUTO_Objects m_objects_ass; /*!<associated objects*/
	AUTO_Objects m_objects_np; /*!<not perceived objects*/
	AUTO_Objects m_objects_nc; /*!<not communicating objects*/

	std::vector<s_hypothesis> m_hypothesis_tree; /*!<current hypethesis tree*/
	std::vector<s_hypothesis> m_prev_hypothesis; /*!<previous hypethesis tree*/
	VECTOR_INT m_already_seen_com; /*!<already seen communication tracks*/
	VECTOR_INT m_already_seen_per; /*!<already seen perception tracks*/
	VECTOR_INT m_idx_gate; /*!<index perception obstacles in gating windows*/

	std::stringstream m_ss; /*!<debugging string to display information in command window*/

	VVECTOR_INT m_ass_per_com_meas; /*!<vector of perception obstacles indexes located inside every communication obstacle gating window*/
	VVECTOR_INT m_prev_gate; /*!<list of perception obstacle which have already been inside the communication gating window*/
	int m_max_com_id; /*!<maximum communication identifier*/
	int m_max_hyp_id; /*!<maximum perception identifier*/
	double m_occluded_area_ratio[MAXIMUM_OBJECT_NUMBER]; /*!<occluded area ratio for every object*/
public:
	//Constructors and Destructors
	//////////////////////////////

	

	// Core Functions
	//////////////////


	/*
	* @function ProcessData()
	* @brief Process the data acquired
	* @param system_time: sytem time
	*/
	void ProcessData();

	/*
	* @function SetMessage()
	* @brief Set debuggung information message
	* @param s: debugging message
	*/
	void SetMessage(std::string s) { m_ss.str(s); };

	/*
	* @function GetMessage()
	* @brief Get debiggung information message
	* @return the debugging message
	*/
	std::string GetMessage() { return m_ss.str(); };

	//Private methods
private:
	MAPSIOElt *_ioOutput;
	void readInputs();
	void adaptation();
	void writeOutputs();
	void inicialization();
	/*
	* @function IntializeTree()
	* @brief Initialize hypothesis tree
	*/
	void IntializeTree();
	
	/*
	* @function UpdateTree()
	* @brief Update hypothesis tree
	*/
	void UpdateTree();
	/*
	* @function UpdateGateTracks()
	* @brief Update perception tracks observed inside gating window for every communication obstacle
	*/
	void UpdateGateTracks();
	/*
	* @function PruneHypothesis()
	* @brief Prune hypothesis with a small probability
	*/
	void PruneHypothesis();
	/*
	* @function FusedTracksEstimation()
	* @brief Estimate fused tracks
	*/
	void FusedTracksEstimation();
	/*
	* @function IsAlreadyHere()
	* @brief Find if an objects with identifier id has already been seen
	* @param id: identifier
	* @param already_seen: list of already seen identifiers
	* @return true if id is already seen, false otherwise
	*/
	bool IsAlreadyHere(int id, std::vector<int> & already_seen);
	bool IsAlreadyHere(int id, VECTOR_INT & already_seen);
	/*
	* @function GetIdPer()
	* @brief Get associated perception id
	* @param id_com: communcation obstacle identifier
	* @param hyp: a hypothesis
	* @return the identifer of the perception obstacle associated to communication obstacle with id, id_com. Perception identifier is 0 if communication obstacle is not perceived
	*/
	int GetIdPer(int id_com, s_hypothesis hyp);
	/*
	* @function GetIdxCom()
	* @brief Get index of the branch containing the communication obstacle in an hypothesis
	* @param id_com: communcation obstacle identifier
	* @param hyp: a hypothesis
	* @return the index of the branch containing the communication obstacle identifeir, id_com, in the hypothesis hyp
	*/
	int GetIdxCom(int id_com, s_hypothesis hyp);
	/*
	* @function GetIdxObstacle()
	* @brief Get index of the obstacle in a list of objects from its identifier
	* @param id: obstacle identifier
	* @param objs: list of objects
	* @return the index of the obstacle with identifier id
	*/
	int GetIdxObstacle(int id, AUTO_Objects objs);
	/*
	* @function IsInGate()
	* @brief Determine if a perception object is inside the gating window of a communication object or not
	* @param id_per: perception object index
	* @param i_com: communciation object index
	* @return true if the perception object is inside the gating window of the communication object, false otherwise
	*/
	bool IsInGate(int id_per, int i_com);
	//double QFunction(double x) {return erfc(x/(sqrt(2.0)))/2;};
	/*
	* @function LocationLikelihood()
	* @brief Determine location likelihood value of a perceived obstacle given a communicating obstacle
	* @param x_per: x position with perception
	* @param y_per: y position with perception
	* @param x_com: x position with communication
	* @param y_com: y position with communication
	* @param x_sigma: x standard deviation of a communication obstacle
	* @param y_sigma: y standard deviation of a communication obstacle
	* @param xy_sigma: xy standard deviation of a communication obstacle
	* @return the likelihood value
	*/
	float LocationLikelihood(float x_per, float y_per, float x_com, float y_com, float x_sigma, float y_sigma, float xy_sigma);

	/*
	* @function Factorial()
	* @brief Calculate factorial value
	* @param n: a number
	* @return the factorial value of n
	*/
	long Factorial(long n) { return n > 1 ? (n * Factorial(n - 1)) : 1; };

	/*
	* @function BinomialCoeff()
	* @brief Calculate binomial coefficient
	* @param n: a number
	* @param n: k number
	* @return the binomial coefficient indexed by n and k
	*/
	int BinomialCoeff(int k, int n) {
		if (k>n)
			return 0;
		else if (k == n || k == 0)
			return 1;
		else
			return BinomialCoeff(k - 1, n - 1) + BinomialCoeff(k, n - 1);
	};
};

#endif
