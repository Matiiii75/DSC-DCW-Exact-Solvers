#include "DagCutwidth.hpp"

int DagCutwidth::compute_weight(
    int pred_cand_set_weight, int cand_c, 
    [[maybe_unused]] const std::vector<uint8_t>& cut_set) const
{
    int C_weight = pred_cand_set_weight; 
    if(cand_c == data.t || cand_c == data.s)
        return 0; 
    if(data.reverse_dag[cand_c][0] != data.s) 
        C_weight -= (int)data.reverse_dag[cand_c].size(); 
    if(data.dag[cand_c][0] != data.t) 
        C_weight += (int)data.dag[cand_c].size(); 
    return C_weight; 
}

int DagCutwidth::compute_dist_from_pred(int best_dist_pred, int weight_pred) const 
{
    if(best_dist_pred == std::numeric_limits<int>::max())
        throw std::runtime_error("DagCutwidth::compute_dist_from_pred -> distance infinie"); 
    return std::max(best_dist_pred, weight_pred); 
}

int DagCutwidth::compute_objective_value(const std::vector<int>& ordre_topo) const 
{
    int obj_val = 0; 
    for(int i = 0; i < (int)ordre_topo.size()-1; ++i) { 
        int valeur_ordre_i = 0; 
        
        for(int j = 0; j <= i; ++j) { 
            int node_in_coupe = ordre_topo[j]; 
            if(node_in_coupe == data.s) continue; 
            
            for(int k = i+1; k < (int)ordre_topo.size(); ++k) {
                int node_hors_coupe = ordre_topo[k];
                if(node_hors_coupe == data.t) continue;
                
                // CORRECTION: Check if LEFT arcs point to the RIGHT
                for(int neigh : data.dag[node_in_coupe]) {
                    if(neigh == node_hors_coupe) {
                        ++valeur_ordre_i; 
                        break; 
                    }
                }
            }
        }
        if(valeur_ordre_i > obj_val) {
            obj_val = valeur_ordre_i; 
        }
    }
    return obj_val;
}

bool DagCutwidth::checker(const std::vector<int>& ordre_topo, int value_found) const 
{
    for(int i = 0; i < (int)ordre_topo.size()-1; ++i) { // verify that it is indeed a valid order 
        for(int j = i+1; j < (int)ordre_topo.size(); ++j) {

            int node1 = ordre_topo[i]; // 1st in the order
            int node2 = ordre_topo[j]; // 2nd

            for(int neigh : data.dag[node2]) {
                if(neigh == node1) {
                    std::cout << node1 << "est un successeur direct de "
                        << node2 << " dans le dag initial " << std::endl;
                    return false; 
                } 
            }

        }
    }

    int checker_value = compute_objective_value(ordre_topo);  
    if(checker_value != value_found) {
        std::cout << "Erreur dans la valeur calculée : " << std::endl;
        std::cout << "Valeur trouvée : " << value_found << std::endl;
        std::cout << "Valeur correcte : " << checker_value << std::endl;
        return false; 
    }

    std::cout << "                 [-------- Checker OK --------]" << std::endl;
    return true; 
}

std::string DagCutwidth::get_name() const 
{
    return "CW"; 
}

std::unique_ptr<Criteres> DagCutwidth::clone(const Data& data) const 
{
    return std::make_unique<DagCutwidth>(data); 
}

void DagCutwidth::aggrege_sub_solution(int& global_value, int sub_optimal_value) const 
{
    global_value = std::max(global_value, sub_optimal_value); 
}

void DagCutwidth::aggrege_trivial_components(int& global_value, int nb_two_nodes_comp) const 
{
    if (nb_two_nodes_comp > 0) {
        global_value = std::max(global_value, 1);
    }
}