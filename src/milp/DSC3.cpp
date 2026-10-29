#include "DSC3.hpp"

void DSC3::solve(bool verbose)
{
    try {
        
        GRBEnv env(true);
        env.set(GRB_IntParam_OutputFlag, verbose);
        env.set(GRB_IntParam_Threads, 1); // FORCE GUROBI TO USE ONLY ONE THREAD PER INSTANCE
        env.start();

        GRBModel model(env);

        if (time_limit > 0.0) {
            model.set(GRB_DoubleParam_TimeLimit, time_limit);
        }

        int nb_nodes = data.dag_size;
        int source = data.s; 
        int puit = data.t; 
        int nb_pos = nb_nodes+1;
        
        std::vector<int> all_pos; // to store all positions [1,n]
        for(int i = 1; i < nb_pos; ++i) all_pos.push_back(i);  

        std::vector<std::pair<int, int>> all_arcs;
        for (int i = 0; i < nb_nodes; ++i) {
            for (int j : data.dag[i]) {
                if (i != source && j != puit) {
                    all_arcs.push_back({i, j});
                }
            }
        }

        // variables 

        // xui = 1 if phi(u) <= i
        std::vector<std::vector<GRBVar>> x(nb_nodes, std::vector<GRBVar>(nb_pos)); 

        for(int u = 0; u < nb_nodes; ++u) {
            for(int i : all_pos) {
                std::string name = "x_" + std::to_string(u) + "_" + std::to_string(i); 
                x[u][i] = model.addVar(0.0, 1.0, 0.0, GRB_BINARY, name); 
            }
        }

        // variable PHI

        std::vector<GRBVar> Phi(nb_nodes); 
        for(int u = 0; u < nb_nodes; ++u) 
            Phi[u] = model.addVar(0.0, GRB_INFINITY, 1.0, GRB_CONTINUOUS, "Phi_"+std::to_string(u)); 

        // CONSTRAINTS 

        // these two constraints define Phi(s) = 0 and Phi(t) = 0
        model.addConstr(Phi[source] == 0); 
        model.addConstr(Phi[puit] ==0); 

        // these two constraints eliminate symmetry by directly fixing the position of source and sink 
        model.addConstr(x[source][1] == 1);               // phi(source) = 1
        model.addConstr(x[puit][nb_pos - 2] == 0);         // phi(sink) = nb_nodes (== n)

        // definition of Phi(u)

        for(int u = 1; u < nb_nodes-1; ++u) {
            for(int v : data.dag[u]) {
                if(v == puit) continue; 
                GRBLinExpr cstr_phi_u = 0; 
                for(int i : all_pos) 
                    cstr_phi_u += x[u][i] - x[v][i];
                model.addConstr(cstr_phi_u <= Phi[u]); 
            }
        }

        // if xui = 1, then xu(i+1) = 1 
        
        for(int u = 0; u < nb_nodes; ++u) {
            for(int i = 1; i < nb_pos-1; ++i) {
                model.addConstr(x[u][i] <= x[u][i+1]); 
            }
        }

        // the number of vertices placed before or on i is equal to i

        for(int i = 1; i < nb_pos; ++i) {
            GRBLinExpr cstr_nb_sommets = 0; 
            for(int u = 0; u < nb_nodes; ++u)
                cstr_nb_sommets += x[u][i]; 
            model.addConstr(cstr_nb_sommets == i); 
        }

        // topological order constraints (u,v) \in A implies phi(u) < phi(v)

        for(auto& arc : all_arcs) {
            int u = arc.first; 
            int v = arc.second; 
            for(int i = 2; i < nb_pos; ++i) 
                model.addConstr(x[u][i-1] >= x[v][i]); 
        }

        // Solving
        model.optimize();

        int status = model.get(GRB_IntAttr_Status);

        if (status == GRB_OPTIMAL || status == GRB_TIME_LIMIT) {

            this->total_time = model.get(GRB_DoubleAttr_Runtime); 

            if (model.get(GRB_IntAttr_SolCount) > 0) { // if at least one solution was found 
                
                this->found_solution = true; 
                this->optimal_value = model.get(GRB_DoubleAttr_ObjVal);
                this->best_bound = model.get(GRB_DoubleAttr_ObjBound);
                this->gap = model.get(GRB_DoubleAttr_MIPGap);

            } else {
                
                try {
                    this->best_bound = model.get(GRB_DoubleAttr_ObjBound); 
                } catch (const GRBException& e) {
                    std::cout << "best bound : non dispo, arret trop précoce"; 
                }
            }
        } else if (status == GRB_INFEASIBLE) {
            std::cout << "[Gurobi] ERREUR : Le modele est infaisable.\n";
        }

        // No manual cleanup needed: model then env (reverse order of
        // construction) are destroyed automatically out of scope,
        // whether the function terminates normally or via an exception.

    } catch (const GRBException& e) {
        std::cerr << "\n[CRASH GUROBI] Code " << e.getErrorCode() << " : " << e.getMessage() << "\n";
    } catch (const std::exception& e) {
        std::cerr << "\n[CRASH C++] " << e.what() << "\n";
    }
}

SolverResults DSC3::get_results() 
{
    SolverResults res;
    
    res.instance_name = this->data.instance_name; 
    res.dag_size = this->data.dag_size; 
    res.degeneracy = this->data.degenerascy;  
    res.found_solution = this->found_solution; 
    res.total_time = this->total_time;  
    res.optimal_value = (int)this->optimal_value;
    res.gap = this->gap; 
    res.best_bound = this->best_bound; 

    return res; 
}