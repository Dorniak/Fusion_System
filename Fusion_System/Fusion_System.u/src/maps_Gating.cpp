////////////////////////////////
// RTMaps SDK Component
////////////////////////////////

////////////////////////////////
// Purpose of this module :
////////////////////////////////

#include "maps_Gating.h"	// Includes the header of this component

const MAPSTypeFilterBase ValeoStructure = MAPS_FILTER_USER_STRUCTURE(AUTO_Objects);
const MAPSTypeFilterBase matchedVector = MAPS_FILTER_USER_STRUCTURE(std::vector<std::vector<int>>);

// Use the macros to declare the inputs
MAPS_BEGIN_INPUTS_DEFINITION(MAPSGating)
    //MAPS_INPUT("iName",MAPS::FilterInteger32,MAPS::FifoReader)
	MAPS_INPUT("LaserObject", ValeoStructure, MAPS::FifoReader)
	MAPS_INPUT("CameraObject", ValeoStructure, MAPS::FifoReader)
	MAPS_INPUT("MatchedLaser", matchedVector, MAPS::FifoReader)
	MAPS_INPUT("MatchedCamera", matchedVector, MAPS::FifoReader)
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

//TODO::Comprobado OK
void MAPSGating::Birth()
{
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




    // Reports this information to the RTMaps console. You can remove this line if you know when Birth() is called in the component lifecycle.
    ReportInfo("Passing through Birth() method");
}

void MAPSGating::Core()
{
	readInputs();
	adaptation();
	ProcessData();
	writeOutputs();
}

void MAPSGating::Death()
{
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

void MAPSGating::readInputs()
{
	while (!DataAvailableInFIFO(Input("LaserObject")) || !DataAvailableInFIFO(Input("CameraObject"))) {}
#pragma region Lectura
	//Leer objetos del laser
	elt = StartReading(Input("LaserObject"));
	m_objects_per2 = static_cast<AUTO_Objects*>(elt->Data());
	StopReading(Input("LaserObject"));
	m_objects_per = *m_objects_per2;

	//Leer objetos de la camara
	elt = StartReading(Input("CameraObject"));
	m_objects_com2 = static_cast<AUTO_Objects*>(elt->Data());
	StopReading(Input("CameraObject"));
	m_objects_com = *m_objects_com2;

	//Leer vector de objetos de camara vistos en la gating window de cada objeto laser
	elt = StartReading(Input("MatchedLaser"));
	input_Laser_Matched = static_cast<MATCH_OBJECTS*>(elt->Data());
	StopReading(Input("MatchedLaser"));
	Laser_Matched = *input_Laser_Matched;

	//Leer vector de objetos de camara vistos en la gating window de cada objeto laser
	elt = StartReading(Input("MatchedCamera"));
	input_Camera_Matched = static_cast<MATCH_OBJECTS*>(elt->Data());
	StopReading(Input("MatchedCamera"));
	Camera_Matched = *input_Camera_Matched;
#pragma endregion
}

void MAPSGating::adaptation()
{
	for (size_t i = 0; i<m_ass_per_com_meas.size(); i++) {
		m_ass_per_com_meas[i].clear();
	}
	m_idx_gate.clear();
	m_ass_per_com_meas.resize(m_objects_com.number_of_objects);
	for (size_t i = 0; i < Laser_Matched.number_objects; i++)
	{
		for (size_t j = 0; j < Laser_Matched.number_matched[i]; j++)
		{
			m_ass_per_com_meas[i].push_back(j);
			if (!IsAlreadyHere(j, m_idx_gate)) {
				m_idx_gate.push_back(j);
			}
		}
		if (m_objects_com.object[i].id>m_max_com_id) {
			m_max_com_id = m_objects_com.object[i].id;
			m_prev_gate.resize(m_max_com_id);
		}
	}
}

void MAPSGating::writeOutputs()
{
}

//ProcessData
void MAPSGating::ProcessData() {

	this->m_objects_ass.number_of_objects = 0;
	this->m_objects_np.number_of_objects = 0;
	this->m_objects_nc.number_of_objects = 0;

	// Initialize hypothesis tree
	IntializeTree();	//TODO::Comprobado OK

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

}

//TODO::Comprobado OK
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
		h.likelihood_branch_nt = 1;
		h.likelihood_branch_dt = 1;
		m_prev_hypothesis.push_back(h);
	}
}

/*
void MAPSGating::Gating() {

	float dist_mahalanobis;
	float x, y, sigma_x, sigma_y, sigma_xy, c;
	for (int i = 0; i<(int)m_ass_per_com_meas.size(); i++) {
		m_ass_per_com_meas[i].clear();
	}
	m_idx_gate.clear();
	//m_ass_per_com_meas.clear();
	m_ass_per_com_meas.resize(m_objects_com.number_of_objects);

	// calculate gating window for every communication obstacle
	for (int i = 0; i<this->m_objects_com.number_of_objects; i++) {
		sigma_x = this->m_objects_com.object[i].x_sigma;
		sigma_y = this->m_objects_com.object[i].y_sigma;
		sigma_xy = this->m_objects_com.object[i].xy_sigma;
		c = sigma_x*sigma_y - pow(sigma_xy, 2);

		for (int j = 0; j<this->m_objects_per.number_of_objects; j++) {
			x = this->m_objects_com.object[i].x_rel - this->m_objects_per.object[j].x_rel;
			y = this->m_objects_com.object[i].y_rel - this->m_objects_per.object[j].y_rel;
			dist_mahalanobis = (x*(x*sigma_y - y*sigma_xy) + y*(y*sigma_x - x*sigma_xy)) / c;
			if (dist_mahalanobis<this->p_gating) {
				this->m_ass_per_com_meas[i].push_back(j);

				if (!IsAlreadyHere(j, m_idx_gate)) {
					m_idx_gate.push_back(j);
				}
			}
		}

		if (this->m_objects_com.object[i].id>m_max_com_id) {
			m_max_com_id = this->m_objects_com.object[i].id;
			m_prev_gate.resize(m_max_com_id);
		}
	}
}
*/

void MAPSGating::UpdateTree() {
#pragma region Inicializacion
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
#pragma endregion
#pragma region Predetected
	// Set perception tracks which have already been detected, i.e., DT tracks
	for (int i = 0; i<this->m_objects_com.number_of_objects; i++) {
		for (int i_p = 0; i_p<(int)this->m_ass_per_com_meas[i].size(); i_p++) {
			idx_per = this->m_objects_per.object[this->m_ass_per_com_meas[i][i_p]].id;
			if (!IsAlreadyHere(idx_per, m_already_seen_per) &&	!IsAlreadyHere(idx_per, m_prev_gate[this->m_objects_com.object[i].id - 1])) {
				n_nt_per++;
				m_already_seen_per.push_back(idx_per);
				//m_prev_gate[this->m_objects_com.object[i].id-1].push_back(idx_per);
			}
		}
	}
#pragma endregion
#pragma region Hypotesys_generator
	// Generate the new hypothesis tree from the previous hypothesis tree 
	for (int i = 0; i<(int)m_prev_hypothesis.size(); i++) {
		int n_prev_com_ass(0), n_prev_com_np(0), n_nt_com(0);
		int id_per;
		int i_start, i_end, i_start_inter, i_end_inter;
		bool update_hypothesis(false);
		std::vector<int> i_prev_np;
		std::vector<int> i_nt_com;

		// Counting the number of associations and not perceived tracks at previous instant and the number of new communication tracks at current instant
		for (int id_c = 0; id_c<m_objects_com.number_of_objects; id_c++) {
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
		//TODO::Comprobado OK (Añadir entradas del gatting ya calculadas)

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
#pragma endregion
		//TODO::Comprobado OK

		//TODO::Modificar calculo de probabilidades
#pragma region Probabilidad
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
						this->m_objects_com.object[idx_com].x_sigma, this->m_objects_com.object[idx_com].y_sigma,0);// this->m_objects_com.object[idx_com].xy_sigma
						likelihood_meas *= this->m_objects_per.object[idx_per].class_confidence;
						
					}
					else {
						likelihood_meas = 0;
					}
					m_hypothesis_tree[h].likelihood_meas *= likelihood_meas;
					m_hypothesis_tree[h].detection_prob *= p_perception_prob;
				}
				else {
						m_hypothesis_tree[h].likelihood_meas *= p_occlusion_ratio;
				}
			}
			m_hypothesis_tree[h].detection_prob *= pow(p_communication_prob, m_objects_com.number_of_objects);

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
#pragma endregion
	}

	//TODO::Modificar hasta aqui
#pragma region Normalise
	// Normalise each hypothesis probability to sum to 1
	for (int h = 0; h<(int)m_hypothesis_tree.size(); h++) {
		if (sum_prob != 0) {
			m_hypothesis_tree[h].prob /= sum_prob;
		}
		else {
			m_hypothesis_tree[h].prob = 1 / (float)(m_hypothesis_tree.size());
		}
	}
#pragma endregion
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

	m_objects_ass.number_of_objects = 0;
	m_objects_np.number_of_objects = 0;
	m_objects_nc.number_of_objects = 0;

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
				if (idx_com >= 0 && idx_per >= 0 && m_objects_ass.number_of_objects<MAXIMUM_OBJECT_NUMBER) {
					m_objects_ass.object[m_objects_ass.number_of_objects] = m_objects_per.object[idx_per];
					associated_perception_tracks.push_back(idx_per);
					m_objects_ass.object[m_objects_ass.number_of_objects].object_class = m_objects_com.object[idx_com].object_class;
					m_objects_ass.number_of_objects++;
				}
			}
			else {
				idx_com = GetIdxObstacle(m_hypothesis_tree[id_best_hyp].assoc_vec[ass].id_com, this->m_objects_com);
				if (idx_com >= 0 && m_objects_np.number_of_objects<MAXIMUM_OBJECT_NUMBER) {
					m_objects_np.object[m_objects_np.number_of_objects] = m_objects_com.object[idx_com];		
					m_objects_np.number_of_objects++;
				}
			}
		}
	}

	// Estimate not communication obstacles
	for (int i_p = 0; i_p<m_objects_per.number_of_objects; i_p++) {
		if (!IsAlreadyHere(i_p, associated_perception_tracks) && m_objects_nc.number_of_objects<MAXIMUM_OBJECT_NUMBER) {
			m_objects_nc.object[m_objects_nc.number_of_objects] = m_objects_per.object[i_p];
			m_objects_nc.number_of_objects++;
		}
	}
}

void MAPSGating::UpdateGateTracks() {

	int id_per;
	for (int i = 0; i<this->m_objects_com.number_of_objects; i++) {
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

int MAPSGating::GetIdxObstacle(int id, AUTO_Objects objs) {
	for (int i = 0; i<objs.number_of_objects; i++) {
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