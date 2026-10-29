#pragma once 

#include "Data.hpp"
#include "Criteres.hpp"
#include "Common.hpp"
#include "State_graph.hpp"
#include "Timer.hpp"

struct FPT_solver 
{
    const Data& data; 
    State_graph& SG; 
    const Criteres& critere; 

    std::queue<int> L;                                  // fifo list for the main algorithm
    std::vector<int> best_dist;                         // best_dist[ID] = shortest distance from source to ID in SG 
    std::vector<std::pair<int,int>> pred_in_pcc;        // pred_in_pcc[ID] = {ID_pred, added candidate}

    double time_limit;  
    Timer timer; 

    bool found_solution; 
    double total_time;
    int optimal_value; 

    FPT_solver(
        const Data& _data, 
        State_graph& _SG, 
        const Criteres& _critere, 
        double time_limit
    ); 

    std::vector<int> rebuild_opt_order() const; 

    /**
     * @brief Computes the cutset associated with a Candidate set 
     * @param cand considered candidate set 
     * @param cut_set_size passed by reference, to retrieve the size of the computed cutset
     * @param cut_set passed by reference, to retrieve the computed cutset
     * @param hors_cut_set passed by reference, to retrieve V-S
     */
    void compute_cut_set(
        const std::vector<int>& cand, 
        int& cut_set_size, 
        std::vector<uint8_t>& cut_set, 
        std::vector<int>& hors_cut_set
    ) const; 

    /**
     * @brief Checks if the time limit has been exceeded 
     * @param iteration_count iteration counter (we only evaluate if it's a multiple of 10000)
     * @return true if the limit has not been reached, false otherwise 
     */
    bool try_eval_time_limit(int iteration_count); 
    
    /**
     * @brief Updates the distance label of a candidate set
     */
    inline void update_distance_label(int C1_ID, int C2_ID, int curr_c) 
    {
        int best_dist_C1 = best_dist[C1_ID]; 
        int weight_C1 = SG.weights[C1_ID]; 
        int dist_from_C1 = critere.compute_dist_from_pred(best_dist_C1, weight_C1); 
        if(best_dist[C2_ID] > dist_from_C1) {
            best_dist[C2_ID] = dist_from_C1;
            pred_in_pcc[C2_ID] = {C1_ID, curr_c}; 
        }
    } 

    /**
     * @brief Solver for the FPT algorithm. It builds the state graph. 
     */
    void solve(); 

    /**
     * @brief extracts the data obtained after resolution. 
     */
    SolverResults get_results() const; 
};