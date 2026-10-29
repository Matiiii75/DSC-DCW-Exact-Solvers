#include "DSC2.hpp"

void DSC2::solve(bool verbose)
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

        std::vector<int> all_pos;
        for (int i = 0; i < nb_nodes - 2; ++i) {
            all_pos.push_back(i);
        }

        std::vector<int> all_nodes;
        for (int i = 0; i < nb_nodes; ++i) {
            if (i != source && i != puit) {
                all_nodes.push_back(i);
            }
        }

        std::vector<std::pair<int, int>> all_arcs;
        for (int i = 0; i < nb_nodes; ++i) {
            for (int j : data.dag[i]) {
                if (i != source && j != puit) {
                    all_arcs.push_back({i, j});
                }
            }
        }

        // VARIABLES DEFINITION 

        // z: zij = 1 if i is placed before j 

        std::vector<std::vector<GRBVar>> z(nb_nodes, std::vector<GRBVar>(nb_nodes)); 

        for(int i = 0; i < nb_nodes; ++i) {
            for(int j = 0; j < nb_nodes; ++j) {
                std::string name = "z_" + std::to_string(i) + "_" + std::to_string(j); 
                z[i][j] = model.addVar(0.0, 1.0, 0.0, GRB_BINARY, name); 
            }
        }

        // Phi 

        std::vector<GRBVar> Phi(nb_nodes); 

        for(int i : all_nodes) {
            std::string name = "Phi_" + std::to_string(i); 
            Phi[i] = model.addVar(0.0, GRB_INFINITY, 0.0, GRB_CONTINUOUS, name); 
        }

        // Mu 

        std::vector<GRBVar> Mu(nb_nodes); 

        for(int i : all_nodes) {
            std::string name = "Mu_" + std::to_string(i); 
            Mu[i] = model.addVar(0.0, GRB_INFINITY, 0.0, GRB_CONTINUOUS, name); 
        }

        // OBJECTIVE DEFINITION 

        GRBLinExpr obj = 0; 
        
        for(int u : all_nodes) {
            obj += Mu[u] - Phi[u]; 
        }
        model.setObjective(obj, GRB_MINIMIZE); 

        // CONSTRAINTS DEFINITION 

        // if (u,v) \in A -> zuv = 1

        for(auto& arc : all_arcs) {
            int u = arc.first; 
            int v = arc.second; 
            model.addConstr(z[u][v] == 1); 
        }

        // antisymmetry 

        for(int i = 0; i < (int)all_nodes.size(); ++i) {
            int u = all_nodes[i]; 
            for(int j = i+1; j < (int)all_nodes.size(); ++j) {
                int v = all_nodes[j]; 
                model.addConstr(z[u][v] + z[v][u] == 1); 
            }
        }

        // Phi definitions

        for(int u : all_nodes) {
            GRBLinExpr cstr_def_Phi = 0; 
            for(int v : all_nodes) {
                if(v == u) continue; 
                cstr_def_Phi += z[v][u]; 
            }
            model.addConstr(Phi[u] == cstr_def_Phi); 
        }

        // Mu DEFINITION 

        for(int u : all_nodes) {
            model.addConstr(Mu[u] >= Phi[u]); 
            for(int v : data.dag[u]) {
                if(v == puit) continue; 
                model.addConstr(Mu[u] >= Phi[v]); 
            }
        }

         

        for(int u = 1; u < puit-2; ++u) {
            for(int v = u+1; v < puit-1; ++v) {
                for(int w = v+1; w < puit; ++w) {

                    model.addConstr(z[u][v] + z[v][w] - 1 <= z[u][w]); 
                    model.addConstr(z[u][w] + z[w][v] - 1 <= z[u][v]); 

                }
            }
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

SolverResults DSC2::get_results() 
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