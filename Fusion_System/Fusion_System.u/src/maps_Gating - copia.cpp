////////////////////////////////
// RTMaps SDK Component
////////////////////////////////

////////////////////////////////
// Purpose of this module :
////////////////////////////////

#include "maps_Gating.h"	// Includes the header of this component

// Use the macros to declare the inputs
MAPS_BEGIN_INPUTS_DEFINITION(MAPSGating)
    //MAPS_INPUT("iName",MAPS::FilterInteger32,MAPS::FifoReader)
	MAPS_INPUT("input1", MAPS::FilterInteger32, MAPS::FifoReader)
	MAPS_INPUT("input2", MAPS::FilterInteger32, MAPS::FifoReader)
	MAPS_INPUT("input3", MAPS::FilterInteger32, MAPS::FifoReader)
	MAPS_INPUT("input4", MAPS::FilterInteger32, MAPS::FifoReader)
MAPS_END_INPUTS_DEFINITION

// Use the macros to declare the outputs
MAPS_BEGIN_OUTPUTS_DEFINITION(MAPSGating)
    //MAPS_OUTPUT("oName",MAPS::Integer32,NULL,NULL,1)
	MAPS_OUTPUT("output1", MAPS::Integer32, NULL, NULL, 1)
	MAPS_OUTPUT("output2", MAPS::Integer32, NULL, NULL, 1)
MAPS_END_OUTPUTS_DEFINITION

// Use the macros to declare the properties
MAPS_BEGIN_PROPERTIES_DEFINITION(MAPSGating)
    //MAPS_PROPERTY("pName",128,false,false)
MAPS_END_PROPERTIES_DEFINITION

// Use the macros to declare the actions
MAPS_BEGIN_ACTIONS_DEFINITION(MAPSGating)
    //MAPS_ACTION("aName",MAPSGating::ActionName)
MAPS_END_ACTIONS_DEFINITION

// Use the macros to declare this component (Gating) behaviour
MAPS_COMPONENT_DEFINITION(MAPSGating,"Gating","1.0",128,
			  MAPS::Threaded,MAPS::Threaded,
			  -1, // Nb of inputs. Leave -1 to use the number of declared input definitions
			  -1, // Nb of outputs. Leave -1 to use the number of declared output definitions
			  -1, // Nb of properties. Leave -1 to use the number of declared property definitions
			  -1) // Nb of actions. Leave -1 to use the number of declared action definitions

//Initialization: Birth() will be called once at diagram execution startup.			  
void MAPSGating::Birth()
{
    // Reports this information to the RTMaps console. You can remove this line if you know when Birth() is called in the component lifecycle.
    ReportInfo("Passing through Birth() method");
}

//ATTENTION: 
//	Make sure there is ONE and ONLY ONE blocking function inside this Core method.
//	Consider that Core() will be called inside an infinite loop while the diagram is executing.
//	Something similar to: 
//		while (componentIsRunning) {Core();}
//
//	Usually, the one and only blocking function is one of the following:
//		* StartReading(MAPSInput& input); //Data request on a single BLOCKING input. A "blocking input" is an input declared as FifoReader, LastOrNextReader, Wait4NextReader or NeverskippingReader (declaration happens in MAPS_INPUT: see the beginning of this file). A SamplingReader input is non-blocking: StartReading will not block with a SamplingReader input.
//		* StartReading(int nCount, MAPSInput* inputs[], int* inputThatAnswered, int nCountEvents = 0, MAPSEvent* events[] = NULL); //Data request on several BLOCKING inputs.
//		* SynchroStartReading(int nb, MAPSInput** inputs, MAPSIOElt** IOElts, MAPSInt64 synchroTolerance = 0, MAPSEvent* abortEvent = NULL); // Synchronized reading - waiting for samples with same or nearly same timestamps on several BLOCKING inputs.
//		* Wait(MAPSTimestamp t); or Rest(MAPSDelay d); or MAPS::Sleep(MAPSDelay d); //Pauses the current thread for some time. Can be used for instance in conjunction with StartReading on a SamplingReader input (in which case StartReading is not blocking).
//		* Any blocking grabbing function or other data reception function from another API (device driver,etc.). In such case, make sure this function cannot block forever otherwise it could freeze RTMaps when shutting down diagram.
//**************************************************************************/
//	In case of no blocking function inside the Core, your component will consume 100% of a CPU.
//  Remember that the StartReading function used with an input declared as a SamplingReader is not blocking.
//	In case of two or more blocking functions inside the Core, this is likely to induce synchronization issues and data loss. (Ex: don't call two successive StartReading on FifoReader inputs.)
/***************************************************************************/
void MAPSGating::Core() 
{
    // Reports this information to the RTMaps console. You can remove this line if you know when Core() is called in the component lifecycle.
    ReportInfo("Passing through Core() method");

    // Sleeps during 500 milliseconds (500000 microseconds).
	//This line will most probably have to be removed when you start programming your component.
	// Replace it with another blocking function. (StartReading?)
    Rest(500000);
}

//De-initialization: Death() will be called once at diagram execution shutdown.
void MAPSGating::Death()
{
    // Reports this information to the RTMaps console. You can remove this line if you know when Death() is called in the component lifecycle.
    ReportInfo("Passing through Death() method");
}














//Constructors and destructors
///////////////////////////////
MAPSGating::MAPSGating(bool use_laser_points, bool save_exp, std::string save_path) {

	this->p_use_laser_points = use_laser_points;
	this->m_objects_per.number_of_objects = 0;
	this->m_objects_com.number_of_objects = 0;
	this->m_objects_ass.number_of_objects = 0;
	this->m_objects_np.number_of_objects = 0;
	this->m_objects_nc.number_of_objects = 0;

	this->m_max_com_id = 0;
	this->m_max_hyp_id = 0;

	for (int i = 0; i<(int)m_ass_per_com_meas.size(); i++) {
		m_ass_per_com_meas[i].clear();
	}
	m_ass_per_com_meas.clear();
	m_hypothesis_tree.clear();
	m_prev_gate.clear();
	m_already_seen_com.clear();
	m_already_seen_per.clear();
	m_prev_hypothesis.clear();

	this->p_save_exp = save_exp;
	this->p_save_path = save_path;

	if (this->p_save_exp) {
		this->p_save_path = Comm_Tools::CreateSaveDirectory(this->p_save_path);

		std::string ass_objs_file = this->p_save_path + "associated_objects.csv";
		this->m_ass_objs_file.open(ass_objs_file.c_str(), std::ios::out);

		std::string np_objs_file = this->p_save_path + "not_perceived_objects.csv";
		this->m_np_objs_file.open(np_objs_file.c_str(), std::ios::out);

		std::string nc_objs_file = this->p_save_path + "not_communicating_objects.csv";
		this->m_nc_objs_file.open(nc_objs_file.c_str(), std::ios::out);

		std::string per_objs_file = this->p_save_path + "input_perception_tracks.csv";
		this->m_perception_objs_file.open(per_objs_file.c_str(), std::ios::out);

		std::string com_objs_file = this->p_save_path + "input_communication_tracks.csv";
		this->m_communication_objs_file.open(com_objs_file.c_str(), std::ios::out);

		std::string hyp_no_pruning_file = this->p_save_path + "hypothesis_tree_before_pruning.csv";
		this->m_hypothesis_no_pruning_file.open(hyp_no_pruning_file.c_str(), std::ios::out);

		std::string hyp_pruning_file = this->p_save_path + "hypothesis_tree_after_pruning.csv";
		this->m_hypothesis_pruning_file.open(hyp_pruning_file.c_str(), std::ios::out);

		std::string gate_objs_file = this->p_save_path + "gate_objects.csv";
		this->m_gate_objs_file.open(gate_objs_file.c_str(), std::ios::out);

		std::string config_file = this->p_save_path + "config.txt";
		this->m_config_file.open(config_file.c_str(), std::ios::out);
	}
}

MAPSGating::~MAPSGating() {

	for (int i = 0; i<(int)m_ass_per_com_meas.size(); i++) {
		m_ass_per_com_meas[i].clear();
	}
	m_ass_per_com_meas.clear();
	m_hypothesis_tree.clear();
	m_prev_gate.clear();
	m_already_seen_com.clear();
	m_already_seen_per.clear();
	m_prev_hypothesis.clear();

	if (this->p_save_exp) {
		this->m_ass_objs_file.close();
		this->m_np_objs_file.close();
		this->m_nc_objs_file.close();
		this->m_perception_objs_file.close();
		this->m_communication_objs_file.close();
		this->m_communication_objs_file.close();
		this->m_gate_objs_file.close();
		this->m_config_file.close();
	}
}

//Core functions
////////////////
//ReadProperties
int MAPSGating::ReadProperties(std::string xml_file) {
	/* Load XML document */
	TiXmlDocument doc(xml_file.c_str());
	if (!doc.LoadFile()) {
		return false;
	}

	//Initialisation of the XmlMap Table
	TiXmlHandle hdl(&doc);

	TiXmlElement *el = hdl.FirstChildElement("obstacle_detection").Element();
	if (!el) { return false; }

	TiXmlElement *el_fus = el->FirstChildElement("fusion_v2p");
	if (!el_fus) { return false; }

	TiXmlElement *el_deb_rep = el_fus->FirstChildElement("debug_report");
	if (!el_deb_rep) { return false; }

	if (el_deb_rep->Attribute("debug")) p_debug_report = to_bool(el_deb_rep->Attribute("debug"));

	TiXmlElement *el_det = el_fus->FirstChildElement("detection");
	if (!el_det) { return false; }

	if (el_det->Attribute("laser_range")) p_laser_range = (float)atof(el_det->Attribute("laser_range"));
	if (el_det->Attribute("angular_resolution")) p_laser_angular_resolution = (float)atof(el_det->Attribute("angular_resolution"));
	if (el_det->Attribute("min_nb_impacts")) p_min_nb_impacts = (float)atof(el_det->Attribute("min_nb_impacts"));
	if (el_det->Attribute("min_obj_width")) p_min_width = (float)atof(el_det->Attribute("min_obj_width"));
	if (el_det->Attribute("sigma_laser")) p_sigma_laser = (float)atof(el_det->Attribute("sigma_laser"));
	if (el_det->Attribute("tx_power")) p_tx_power = (float)atof(el_det->Attribute("tx_power"));
	if (el_det->Attribute("ht")) p_ht = (float)atof(el_det->Attribute("ht"));
	if (el_det->Attribute("hr")) p_hr = (float)atof(el_det->Attribute("hr"));
	if (el_det->Attribute("lambda")) p_lambda = (float)atof(el_det->Attribute("lambda"));
	if (el_det->Attribute("noise_dbm")) p_noise_dbm = (float)atof(el_det->Attribute("noise_dbm"));
	if (el_det->Attribute("perception_prob")) p_perception_prob = (float)atof(el_det->Attribute("perception_prob"));
	if (el_det->Attribute("communication_prob")) p_communication_prob = (float)atof(el_det->Attribute("communication_prob"));

	TiXmlElement *el_thr = el_fus->FirstChildElement("threshold");
	if (!el_thr) { return false; }

	if (el_thr->Attribute("gating")) p_gating = (float)atof(el_thr->Attribute("gating"));
	if (el_thr->Attribute("hypothesis_pruning")) p_hypothesis_pruning = (float)atof(el_thr->Attribute("hypothesis_pruning"));
	if (el_thr->Attribute("occlusion_ratio")) p_occlusion_ratio = (float)atof(el_thr->Attribute("occlusion_ratio"));

	TiXmlElement *el_save = el_fus->FirstChildElement("saving_parameters");
	if (!el_save) { return false; }

	if (el_save->Attribute("associated_objects")) p_save_ass_objs = to_bool(el_save->Attribute("associated_objects"));
	if (el_save->Attribute("not_percieved_objects")) p_save_np_objs = to_bool(el_save->Attribute("not_percieved_objects"));
	if (el_save->Attribute("not_communicating_objects")) p_save_nc_objs = to_bool(el_save->Attribute("not_communicating_objects"));
	if (el_save->Attribute("perception_tracks")) p_save_perception_objs = to_bool(el_save->Attribute("perception_tracks"));
	if (el_save->Attribute("communication_tracks")) p_save_communication_objs = to_bool(el_save->Attribute("communication_tracks"));
	if (el_save->Attribute("hypothesis_tree")) p_save_hypothesis = to_bool(el_save->Attribute("hypothesis_tree"));
	if (el_save->Attribute("gating_window")) p_save_gate_objects = to_bool(el_save->Attribute("gating_window"));

	TiXmlElement *el_color = el_fus->FirstChildElement("object_color");
	if (!el_color) { return false; }

	if (el_color->Attribute("blue")) p_color_b = atoi(el_color->Attribute("blue"));
	if (el_color->Attribute("green")) p_color_g = atoi(el_color->Attribute("green"));
	if (el_color->Attribute("red")) p_color_r = atoi(el_color->Attribute("red"));

	el_color = el_fus->FirstChildElement("ass_tracks_color");
	if (!el_color) { return false; }

	if (el_color->Attribute("blue")) p_color_b_ass = atoi(el_color->Attribute("blue"));
	if (el_color->Attribute("green")) p_color_g_ass = atoi(el_color->Attribute("green"));
	if (el_color->Attribute("red")) p_color_r_ass = atoi(el_color->Attribute("red"));

	el_color = el_fus->FirstChildElement("np_tracks_color");
	if (!el_color) { return false; }

	if (el_color->Attribute("blue")) p_color_b_np = atoi(el_color->Attribute("blue"));
	if (el_color->Attribute("green")) p_color_g_np = atoi(el_color->Attribute("green"));
	if (el_color->Attribute("red")) p_color_r_np = atoi(el_color->Attribute("red"));

	if (this->p_save_exp) {
		this->m_config_file << "XML params file path: " << xml_file << "\n";
	}
	return 0;
}

//ReadInputs
int MAPSGating::ReadInputs(const objects & objs_per, const objects & objs_com) {

	UtilsObject::PrintObjects(this->m_objects_per, objs_per);
	UtilsObject::PrintObjects(this->m_objects_com, objs_com);

	return 0;
}

int MAPSGating::ReadInputs(const points_3D & pts) {

	memcpy(&this->m_laser_points, &pts, sizeof(points_3D));

	return 0;
}

//ProcessData
int MAPSGating::ProcessData(double system_time) {

	this->m_objects_ass.nb_objects = 0;
	this->m_objects_np.nb_objects = 0;
	this->m_objects_nc.nb_objects = 0;

	// Initialize hypothesis tree
	IntializeTree();

	// Calculate gating
	Gating();

	if (this->p_save_exp&&this->p_save_gate_objects) {
		SaveHypothesis(this->m_hypothesis_tree, this->m_hypothesis_no_pruning_file, system_time);
	}

	if (this->p_use_laser_points) {
		OcclusionAreaRatio();

		for (int i_c = 0; i_c<(int)this->m_objects_com.nb_objects; i_c++) {
			m_ss << "Communicating Object " << i_c << ": occlusion ratio (" << m_occluded_area_ratio[i_c] << ")\n";
		}
	}

	// Update hypothesis tree
	UpdateTree();

	for (int h = 0; h<(int)this->m_hypothesis_tree.size(); h++) {
		m_ss << "Hypothesis " << this->m_hypothesis_tree[h].id << "(";
		for (int ass = 0; ass<(int)this->m_hypothesis_tree[h].assoc_vec.size(); ass++) {
			m_ss << "Com_" << this->m_hypothesis_tree[h].assoc_vec[ass].id_com << "-Per_" << this->m_hypothesis_tree[h].assoc_vec[ass].id_per;
		}
		m_ss << ") : measurement likelihood (" << this->m_hypothesis_tree[h].likelihood_meas
			<< "), branch likelihood (" << this->m_hypothesis_tree[h].likelihood_branch
			<< "), branch NT likelihood (" << this->m_hypothesis_tree[h].likelihood_branch_nt
			<< "), N NT h (" << this->m_hypothesis_tree[h].n_nt_h
			<< "), N NT com (" << this->m_hypothesis_tree[h].n_nt_com
			<< "), branch DT likelihood (" << this->m_hypothesis_tree[h].likelihood_branch_dt
			<< "), N NT Per (" << this->m_hypothesis_tree[h].n_nt_per
			<< "), N Prev Com NP (" << this->m_hypothesis_tree[h].n_prev_com_np
			<< "), detetection probability (" << this->m_hypothesis_tree[h].detection_prob
			<< "), posterior probability (" << this->m_hypothesis_tree[h].prob << ")\n";
	}

	if (this->p_save_exp&&this->p_save_hypothesis) {
		SavePerceptionTracks(this->m_gate_objs_file, system_time);
	}

	// Prune hypothesis
	PruneHypothesis();

	m_ss << "Hypothesis after pruning \n";
	for (int h = 0; h<(int)this->m_hypothesis_tree.size(); h++) {
		m_ss << "Hypothesis " << this->m_hypothesis_tree[h].id << " with probability " << this->m_hypothesis_tree[h].prob << "\n";
	}

	// Update the list of tracks insed gating window
	UpdateGateTracks();

	// estimated fused tracks
	FusedTracksEstimation();

	if (this->p_save_exp) {
		if (this->p_save_ass_objs) {
			SaveROIs(this->m_objects_ass, this->m_ass_objs_file, system_time);
		}

		if (this->p_save_np_objs) {
			SaveROIs(this->m_objects_np, this->m_np_objs_file, system_time);
		}

		if (this->p_save_nc_objs) {
			SaveROIs(this->m_objects_nc, this->m_nc_objs_file, system_time);
		}

		if (this->p_save_perception_objs) {
			SaveROIs(this->m_objects_per, this->m_perception_objs_file, system_time);
		}

		if (this->p_save_communication_objs) {
			SaveCommunicationTracks(this->m_communication_objs_file, system_time);
		}

		if (this->p_save_hypothesis) {
			SaveHypothesis(this->m_hypothesis_tree, this->m_hypothesis_pruning_file, system_time);
		}
	}

	return 0;
}

//WriteOutputs
int MAPSGating::WriteOutputs(objects & o_objs, int o_type) {

	switch (o_type) {
	case V2PFUSION_OUTPUT_FUSED: {
		o_objs.nb_objects = 0;
		for (int i = 0; i<this->m_objects_ass.nb_objects; i++) {
			if (o_objs.nb_objects >= MAXIMUM_OBJECT_NUMBER) {
				break;
			}
			UtilsObject::PrintObject(o_objs.object[i], this->m_objects_ass.object[i]);
			o_objs.nb_objects++;
		}
		for (int i = 0; i<this->m_objects_np.nb_objects; i++) {
			if (o_objs.nb_objects >= MAXIMUM_OBJECT_NUMBER) {
				break;
			}
			UtilsObject::PrintObject(o_objs.object[this->m_objects_ass.nb_objects + i], this->m_objects_np.object[i]);
			o_objs.nb_objects++;
		}
		for (int i = 0; i<this->m_objects_nc.nb_objects; i++) {
			if (o_objs.nb_objects >= MAXIMUM_OBJECT_NUMBER) {
				break;
			}
			UtilsObject::PrintObject(o_objs.object[this->m_objects_np.nb_objects + this->m_objects_ass.nb_objects + i], this->m_objects_nc.object[i]);
			o_objs.nb_objects++;
		}
		break;
	}
	case V2PFUSION_OUTPUT_ASS: {
		UtilsObject::PrintObjects(o_objs, this->m_objects_ass);
		break;
	}
	case V2PFUSION_OUTPUT_NP: {
		UtilsObject::PrintObjects(o_objs, this->m_objects_np);
		break;
	}
	case V2PFUSION_OUTPUT_NC: {
		UtilsObject::PrintObjects(o_objs, this->m_objects_nc);
		break;
	}
	case V2PFUSION_OUTPUT_GATE: {
		o_objs.nb_objects = 0;
		for (int i = 0; i<(int)m_idx_gate.size(); i++) {
			if (o_objs.nb_objects<MAXIMUM_OBJECT_NUMBER) {
				UtilsObject::PrintObject(o_objs.object[o_objs.nb_objects], this->m_objects_per.object[m_idx_gate[i]]);
				o_objs.nb_objects++;
			}
		}
		break;
	}
	}
	return 0;
}

int MAPSGating::WriteHypothesisTree(std::vector<s_hypothesis> & o_hyp) {
	o_hyp.clear();

	for (int h = 0; h<(int)m_hypothesis_tree.size(); h++) {
		o_hyp.push_back(m_hypothesis_tree[h]);
	}

	return 0;
}

int MAPSGating::WriteOccludedAreaRatio(double *occ_ratio, int nb_objects) {
	for (int i = 0; i<nb_objects; i++) {
		occ_ratio[i] = m_occluded_area_ratio[i];
	}

	return 0;
}

void MAPSGating::IntializeTree() {
	m_prev_hypothesis.clear();

	// Copy hypothesis to previous hypothesis
	for (int i = 0; i<(int)m_hypothesis_tree.size(); i++) {
		m_prev_hypothesis.push_back(m_hypothesis_tree[i]);
	}

	m_hypothesis_tree.clear();

	// if no previous hypothesis, initalise an hypothesis with no information
	if (m_prev_hypothesis.empty()) {
		this->m_max_hyp_id = 1;
		s_hypothesis h;
		h.id = this->m_max_hyp_id;
		h.already_seen_per.clear();
		h.assoc_vec.clear();
		h.detection_prob = 1;
		h.likelihood_meas = 1;
		h.likelihood_branch = 1;
		h.prob = 1;
		h.n_nt_h = 0;
		//h.n_nt_np_h=0;
		//h.n_nt_per_h_fact=1;
		//h.n_nt_np_h_fact=1;
		h.likelihood_branch_nt = 1;
		h.likelihood_branch_dt = 1;
		m_prev_hypothesis.push_back(h);
	}
}

void MAPSGating::Gating() {

	float dist_mahalanobis;
	float x, y, sigma_x, sigma_y, sigma_xy, c;
	for (int i = 0; i<(int)m_ass_per_com_meas.size(); i++) {
		m_ass_per_com_meas[i].clear();
	}
	m_idx_gate.clear();
	//m_ass_per_com_meas.clear();
	m_ass_per_com_meas.resize(m_objects_com.nb_objects);

	// calculate gating window for every communication obstacle
	for (int i = 0; i<this->m_objects_com.nb_objects; i++) {
		sigma_x = this->m_objects_com.object[i].x_sigma;
		sigma_y = this->m_objects_com.object[i].y_sigma;
		sigma_xy = this->m_objects_com.object[i].xy_sigma;
		c = sigma_x*sigma_y - pow(sigma_xy, 2);

		for (int j = 0; j<this->m_objects_per.nb_objects; j++) {
			x = this->m_objects_com.object[i].x_rel - this->m_objects_per.object[j].x_rel;
			y = this->m_objects_com.object[i].y_rel - this->m_objects_per.object[j].y_rel;
			dist_mahalanobis = (x*(x*sigma_y - y*sigma_xy) + y*(y*sigma_x - x*sigma_xy)) / c;
			if (dist_mahalanobis<this->p_gating) {
				this->m_ass_per_com_meas[i].push_back(j);

				if (!IsAlreadyHere(j, m_idx_gate)) {
					m_idx_gate.push_back(j);
				}
				//UtilsObject::PrintObject(this->m_objects_ass.object[this->m_objects_ass.nb_objects],this->m_objects_per.object[j]);
				//this->m_objects_ass.nb_objects++;
			}
		}

		if (this->m_objects_com.object[i].id>m_max_com_id) {
			m_max_com_id = this->m_objects_com.object[i].id;
			m_prev_gate.resize(m_max_com_id);
		}
		//UtilsObject::PrintObject(this->m_objects_np.object[this->m_objects_np.nb_objects],this->m_objects_com.object[i]);
		//this->m_objects_np.nb_objects++;
	}
}

void MAPSGating::UpdateTree() {
	std::vector<int> nt_com_idx;
	std::vector<int> nt_per_idx;
	std::vector<int> dt_com_idx;
	std::vector<int> dt_per_idx;
	nt_com_idx.clear();
	dt_com_idx.clear();

	int idx_per, idx_com;
	int n_nt_per(0);
	m_already_seen_per.clear();
	float sum_prob(0);

	// Set perception tracks which have already been detected, i.e., DT tracks
	for (int i = 0; i<this->m_objects_com.nb_objects; i++) {
		/*if (IsAlreadyHere(this->m_objects_com.object[i].id, m_already_seen_com)) {
		dt_com_idx.push_back(i);
		} else {
		nt_com_idx.push_back(i);
		m_already_seen_com.push_back(this->m_objects_com.object[i].id);
		}*/

		for (int i_p = 0; i_p<(int)this->m_ass_per_com_meas[i].size(); i_p++) {
			idx_per = this->m_objects_per.object[this->m_ass_per_com_meas[i][i_p]].id;
			if (!IsAlreadyHere(idx_per, m_already_seen_per) &&
				!IsAlreadyHere(idx_per, m_prev_gate[this->m_objects_com.object[i].id - 1])) {
				n_nt_per++;
				m_already_seen_per.push_back(idx_per);
				//m_prev_gate[this->m_objects_com.object[i].id-1].push_back(idx_per);
			}
		}
	}

	// Generate the new hypothesis tree from the previous hypothesis tree 
	for (int i = 0; i<(int)m_prev_hypothesis.size(); i++) {
		int n_prev_com_ass(0), n_prev_com_np(0), n_nt_com(0);
		int id_per;
		int i_start, i_end, i_start_inter, i_end_inter;
		bool update_hypothesis(false);
		std::vector<int> i_prev_np;
		std::vector<int> i_nt_com;

		// Counting the number of associations and not perceived tracks at previous instant and the number of new communication tracks at current instant
		for (int id_c = 0; id_c<m_objects_com.nb_objects; id_c++) {
			update_hypothesis = true;
			id_per = GetIdPer(m_objects_com.object[id_c].id, m_prev_hypothesis[i]);
			if (id_per>0) {
				if (IsInGate(id_per, id_c)) {
					n_prev_com_ass++;
				}
				else {
					// Delete the hypothesis if the associated perception track is not in the gating window any more
					update_hypothesis = false;
					break;
				}
			}
			else if (id_per == 0) {
				i_prev_np.push_back(id_c);
				n_prev_com_np++;
			}
			else {
				i_nt_com.push_back(id_c);
				n_nt_com++;
			}
		}

		if (!update_hypothesis) {
			continue;
		}

		i_start = (int)m_hypothesis_tree.size();

		// Generate a new hypothesis from the previous hypothsesis
		m_hypothesis_tree.push_back(m_prev_hypothesis[i]);
		//m_hypothesis_tree[i_start].id = i_start;
		m_hypothesis_tree[i_start].detection_prob = 1;
		m_hypothesis_tree[i_start].likelihood_meas = 1;
		m_hypothesis_tree[i_start].likelihood_branch = 1;
		m_hypothesis_tree[i_start].n_nt_h = 0;
		//m_hypothesis_tree[i_start].n_nt_np_h=0;
		//m_hypothesis_tree[i_start].n_nt_per_h_fact=1;
		//m_hypothesis_tree[i_start].n_nt_np_h_fact=1;
		m_hypothesis_tree[i_start].likelihood_branch_nt = 1;
		m_hypothesis_tree[i_start].likelihood_branch_dt = 1;

		// Determine the first and last index corresponding to the newly generates hypothesis
		i_end = i_start + 1;
		i_start_inter = i_start;
		i_end_inter = i_end;
		int i_inter;

		// Generate new hypothesis from not perceived obstacles
		// These three loops are supposed to generate all the possible combinations from a given hypothesis a previous instant
		// This should be tested more carefully and intensivelly
		for (int i_np = 0; i_np<(int)i_prev_np.size(); i_np++) {
			// Generate new hypothesis with perception obstacle located inside the gating window
			for (int i_p = 0; i_p<(int)this->m_ass_per_com_meas[i_np].size(); i_p++) {
				idx_per = this->m_objects_per.object[this->m_ass_per_com_meas[i_np][i_p]].id;
				i_inter = i_start_inter;
				// Generate new hypothesis for every possible association inside the gating window
				while (i_inter<i_end_inter) {
					if (!IsAlreadyHere(idx_per, m_hypothesis_tree[i_inter].already_seen_per) &&
						!IsAlreadyHere(idx_per, m_prev_gate[this->m_objects_com.object[i_np].id - 1])) {
						// change the associated perception track id and increase hypothesis id
						m_hypothesis_tree.push_back(m_hypothesis_tree[i_inter]);
						int idx_com = GetIdxCom(m_objects_com.object[i_np].id, m_hypothesis_tree[i_end]);
						if (idx_com >= 0) {
							m_hypothesis_tree[i_end].assoc_vec[idx_com].id_per = idx_per;
						}
						m_hypothesis_tree[i_end].already_seen_per.push_back(idx_per);
						this->m_max_hyp_id++;
						m_hypothesis_tree[i_end].id = this->m_max_hyp_id;
						i_end++;
					}
					i_inter++;
				}
			}
			i_start_inter = i_end_inter;
			i_end_inter = i_end;
		}

		i_start_inter = i_start;
		//i_end = i_start+1;
		i_end_inter = i_end;
		s_ass ass_per_com;


		// Add new communication tracks to the hypothsis tree
		// These tracks are intiated as not perceived
		for (int h = i_start; h<i_end; h++) {
			for (int i_nt = 0; i_nt<(int)i_nt_com.size(); i_nt++) {
				ass_per_com.id_com = m_objects_com.object[i_nt].id;
				ass_per_com.id_per = 0;
				m_hypothesis_tree[h].n_nt_h++;
				m_hypothesis_tree[h].assoc_vec.push_back(ass_per_com);
			}
		}


		// Calculate all the possible association for the new communicating tracks and generate new possibilities from this point
		i_start_inter = i_start;
		i_end_inter = i_end;
		for (int i_nt = 0; i_nt<(int)i_nt_com.size(); i_nt++) {
			for (int i_p = 0; i_p<(int)this->m_ass_per_com_meas[i_nt].size(); i_p++) {
				idx_per = this->m_objects_per.object[this->m_ass_per_com_meas[i_nt][i_p]].id;
				i_inter = i_start_inter;
				while (i_inter<i_end_inter) {
					if (!IsAlreadyHere(idx_per, m_hypothesis_tree[i_inter].already_seen_per)) {
						m_hypothesis_tree.push_back(m_hypothesis_tree[i_inter]);

						if (IsAlreadyHere(idx_per, m_prev_gate[this->m_objects_com.object[i_nt].id - 1])) {
							m_hypothesis_tree[i_end].n_nt_h--;
						}

						int idx_com = GetIdxCom(m_objects_com.object[i_nt].id, m_hypothesis_tree[i_end]);
						if (idx_com >= 0) {
							m_hypothesis_tree[i_end].assoc_vec[idx_com].id_per = idx_per;
						}
						m_hypothesis_tree[i_end].already_seen_per.push_back(idx_per);
						this->m_max_hyp_id++;
						m_hypothesis_tree[i_end].id = this->m_max_hyp_id;
						i_end++;
					}
					i_inter++;
				}
				i_start_inter = i_end_inter;
				i_end_inter = i_end;
			}
		}

		// Calculate the measurement likeihood for every hypothesis of the tree
		float likelihood_meas;
		for (int h = i_start; h<i_end; h++) {
			// Calculate the product of measurement likelihood of every association
			for (int ass = 0; ass<(int)m_hypothesis_tree[h].assoc_vec.size(); ass++) {
				idx_com = GetIdxObstacle(m_hypothesis_tree[h].assoc_vec[ass].id_com, this->m_objects_com);
				// Distinguish the likelihood for an associated track and a not perceived one
				if (m_hypothesis_tree[h].assoc_vec[ass].id_per>0) {
					idx_per = GetIdxObstacle(m_hypothesis_tree[h].assoc_vec[ass].id_per, this->m_objects_per);
					if (idx_per >= 0 && idx_com >= 0) {
						likelihood_meas = LocationLikelihood(this->m_objects_per.object[idx_per].x_rel, this->m_objects_per.object[idx_per].y_rel, this->m_objects_com.object[idx_com].x_rel, this->m_objects_com.object[idx_com].y_rel,
							this->m_objects_com.object[idx_com].x_sigma, this->m_objects_com.object[idx_com].y_sigma, this->m_objects_com.object[idx_com].xy_sigma);
						likelihood_meas *= this->m_objects_per.object[idx_per].class_probability[this->m_objects_com.object[idx_com].obj_class];
					}
					else {
						likelihood_meas = 0;
					}
					m_hypothesis_tree[h].likelihood_meas *= likelihood_meas;
					m_hypothesis_tree[h].detection_prob *= p_perception_prob;
				}
				else {
					if (this->p_use_laser_points) {
						m_hypothesis_tree[h].likelihood_meas *= (float)m_occluded_area_ratio[idx_com] * p_occlusion_ratio;
					}
					else {
						m_hypothesis_tree[h].likelihood_meas *= p_occlusion_ratio;
					}
				}
				//m_hypothesis_tree[h].detection_prob *= p_communication_prob;
			}
			m_hypothesis_tree[h].detection_prob *= pow(p_communication_prob, m_objects_com.nb_objects);

			// Calculate the branch likelihood given by combinatorial considerations
			// Here, fixed detection probability is used, but it could be modify by a more sophisticated law
			m_hypothesis_tree[h].likelihood_branch_nt = (float)((double)(Factorial(m_hypothesis_tree[h].n_nt_h)) / (double)(Factorial(n_nt_com)));
			m_hypothesis_tree[h].likelihood_branch_dt = 1 / (float)(BinomialCoeff(n_prev_com_np, n_prev_com_np + n_nt_per));
			m_hypothesis_tree[h].likelihood_branch = m_hypothesis_tree[h].likelihood_branch_nt*m_hypothesis_tree[h].likelihood_branch_dt*m_hypothesis_tree[h].detection_prob;

			m_hypothesis_tree[h].n_prev_com_np = n_prev_com_np;
			m_hypothesis_tree[h].n_nt_per = n_nt_per;
			m_hypothesis_tree[h].n_nt_com = n_nt_com;

			m_hypothesis_tree[h].prob *= m_hypothesis_tree[h].likelihood_meas*m_hypothesis_tree[h].likelihood_branch;
			sum_prob += m_hypothesis_tree[h].prob;

		}
	}

	// Normalise each hypothesis probability to sum to 1
	for (int h = 0; h<(int)m_hypothesis_tree.size(); h++) {
		if (sum_prob != 0) {
			m_hypothesis_tree[h].prob /= sum_prob;
		}
		else {
			m_hypothesis_tree[h].prob = 1 / (float)(m_hypothesis_tree.size());
		}
	}
}

void MAPSGating::PruneHypothesis() {
	int h = 0;
	int idx_com;
	float sum_prob(1.0f);
	bool hypothesis_with_per(false);

	std::vector<s_hypothesis>::iterator it_h = this->m_hypothesis_tree.begin();
	// Delete hypothesis with probability below p_hypothesis_pruning threshold
	// Be careful of keeping the hypothesis with all the not perceived branch in memory, this allows to keep generating new hypothesis from new perceived obstacles
	while (h<(int)m_hypothesis_tree.size()) {
		if (m_hypothesis_tree[h].prob >= p_hypothesis_pruning) {
			h++;
		}
		else {
			hypothesis_with_per = false;
			for (int ass = 0; ass<(int)m_hypothesis_tree[h].assoc_vec.size(); ass++) {
				if (m_hypothesis_tree[h].assoc_vec[ass].id_per>0) {
					//idx_per = GetIdxObstacle(m_hypothesis_tree[h].assoc_vec[ass].id_per,this->m_objects_per);
					idx_com = GetIdxObstacle(m_hypothesis_tree[h].assoc_vec[ass].id_com, this->m_objects_com);
					std::vector<int>::iterator it = this->m_ass_per_com_meas[idx_com].begin();
					for (int i_p = 0; i_p<(int)this->m_ass_per_com_meas[idx_com].size(); i_p++) {
						if (this->m_objects_per.object[this->m_ass_per_com_meas[idx_com][i_p]].id == m_hypothesis_tree[h].assoc_vec[ass].id_per) {
							this->m_ass_per_com_meas[idx_com].erase(it + i_p);
							break;
						}
					}
					hypothesis_with_per = true;
				}
				/*else {
				m_hypothesis_tree[h].prob=p_hypothesis_pruning;
				}*/
				//m_hypothesis_tree[h].detection_prob *= p_communication_prob;
			}
			sum_prob += -m_hypothesis_tree[h].prob;
			if (hypothesis_with_per) {
				this->m_hypothesis_tree.erase(it_h + h);
			}
			else {
				m_hypothesis_tree[h].prob = p_hypothesis_pruning;
				sum_prob += p_hypothesis_pruning;
			}
		}
	}

	// Normalise each hypothesis probability to sum to 1
	for (int h = 0; h<(int)m_hypothesis_tree.size(); h++) {
		if (sum_prob != 0) {
			m_hypothesis_tree[h].prob /= sum_prob;
		}
		else {
			m_hypothesis_tree[h].prob = 1 / (float)(m_hypothesis_tree.size());
		}
	}
}

void MAPSGating::FusedTracksEstimation() {
	int id_best_hyp(0);
	float prob_best_hyp(0);
	std::vector<int> associated_perception_tracks;
	int idx_com, idx_per;

	associated_perception_tracks.clear();

	m_objects_ass.nb_objects = 0;
	m_objects_np.nb_objects = 0;
	m_objects_nc.nb_objects = 0;

	if (!this->m_hypothesis_tree.empty()) {
		// Pick the best hypothesis
		for (int h = 0; h<(int)this->m_hypothesis_tree.size(); h++) {
			if (m_hypothesis_tree[h].prob>prob_best_hyp) {
				prob_best_hyp = m_hypothesis_tree[h].prob;
				id_best_hyp = h;
			}
		}

		// Estimate association and not perceived obstacles
		for (int ass = 0; ass<(int)this->m_hypothesis_tree[id_best_hyp].assoc_vec.size(); ass++) {
			if (m_hypothesis_tree[id_best_hyp].assoc_vec[ass].id_per>0) {
				idx_per = GetIdxObstacle(m_hypothesis_tree[id_best_hyp].assoc_vec[ass].id_per, this->m_objects_per);
				idx_com = GetIdxObstacle(m_hypothesis_tree[id_best_hyp].assoc_vec[ass].id_com, this->m_objects_com);
				if (idx_com >= 0 && idx_per >= 0 && m_objects_ass.nb_objects<MAXIMUM_OBJECT_NUMBER) {
					UtilsObject::PrintObject(m_objects_ass.object[m_objects_ass.nb_objects], m_objects_per.object[idx_per]);
					associated_perception_tracks.push_back(idx_per);

					//m_objects_ass.object[m_objects_ass.nb_objects].id = m_objects_com.object[idx_com].id;
					m_objects_ass.object[m_objects_ass.nb_objects].obj_class = m_objects_com.object[idx_com].obj_class;
					m_objects_ass.object[m_objects_ass.nb_objects].color_b = p_color_b_ass;
					m_objects_ass.object[m_objects_ass.nb_objects].color_g = p_color_g_ass;
					m_objects_ass.object[m_objects_ass.nb_objects].color_r = p_color_r_ass;
					m_objects_ass.nb_objects++;
				}
			}
			else {
				idx_com = GetIdxObstacle(m_hypothesis_tree[id_best_hyp].assoc_vec[ass].id_com, this->m_objects_com);
				if (idx_com >= 0 && m_objects_np.nb_objects<MAXIMUM_OBJECT_NUMBER) {
					UtilsObject::PrintObject(m_objects_np.object[m_objects_np.nb_objects], m_objects_com.object[idx_com]);

					m_objects_np.object[m_objects_np.nb_objects].color_b = p_color_b_np;
					m_objects_np.object[m_objects_np.nb_objects].color_g = p_color_g_np;
					m_objects_np.object[m_objects_np.nb_objects].color_r = p_color_r_np;
					m_objects_np.nb_objects++;
				}
			}
		}
	}

	// Estimate not communication obstacles
	for (int i_p = 0; i_p<m_objects_per.nb_objects; i_p++) {
		if (!IsAlreadyHere(i_p, associated_perception_tracks) && m_objects_nc.nb_objects<MAXIMUM_OBJECT_NUMBER) {
			UtilsObject::PrintObject(m_objects_nc.object[m_objects_nc.nb_objects], m_objects_per.object[i_p]);
			m_objects_nc.nb_objects++;
		}
	}
}

void MAPSGating::OcclusionAreaRatio() {

	double delta, lambda1, lambda2, ellipse_angle;
	double theta_min, theta_max, theta_obj;
	double theta;
	int idx_theta;

	int nb_used_pts;
	double sum_weight;

	float dist_mahalanobis;
	float x, y, sigma_x, sigma_y, sigma_xy, c;

	double area_occ;

	double dist_laser, dist_obj;

	float max_dist = sqrt(this->p_gating);

	std::vector<bool> m_seen_angle;

	// Calculate occlusion area ratio for every communicating object
	for (int i_c = 0; i_c<this->m_objects_com.nb_objects; i_c++) {

		m_seen_angle.clear();
		area_occ = 0;
		nb_used_pts = 0;
		sum_weight = 0;

		sigma_x = this->m_objects_com.object[i_c].x_sigma;
		sigma_y = this->m_objects_com.object[i_c].y_sigma;
		sigma_xy = this->m_objects_com.object[i_c].xy_sigma;
		c = sigma_x*sigma_y - pow(sigma_xy, 2);

		delta = pow(sigma_x - sigma_y, 2) + 4 * pow(sigma_xy, 2);
		lambda1 = ((double)(sigma_x + sigma_y) + sqrt(delta))*this->p_gating / 2;
		lambda2 = ((double)(sigma_x + sigma_y) - sqrt(delta))*this->p_gating / 2;

		ellipse_angle = atan2((double)(this->p_gating*sigma_x - lambda1), (double)(sigma_xy));

		// Calculate the limits of covariance ellipse of the communcation object
		// To Do check the role of lambda1 and lambda2
		double a1 = atan2((double)m_objects_com.object[i_c].y_rel - lambda2, (double)m_objects_com.object[i_c].x_rel);
		double a2 = atan2((double)m_objects_com.object[i_c].y_rel + lambda2, (double)m_objects_com.object[i_c].x_rel);
		theta_min = min((double)a1, (double)a2);
		theta_max = max((double)a1, (double)a2);
		theta_obj = (double)atan2(m_objects_com.object[i_c].y_rel, m_objects_com.object[i_c].x_rel);

		m_seen_angle.resize((int)floor((theta_max - theta_min) / (p_laser_angular_resolution*PI / 180)));

		for (int i = 0; i<(int)m_seen_angle.size(); i++) {
			m_seen_angle[i] = false;
		}

		// Calculate the occlusion weight of every laser point
		for (int j = 0; j<this->m_laser_points.nb_points; j++) {
			// If the laser point is in direction of the covariance ellipse, then it has to be considered
			if (this->m_laser_points.point[j].theta>theta_min&&this->m_laser_points.point[j].theta<theta_max && !m_seen_angle.empty()) {

				dist_laser = pow(this->m_laser_points.point[j].x, 2) + pow(this->m_laser_points.point[j].y, 2);
				dist_obj = min(pow(this->m_objects_com.object[i_c].x_rel, 2) + pow(this->m_objects_com.object[i_c].y_rel, 2), V2PFUSION_MAXIMUM_DISTANCE);
				x = this->m_objects_com.object[i_c].x_rel - this->m_laser_points.point[j].x;
				y = this->m_objects_com.object[i_c].y_rel - this->m_laser_points.point[j].y;
				dist_mahalanobis = sqrt((x*(x*sigma_y - y*sigma_xy) + y*(y*sigma_x - x*sigma_xy)) / c);

				idx_theta = min((int)floor((this->m_laser_points.point[j].theta - theta_min) / (p_laser_angular_resolution*PI / 180)), (int)m_seen_angle.size() - 1);
				idx_theta = max(idx_theta, 0);
				m_seen_angle[idx_theta] = true;

				// Consider the point different if it is in front of the communication object or behind this object
				if ((double)dist_laser<(double)dist_obj) {
					nb_used_pts++;
					// weight the laser impact using its gap with the communication object
					// angular gap
					double n_occ = min(abs((this->m_laser_points.point[j].theta - theta_min) / (theta_obj - theta_min)), abs((this->m_laser_points.point[j].theta - theta_max) / (theta_obj - theta_max)));
					sum_weight += n_occ;
					// distance gap, add 1/2 when the obstacle is in front of the object (at least one half is hidden)
					if (dist_mahalanobis<max_dist) {
						n_occ *= 1 / 2 + (dist_mahalanobis / max_dist) / 2;
					}
					area_occ += n_occ;
				}
				else {
					// weight the laser impact using its gap with the communication object
					// angular gap
					double n_occ = min(abs((this->m_laser_points.point[j].theta - theta_min) / (theta_obj - theta_min)), abs((this->m_laser_points.point[j].theta - theta_max) / (theta_obj - theta_max)));
					sum_weight += n_occ;
					// distance gap (0 when the laser impact is behind the covariance ellipse)
					if (dist_mahalanobis<max_dist) {
						n_occ *= ((max_dist - dist_mahalanobis) / max_dist) / 2;
					}
					else {
						n_occ = 0;
					}
					nb_used_pts++;
					area_occ += n_occ;
				}
			}
		}

		// Count the theoritical number of impacts that should be present
		for (int i = 0; i<(int)m_seen_angle.size(); i++) {
			if (!m_seen_angle[i]) {
				theta = theta_min + i*p_laser_angular_resolution*PI / 180;
				double n_occ = min(abs((theta - theta_min) / (theta_obj - theta_min)), abs((theta - theta_max) / (theta_obj - theta_max)));
				sum_weight += n_occ;
			}
		}

		// Update the occluded area ratio
		if (sum_weight != 0) {
			m_occluded_area_ratio[i_c] = area_occ / sum_weight;
		}
		else {
			m_occluded_area_ratio[i_c] = 0;
		}
		//m_occluded_area_ratio[i_c] = area_occ/nb_used_pts;
	}
}

void MAPSGating::UpdateGateTracks() {

	int id_per;
	for (int i = 0; i<this->m_objects_com.nb_objects; i++) {
		// Clear previous perception obstacles in gate
		m_prev_gate[this->m_objects_com.object[i].id - 1].clear();
		// Add current perception obstacles in gate
		for (int i_p = 0; i_p<(int)this->m_ass_per_com_meas[i].size(); i_p++) {
			id_per = this->m_objects_per.object[this->m_ass_per_com_meas[i][i_p]].id;
			m_prev_gate[this->m_objects_com.object[i].id - 1].push_back(id_per);
		}
	}

}

bool MAPSGating::IsAlreadyHere(int id, std::vector<int> & already_seen) {
	for (size_t i = 0; i<already_seen.size(); i++)
	{
		if (already_seen[i] == id)
			return true;
	}
	return false;
}

int MAPSGating::GetIdPer(int id_com, s_hypothesis hyp) {
	for (int i = 0; i<(int)hyp.assoc_vec.size(); i++) {
		if (hyp.assoc_vec[i].id_com == id_com) {
			return hyp.assoc_vec[i].id_per;
		}
	}
	return -1;
}

int MAPSGating::GetIdxCom(int id_com, s_hypothesis hyp) {
	for (int i = 0; i<(int)hyp.assoc_vec.size(); i++) {
		if (hyp.assoc_vec[i].id_com == id_com) {
			return i;
		}
	}
	return -1;
}

int MAPSGating::GetIdxObstacle(int id, objects objs) {
	for (int i = 0; i<objs.nb_objects; i++) {
		if (objs.object[i].id == id) {
			return i;
		}
	}
	return -1;
}

bool MAPSGating::IsInGate(int id_per, int i_com) {

	for (int i = 0; i<(int)this->m_ass_per_com_meas[i_com].size(); i++) {
		if (this->m_objects_per.object[this->m_ass_per_com_meas[i_com][i]].id == id_per) {
			return true;
		}
	}

	return false;
}

float MAPSGating::LocationLikelihood(float x_per, float y_per, float x_com, float y_com, float x_sigma, float y_sigma, float xy_sigma) {
	// Calculate the gaussian distrobution along x-y coordinates
	float c = x_sigma*y_sigma - pow(xy_sigma, 2);
	float x = x_per - x_com;
	float y = y_per - y_com;

	return (float)(exp(-(x*(x*y_sigma - y*xy_sigma) + y*(y*x_sigma - x*xy_sigma)) / (2 * c)) / (2 * PI*sqrt(c)));
}

void MAPSGating::SaveROIs(const objects & objs, std::ofstream & save_file, double system_time) {

	save_file << system_time << ";";
	save_file << objs.nb_objects;
	for (int i = 0; i<objs.nb_objects; i++) {
		save_file << ";";
		save_file << objs.object[i].id << ";";
		save_file << objs.object[i].x_rel << ";";
		save_file << objs.object[i].y_rel << ";";
		save_file << objs.object[i].vx_rel << ";";
		save_file << objs.object[i].vy_rel << ";";
		save_file << objs.object[i].x_abs << ";";
		save_file << objs.object[i].y_abs << ";";
		save_file << objs.object[i].vx_abs << ";";
		save_file << objs.object[i].vy_abs;

		for (int c = 0; c<NB_CLASSES; c++) {
			save_file << ";";
			save_file << objs.object[i].class_probability[c];
		}
	}
	save_file << "\n";
}

void MAPSGating::SaveHypothesis(const std::vector<s_hypothesis> & hyp, std::ofstream & save_file, double system_time) {

	save_file << system_time << ";";
	save_file << (int)hyp.size();
	for (int i = 0; i<(int)hyp.size(); i++) {
		save_file << ";";
		save_file << hyp[i].id << ";";
		save_file << hyp[i].likelihood_meas << ";";
		save_file << hyp[i].likelihood_branch << ";";
		save_file << hyp[i].prob << ";";
		save_file << hyp[i].likelihood_branch_nt << ";";
		save_file << hyp[i].likelihood_branch_dt << ";";
		save_file << hyp[i].detection_prob << ";";
		save_file << (int)hyp[i].assoc_vec.size();
		for (int h = 0; h<(int)hyp[i].assoc_vec.size(); h++) {
			save_file << ";";
			save_file << hyp[i].assoc_vec[h].id_com << ";" << hyp[i].assoc_vec[h].id_per;
		}
	}
	save_file << "\n";

}

void MAPSGating::SaveCommunicationTracks(std::ofstream & save_file, double system_time) {

	save_file << system_time << ";";
	save_file << m_objects_com.nb_objects;
	for (int i = 0; i<m_objects_com.nb_objects; i++) {
		save_file << ";";
		save_file << m_objects_com.object[i].id << ";";
		save_file << m_objects_com.object[i].x_rel << ";";
		save_file << m_objects_com.object[i].y_rel << ";";
		save_file << m_objects_com.object[i].vx_rel << ";";
		save_file << m_objects_com.object[i].vy_rel << ";";
		save_file << m_objects_com.object[i].x_abs << ";";
		save_file << m_objects_com.object[i].y_abs << ";";
		save_file << m_objects_com.object[i].vx_abs << ";";
		save_file << m_objects_com.object[i].vy_abs << ";";
		save_file << m_objects_com.object[i].obj_class << ";";
		save_file << m_occluded_area_ratio[i];
	}
	save_file << "\n";
}

void MAPSGating::SavePerceptionTracks(std::ofstream & save_file, double system_time) {

	int idx_per;
	for (int g = 0; g<(int)this->m_ass_per_com_meas.size(); g++) {
		save_file << system_time << ";";
		save_file << g << ";";
		save_file << (int)this->m_ass_per_com_meas[g].size();
		for (int i = 0; i<(int)this->m_ass_per_com_meas[g].size(); i++) {
			idx_per = this->m_ass_per_com_meas[g][i];
			save_file << ";";
			save_file << m_objects_per.object[idx_per].id << ";";
			save_file << m_objects_per.object[idx_per].x_rel << ";";
			save_file << m_objects_per.object[idx_per].y_rel << ";";
			save_file << m_objects_per.object[idx_per].vx_rel << ";";
			save_file << m_objects_per.object[idx_per].vy_rel << ";";
			save_file << m_objects_per.object[idx_per].x_abs << ";";
			save_file << m_objects_per.object[idx_per].y_abs << ";";
			save_file << m_objects_per.object[idx_per].vx_abs << ";";
			save_file << m_objects_per.object[idx_per].vy_abs;

			for (int c = 0; c<NB_CLASSES; c++) {
				save_file << ";";
				save_file << m_objects_per.object[idx_per].class_probability[c];
			}
		}
		save_file << "\n";
	}
}