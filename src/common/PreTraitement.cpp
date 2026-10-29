#include "PreTraitement.hpp"

std::vector<std::vector<int>> PreTraitement::compute_composantes()
{
    std::vector<std::vector<int>> composantes; 
    std::vector<int> to_visit; 
    for(int u = 0; u < data.dag_size; ++u) {
        if(u == data.s || u == data.t) continue; 
        to_visit.push_back(u); 
    }
    std::vector<uint8_t> visited(data.dag_size, 0); 
    for(int u : to_visit)
    {
        if(visited[u]) continue; 
        std::vector<int> composante; 
        visited[u] = 1; // remember that we visited it 
        std::queue<int> fifo_list; 
        fifo_list.push(u);

        while(!fifo_list.empty())
        {
            int curr = fifo_list.front(); 
            fifo_list.pop(); 
            composante.push_back(curr);
            
            for(int curr_succ : data.dag[curr]) { 
                if(curr_succ == data.t || visited[curr_succ]) continue; 
                visited[curr_succ] = 1; 
                fifo_list.push(curr_succ); // add to fifo to process its neighbors 
            }
            for(int curr_preds : data.reverse_dag[curr]) {
                if(curr_preds == data.s || visited[curr_preds]) continue;  
                visited[curr_preds] = 1; 
                fifo_list.push(curr_preds); 
            }
        }
        int comp_size = (int)composante.size(); 
        if(comp_size == 1) {
            this->one_node_comp.push_back(composante[0]);
        }
        if(comp_size == 2) {
            int u = composante[0]; 
            int v = composante[1]; 
            two_nodes_comp.push_back({u,v}); 
        }
        if(comp_size > 2) {
            composantes.push_back(composante); 
        }
    }
    return composantes; 
}

Dag PreTraitement::compute_induced_dag(const std::vector<int>& composante)
{
    int new_dag_size = (int)composante.size()+2; // because we add s & t 
    int initial_puit = data.t; 
    int source = 0; 
    int puit = new_dag_size-1; // sink of the new dag 

    std::vector<int> new_to_old(new_dag_size); // new_to_old[i] = j -> j becomes i 
    std::unordered_map<int,int> old_to_new; // otn[i] = j -> i has become j 

    new_to_old[source] = source; 
    old_to_new[source] = source;    // the new s & t are mapped to themselves
    new_to_old[puit] = puit;        
    old_to_new[puit] = puit; 

    int new_next_dispo = 1; // value of the next available new node
    for(int u : composante) {
        new_to_old[new_next_dispo] = u; 
        old_to_new[u] = new_next_dispo; 
        new_next_dispo++; // we move to the next 
    }

    // subgraph creation 

    Dag new_dag(new_dag_size);
    std::vector<bool> has_0_pred(new_dag_size, true); // true -> zero in-degree
    
    for(int u : composante) { 
        int new_u = old_to_new.at(u); // we retrieve its new value
        for(int u_neigh : this->data.dag[u]) { 
            if(u_neigh == initial_puit) continue; 
            int new_neigh_u = old_to_new.at(u_neigh); // new value of the neighbor
            has_0_pred[new_neigh_u] = false; 
            new_dag[new_u].push_back(new_neigh_u); 
        }
    }
    // connect source and sink in the new dag 
    for(int u = 1; u < puit; ++u) { 
        if(has_0_pred[u]) 
            new_dag[source].push_back(u); 
        if((int)new_dag[u].size() == 0) { 
            new_dag[u].push_back(puit);   
        }
    }

    this->all_map_new_to_old.push_back(new_to_old); 
    return new_dag; 
}

void PreTraitement::compute_sub_instances()
{
    std::vector<std::vector<int>> composantes; 
    composantes = compute_composantes();
    for(const auto& composante : composantes) {
        Dag sub_dag = compute_induced_dag(composante); 
        Data sub_instance(sub_dag); 
        this->sub_instances.push_back(sub_instance); 
    }
}

std::vector<int> PreTraitement::re_label_sub_solution(
    const std::vector<int>& sub_sol, 
    int instance_idx
) const 
{
    std::vector<int> re_labelled_sol;
    for(int pos = 0; pos < (int)sub_sol.size(); ++pos) 
    {
        int node = sub_sol[pos]; 
        re_labelled_sol.push_back(this->all_map_new_to_old[instance_idx][node]); 
    }
    return re_labelled_sol; 
}

SolverResults PreTraitement::store_results(
    int optimal_value, 
    int total_cands, 
    double total_time, 
    const std::vector<int>& optimal_order
) const 
{
    SolverResults res;
    res.instance_name = data.instance_name; 
    res.dag_size = data.dag_size; 
    res.degeneracy = data.degenerascy; 
    res.optimal_value = optimal_value; 
    res.nb_cands = total_cands; 
    res.total_time = total_time; 
    res.optimal_order = optimal_order; 
    res.nb_sub_pb_solved = (int)sub_instances.size();  
    res.nb_total_composantes = count_composantes();  
    return res; 
}

int PreTraitement::count_composantes() const
{
    return (int)one_node_comp.size() + (int)two_nodes_comp.size() + (int)sub_instances.size(); 
}