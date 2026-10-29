#pragma once 

#include "Data.hpp"
#include "Common.hpp"

struct PreTraitement 
{   
    const Data& data;                                       // original data
    std::vector<int> one_node_comp; 
    std::vector<std::pair<int,int>> two_nodes_comp;
    std::vector<std::vector<int>> all_map_new_to_old; 
    std::vector<Data> sub_instances; 

    PreTraitement(const Data& _data): data(_data) {}
    
    // computes all connected components, and stores their sizes
    std::vector<std::vector<int>> compute_composantes();
    
    // computes the induced subgraph (re-indexed) by a component
    Dag compute_induced_dag(const std::vector<int>& composante);  
    
    // computes the sub-instances induced by weakly connected components 
    void compute_sub_instances();

    std::vector<int> re_label_sub_solution(
        const std::vector<int>& sub_sol,
        int instance_idx
    ) const; 

    SolverResults store_results(
        int optimal_value, 
        int total_cands,
        double total_time,
        const std::vector<int>& optimal_order
    ) const; 

    int count_composantes() const; 
};