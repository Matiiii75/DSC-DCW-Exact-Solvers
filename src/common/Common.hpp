#pragma once 

#include "Data.hpp"

struct SolverResults
{
    std::string instance_name; 
    int dag_size; 
    int degeneracy; 
    bool found_solution = 0;
    double total_time = -1.0; 
    int optimal_value = -1; 
    int nb_cands = -1; 
    std::vector<int> optimal_order;
    double gap = -1.0;
    double best_bound = -1.0; 
    int nb_sub_pb_solved = -1; // number of components of size > 2 detected in the instance 
    int nb_total_composantes = -1; 
    SolverResults() {}
}; 

/**
 * @brief function that tests the inclusion of v1 in v2 - {t} (since t is not counted in the objective)
 * @param v1 the vector that is included 
 * @param v2 uint8_t vector where v2[u] = 1 if u \in cut-set, 0 otherwise  
 * @param t the sink node that is not considered in the calculation
 * @return true if included, false otherwise
 * @note t will be set to -1 by default to indicate that we are just checking an inclusion without trying to exclude the sink t 
 */
bool is_included(const std::vector<int>& v1, const std::vector<uint8_t>& v2, int t); 

/**
 * @brief determines if there exists a successor of gamma that is not in the cut set
 * (l.8,9 - algo 2)
 * @param succ_gamma vector containing the successors of gamma 
 * @param t the sink node of the dag (precisely not to be considered in the calculations)
 * @param cut_set uint8_t vector where cut_set[u] = 1 if u \in cut-set, 0 otherwise
 * @return true if a successor exists, false otherwise 
 * @note version where the cut_set is an unordered_set
 */
bool is_disjoint(const std::vector<int>& succ_gamma, int t, 
    const std::vector<uint8_t>& cut_set);