#include "Common.hpp"

bool is_disjoint(const std::vector<int>& succ_gamma, int t, 
    const std::vector<uint8_t>& cut_set) 
{
    for(int u : succ_gamma) {
        if(u == t) continue; // sink -> ignore 
        if(cut_set[u] == 0) // if it is not in the cut_set
            return true; 
    }
    
    return false; 
}

bool is_included(const std::vector<int>& v1, const std::vector<uint8_t>& v2, int t) {

    int v1_size = (int)v1.size(); 

    if(v1_size > 0 && v1.back() == t) // ignore the sink 
        --v1_size; 

    for(int i = 0; i < v1_size; ++i) { // for each index of v1
        int v_i = v1[i]; 
        if(v2[v_i] == 0) return false; // if it is not in v2
    }

    return true; // if we reach this point, all elements of v1 are in v2
}