#include "Master.hpp"

void Master::solve_FPT_pre_traitement() const 
{
    if(critere == nullptr)
        throw std::invalid_argument("Master::solver_FPT_pre_traitement -> critere == nullptr"); 

    display_execution_choice("FPT-PT", critere->get_name()); 

    double time_remaining = time_limit; 
    int total_value = 0; 
    int nb_total_cands = 0; 
    std::vector<std::vector<int>> sub_solutions; 
    SolverResults res; 
    ResultsLogs logs; 
    bool found_global_solution = true; 

    PreTraitement pt(data); 
    pt.compute_sub_instances(); 

    for(int i = 0; i < (int)pt.sub_instances.size(); ++i) 
    {
        // we must create sub_critere because critere possesses the topology
        // of the initial dag and will therefore not be able to evaluate the cuts properly. 
        // we therefore create a sub_critere from sub_inst
        const Data& sub_inst = pt.sub_instances[i]; 
        auto sub_critere = critere->clone(sub_inst); 
        State_graph sub_SG(sub_inst); 
        FPT_solver sub_solver(sub_inst, sub_SG, *sub_critere, time_remaining);
        sub_solver.solve(); 
        SolverResults sub_res = sub_solver.get_results();
        nb_total_cands += sub_res.nb_cands; 
        time_remaining -= sub_res.total_time; 

        if(sub_res.found_solution) {
            critere->aggrege_sub_solution(total_value, sub_res.optimal_value);  
            std::vector<int> sub_solution = sub_res.optimal_order; 
            std::vector<int> relabelled_sol;
            relabelled_sol = pt.re_label_sub_solution(sub_solution, i); 
            sub_solutions.push_back(relabelled_sol); 
        } else {
            found_global_solution = false; 
            break;
        }
    }
    critere->aggrege_trivial_components(total_value, (int)pt.two_nodes_comp.size()); 

    double total_time = time_limit - time_remaining; 

    if(!found_global_solution) // if we haven't found a complete solution
    {
        res = pt.store_results(
            -1, // default opt
            nb_total_cands, 
            total_time, 
            {-1} // default global solution 
        ); 
    } 
    else // if we have found a complete solution
    {
        // reconstruction of the solution: 
        std::vector<int> global_solution; 
        
        for(int node : pt.one_node_comp) // add 1-vertex comp
            global_solution.push_back(node); 

        for(auto& nodes : pt.two_nodes_comp) { // add 2-vertices comp
            int u = nodes.first; 
            int v = nodes.second; 
            bool u_is_pref_of_v = false; 
            for(int neigh : data.dag[u]) {
                if(neigh == v) {
                    u_is_pref_of_v = true; 
                    global_solution.push_back(u); 
                    global_solution.push_back(v); 
                    break; 
                }
            }
            if(!u_is_pref_of_v) {
                global_solution.push_back(v); 
                global_solution.push_back(u); 
            }
        }

        for(auto& sol : sub_solutions) { // add > 2 vertices comp 
            for(int node : sol) 
                global_solution.push_back(node); 
        }

        res = pt.store_results(
            total_value, 
            nb_total_cands, 
            total_time, 
            global_solution
        );  
    }

    logs.display_FPT_pre_traitement(res, display_order); 
    if(found_global_solution)
        critere->checker(res.optimal_order, res.optimal_value);
    if(write_results)
        logs.write_FPT_pre_traitement_results(res, critere->get_name()); 
}

void Master::solve_FPT() const 
{
    if(critere == nullptr)
        throw std::invalid_argument("Master::solver_FPT -> critere == nullptr ...");

    display_execution_choice("FPT", critere->get_name()); 

    SolverResults res; 
    ResultsLogs logs; 
    State_graph SG(data); 
    FPT_solver solver(data, SG, *critere, time_limit); 
    solver.solve(); 
    res = solver.get_results(); 
    logs.display_FPT(res, display_order); 
    if(res.found_solution)
        critere->checker(res.optimal_order, res.optimal_value); 
    if(write_results)
        logs.write_FPT_results(res, critere->get_name()); 
}

void Master::solve_CW2() const 
{
    display_execution_choice("CW2", "CW"); 

    SolverResults res; 
    ResultsLogs logs; 
    CW2 milp(data, time_limit); 
    milp.solve(); 
    res = milp.get_results(); 
    logs.display_Milp(res, display_order); 
    if(write_results)
        logs.write_MILP_results(res, "CW2"); 
}

void Master::solve_CW3() const 
{
    display_execution_choice("CW3", "CW");

    SolverResults res; 
    ResultsLogs logs; 
    CW3 milp(data, time_limit); 
    milp.solve(); 
    res = milp.get_results(); 
    logs.display_Milp(res, display_order); 
    if(write_results)
        logs.write_MILP_results(res, "CW3"); 
}

void Master::solve_DSC1() const 
{
    display_execution_choice("DSC1", "DSC");

    SolverResults res; 
    ResultsLogs logs; 
    DSC1 milp(data, time_limit); 
    milp.solve(); 
    res = milp.get_results(); 
    logs.display_Milp(res, display_order); 
    if(write_results)
        logs.write_MILP_results(res, "DSC1"); 
}

void Master::solve_DSC2() const 
{
    display_execution_choice("DSC2", "DSC");

    SolverResults res; 
    ResultsLogs logs; 
    DSC2 milp(data, time_limit); 
    milp.solve(); 
    res = milp.get_results(); 
    logs.display_Milp(res, display_order); 
    if(write_results)
        logs.write_MILP_results(res, "DSC2"); 
}

void Master::solve_DSC3() const 
{
    display_execution_choice("DSC3", "DSC");

    SolverResults res; 
    ResultsLogs logs; 
    DSC3 milp(data, time_limit); 
    milp.solve(); 
    res = milp.get_results(); 
    logs.display_Milp(res, display_order); 
    if(write_results)
        logs.write_MILP_results(res, "DSC3"); 
}

void Master::display_execution_choice(const std::string& algo_type, const std::string& critere) const 
{
    std::cout << "             [-------- EXECUTING "; 
    std::cout << algo_type << " " << critere; 
    std::cout << " --------]" << std::endl;
    std::cout << std::endl;
    std::cout << "[Time limit]            : " << time_limit << std::endl;
    std::cout << "[Write results]         : "; 
    if(write_results) std::cout << "enabled" << std::endl;
    else std::cout << "disabled" << std::endl;
    std::cout << std::endl;
}