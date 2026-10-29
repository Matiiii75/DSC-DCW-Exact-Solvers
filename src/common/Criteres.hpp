#pragma once 

#include <memory>
#include "Data.hpp"

struct Criteres 
{
    virtual ~Criteres() = default; // destructor 

    /**
     * @brief Computes the weight of a candidate set based on a predecessor 
     * candidate set and the added candidate element 
     * @param pred_cand_set_weight the weight of a predecessor 
     * @param cand_c the added candidate
     * @param cut_set necessary for the DSC update (but not for CW)
     */
    virtual int compute_weight(
        int pred_cand_set_weight, 
        int cand_c, 
        const std::vector<uint8_t>& cut_set
    ) const = 0; 

    /**
     * @brief computes the optimal distance from a predecessor 
     * @param best_dist_pred the distance label of the predecessor
     * @param weight_pred the weight of the predecessor in SG 
     */
    virtual int compute_dist_from_pred(
        int best_dist_pred, 
        int weight_pred
    ) const = 0;  

    /**
     * @brief Computes the objective value given a topological order 
     * @param ordre_topo topological order being evaluated 
     */
    virtual int compute_objective_value(
        const std::vector<int>& ordre_topo
    ) const = 0; 

    /**
     * @brief Checker that evaluates the validity of an order and its associated value 
     * @param ordre_topo topological order to evaluate 
     * @param value_found the found objective value 
     */
    virtual bool checker(
        const std::vector<int>& ordre_topo, 
        int value_found
    ) const = 0; 

    // method to identify the criterion 
    // returns "DSC" or "CW"
    virtual std::string get_name() const = 0; 

    virtual std::unique_ptr<Criteres> clone(const Data& data) const = 0; 

    /**
     * @brief Updates the global objective value during preprocessing
     * @param global_value The global objective value being calculated (passed by reference)
     * @param sub_optimal_value The optimal value returned by solving a sub-instance
     */
    virtual void aggrege_sub_solution(int& global_value, int sub_optimal_value) const = 0; 

    /**
     * @brief Updates the global value with trivial components (size 2)
     * @param global_value The global objective value being calculated (passed by reference)
     * @param nb_two_nodes_comp The number of 2-node components
     */
    virtual void aggrege_trivial_components(int& global_value, int nb_two_nodes_comp) const = 0;
};