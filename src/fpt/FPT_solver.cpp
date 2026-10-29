#include "FPT_solver.hpp"

FPT_solver::FPT_solver(
    const Data& _data, 
    State_graph& _SG, 
    const Criteres& _critere, 
    double _time_limit
): data(_data), SG(_SG), critere(_critere), time_limit(_time_limit), found_solution(true) 
{
    timer.start_timer(); 
    // Initialization of the root (state 0)
    L.push(0); 
    best_dist.push_back(0); 
    pred_in_pcc.push_back({-1, -1});
}

void FPT_solver::compute_cut_set(
    const std::vector<int>& cand, 
    int& cut_set_size, 
    std::vector<uint8_t>& cut_set, 
    std::vector<int>& hors_cut_set
) const 
{
    // Remember that to compute the cut-set, we need to determine the elements 
    // that are not in N_G^+(cand). We set all elements to 1. We traverse N_G^+(cand)
    // using the transitive closure and we set 0 for each vertex we encounter. 

    for(const int c : cand) { // for each candidate 
        for(int i = 0; i < data.dag_size; ++i) { // for each graph node
            if(data.TC[c][i] && cut_set[i]) { // if c->i && we haven't seen i yet 
                cut_set[i] = 0; // if c -> i, i is not in the cut-set 
                hors_cut_set.push_back(i); 
            }
        }
    }

    cut_set_size = data.dag_size - (int)hors_cut_set.size(); 
}

std::vector<int> FPT_solver::rebuild_opt_order() const 
{
    std::vector<int> ordre_topo; 
    int curr_node = (int)pred_in_pcc.size() - 1; // we retrieve the last candidate set ( {t} )
    while(curr_node != 0) {
        ordre_topo.push_back(pred_in_pcc[curr_node].second); 
        curr_node = pred_in_pcc[curr_node].first; 
    }
    std::reverse(ordre_topo.begin(), ordre_topo.end()); 
    return ordre_topo;  
}

bool FPT_solver::try_eval_time_limit(int iteration_count) 
{
    if(iteration_count % 10000 != 0) 
        return true; 
    double temps_courant = timer.get_temps_passe(); 
    if(temps_courant >= time_limit) {
        this->found_solution = false; 
        return false; 
    }
    return true; 
}

void FPT_solver::solve() 
{   
    int iteration_count = 0; 
    std::vector<int> C2; // pre-allocation 
    C2.reserve(data.dag_size); 

    while(!L.empty())
    {
        iteration_count++; 
        if(!try_eval_time_limit(iteration_count)) // if false -> we must stop 
            break; 
        
        int C_ID = L.front(); 
        L.pop(); 
        std::vector<int> C = SG.get_cand(C_ID); 
        int cut_set_size = 0; // 0 by default, we will set it in compute_cut_set
        std::vector<uint8_t> cut_set(data.dag_size, 1); 
        std::vector<int> hors_cut_set; 
        compute_cut_set(C, cut_set_size, cut_set, hors_cut_set);
        
        // checking new layer? 
        // (a layer is a set of cands whose associated cut-sets have the same size)
        if(cut_set_size > SG.curr_layer_size) {
            SG.purge_layer(); // cleanup 
            SG.curr_layer_size++; // we start a new layer of size +1
        }

        SG.curr_layer_IDs.push_back(C_ID); 
        
        for(int i = 0; i < (int)C.size(); ++i) // for each candidate in C
        { 
            C2 = C; 
            C2[i] = C2.back(); 
            C2.pop_back();
            int curr_c = C[i]; // copy c 
            cut_set[curr_c] = 1; // the candidate enters the cutset 

            for(int u : data.dag[curr_c]) { // we add all succs of c that have no more preds outside cutset
                if(is_included(data.reverse_dag[u], cut_set, -1)) 
                    C2.push_back(u); // -1 means we look at simple inclusion of r-dag in e-c-s
            }   

            sort(C2.begin(), C2.end()); // sort in ascending order 
            if(C2.size() == 0) continue; 

            keyHash C2_hash = compute_cand_hash(C2, data.node_to_hash); 
            int C2_ID = SG.is_cand_in_SG(C2, C2_hash); 

            if(C2_ID == -1) // (l.10) (if it is -1, it means we haven't found an ID for this hash)
            { 
                SG.add_cand_to_SG(C2, C2_hash); 
                C2_ID = (int)SG.ID_to_cands.size()-1; // we just added it, its ID is the last index 
                L.push(C2_ID); // add to FIFO
                int C_weight = SG.weights[C_ID]; 
                int C2_weight = critere.compute_weight(C_weight, curr_c, cut_set); // cut_set is the cut-set of C2 
                SG.set_weight(C2_ID, C2_weight); // we compute the weight of C2 
                best_dist.push_back(std::numeric_limits<int>::max()); // inf by default 
                pred_in_pcc.push_back({-1,-1}); // add a default vertex to keep pred_in_pcc properly indexed 
            }

            // SG.add_arc_from_C1_to_C2(C_ID, C2_ID); 

            update_distance_label(C_ID, C2_ID, curr_c); // update the distance label 

            // we finish the loop on curr_c 
            // -> we remove it from the cut-set and move to the next candidate 
            cut_set[curr_c] = 0; 
        }
    }

    // retrieving results 
    this->total_time = timer.get_temps_passe();  
}

SolverResults FPT_solver::get_results() const 
{
    SolverResults res;
    res.instance_name = this->data.instance_name; 
    res.dag_size = this->data.dag_size; 
    res.degeneracy = this->data.degenerascy;  
    res.found_solution = this->found_solution; 
    res.total_time = this->total_time; 
    res.nb_cands = this->SG.ID_to_cands.size(); 
    if(found_solution) {
        res.optimal_value = best_dist.back(); 
        res.optimal_order = rebuild_opt_order(); 
    }
    return res; 
}