#include "DagSumCut.hpp"
#include "Common.hpp"

int DagSumCut::compute_weight(int pred_cand_set_weight, int cand_c, const std::vector<uint8_t>& cut_set) const 
{
    int C_weight = pred_cand_set_weight; 

    for(int u : data.reverse_dag[cand_c]) {
        if(u == data.s) continue; 
        if(is_included(data.dag[u], cut_set, data.t))
            --C_weight; 
    }

    if(is_disjoint(data.dag[cand_c], data.t, cut_set))
        ++C_weight; 

    return C_weight; 
}

int DagSumCut::compute_dist_from_pred(int best_dist_pred, int weight_pred) const 
{
    if(best_dist_pred == std::numeric_limits<int>::max())
        throw std::runtime_error("DagSumCut::compute_dist_from_pred -> distance infinie"); 
    return best_dist_pred + weight_pred; 
}

int DagSumCut::compute_objective_value(const std::vector<int>& ordre_topo) const  
{
    int obj_val = 0; 
    for(int i = 0; i < (int)ordre_topo.size()-1; ++i) { // for each position 
        int valeur_ordre_i = 0; 
        for(int j = 0; j <= i; ++j) { // for each position before i (in cut)
            bool go_next_int_coupe = false; 
            int node_in_coupe = ordre_topo[j]; // retrieve the associated vertex 
            if(node_in_coupe == data.s) continue; // ignore s 
            for(int k = i+1; k < (int)ordre_topo.size(); ++k) { // for each position after i (outside cut)
                int node_hors_coupe = ordre_topo[k]; // retrieve the associated node 
                if(node_hors_coupe == data.t) continue; 
                for(int neigh : data.dag[node_in_coupe]) {
                    if(neigh == node_hors_coupe) {
                        ++valeur_ordre_i; // found a successor outside the cut 
                        go_next_int_coupe = true; 
                        break; // go next node in cut 
                    }
                }
                if(go_next_int_coupe) break; 
            }
        }
        obj_val += valeur_ordre_i; 
    }
    return obj_val; 
}

bool DagSumCut::checker(const std::vector<int>& ordre_topo, int value_found) const 
{
    // start by checking that it is a valid topological order
    // i.e., check that for every pair (u,v) following the topological order, 
    // (v,u) notin A 

    for(int i = 0; i < (int)ordre_topo.size()-1; ++i) {
        for(int j = i+1; j < (int)ordre_topo.size(); ++j) {

            int node1 = ordre_topo[i]; // 1st in the order
            int node2 = ordre_topo[j]; // 2nd

            for(int neigh : data.dag[node2])
                if(neigh == node1) {
                    std::cout << node1 << "est un successeur direct de "
                        << node2 << " dans le dag initial " << std::endl;
                    return false; 
                } 

        }
    }

    int verif_value = compute_objective_value(ordre_topo); 
    if(verif_value != value_found) {
        std::cout << "Erreur dans la valeur calculée : " << std::endl;
        std::cout << "Valeur trouvée : " << value_found << std::endl;
        std::cout << "Valeur correcte : " <<  verif_value << std::endl;
        return false; 
    }
    
    std::cout << "                 [-------- Checker OK --------]" << std::endl;
    return true; 
}

std::string DagSumCut::get_name() const 
{
    return "DSC"; 
}

std::unique_ptr<Criteres> DagSumCut::clone(const Data& data) const
{
    return std::make_unique<DagSumCut>(data); 
}

void DagSumCut::aggrege_sub_solution(int& global_value, int sub_optimal_value) const
{
    global_value += sub_optimal_value; 
}

void DagSumCut::aggrege_trivial_components(int& global_value, int nb_two_nodes_comp) const 
{
    global_value += nb_two_nodes_comp;
}