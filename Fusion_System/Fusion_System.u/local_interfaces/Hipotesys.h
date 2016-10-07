#pragma once
#include <vector>
typedef struct
{
	int id_per; /*!<perception obstacle id*/
	int id_com; /*!<communication obstacle id*/
} s_ass;

typedef struct
{
	int id; /*!<hypothesis id*/
	std::vector<s_ass> assoc_vec; /*!<vector containing perception-communication associations under the hypothesis*/
	float likelihood_meas; /*!<hypothesis measurement likelihood*/
	float likelihood_branch; /*!<hypothesis branch likelihood*/
	float prob; /*!<hypothesis posterior probability*/
	std::vector<int> already_seen_per; /*!<already seen perception tracks in the hypothesis*/
	int n_nt_h; /*!<number of new tracks in the hypothesis*/
	int n_prev_com_np; /*!<previous number of not perceived communicaion tracks in the hypothesis*/
	int n_nt_per; /*!<number of new perception tracks in the hypothesis*/
	int n_nt_com; /*!<number of new communication tracks in the hypothesis*/
	float likelihood_branch_nt; /*!<branch likelihood of new tracks*/
	float likelihood_branch_dt; /*!<branch likelihood of already existing tracks*/
	float detection_prob; /*!<detection probability in the tracks in the hypothesis*/
} s_hypothesis;