#include "CW2.hpp"

void CW2::solve(bool verbose)
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

        // VARIABLES 

        // z: zij = 1 if i is placed before j 

        std::vector<std::vector<GRBVar>> z(nb_nodes, std::vector<GRBVar>(nb_nodes)); 

        for(int i = 0; i < nb_nodes; ++i) {
            for(int j = 0; j < nb_nodes; ++j) {
                std::string name = "z_" + std::to_string(i) + "_" + std::to_string(j); 
                z[i][j] = model.addVar(0.0, 1.0, 0.0, GRB_BINARY, name); 
            }
        }

        // l: lui = 1 if phi(u) <= i
        // r: rui = 1 if phi(u) > i 

        std::vector<std::vector<GRBVar>> l(nb_nodes, std::vector<GRBVar>(nb_pos)); 
        std::vector<std::vector<GRBVar>> r(nb_nodes, std::vector<GRBVar>(nb_pos));

        for(int u = 0; u < nb_nodes; ++u) {
            for(int i : all_pos) {
                std::string name1 = "l_" + std::to_string(u) + "_" + std::to_string(i); 
                l[u][i] = model.addVar(0.0, 1.0, 0.0, GRB_BINARY, name1); 
                std::string name2 = "r_" + std::to_string(u) + "_" + std::to_string(i); 
                r[u][i] = model.addVar(0.0, 1.0, 0.0, GRB_BINARY, name2);
            }
        }

        // variables auvi = 1 if phi(u) <= i, phi(v) > i. 0 otherwise 
        std::map<std::pair<int,int>, std::vector<GRBVar>> a; 
        for(const auto& arc : all_arcs) {
            int u = arc.first; 
            int v = arc.second; 
            a[{u,v}].resize(nb_pos); // allocate the exact size for this arc
            for(int i : all_pos) {
                std::string name = "a_" + std::to_string(u) + "_" + std::to_string(v) + std::to_string(i); 
                // we can declare z as CONTINUOUS 
                a[{u,v}][i] = model.addVar(0.0, 1.0 , 0.0, GRB_CONTINUOUS, name); 
            }
        }

        // C : 

        GRBVar C = model.addVar(0.0, GRB_INFINITY, 0.0, GRB_CONTINUOUS, "C"); 

        // objective 

        GRBLinExpr obj = C; 
        model.setObjective(obj, GRB_MINIMIZE); 

        // constraints 

        for(auto& arc : all_arcs) {
            int u = arc.first; 
            int v = arc.second; 
            model.addConstr(z[u][v] == 1); 
        }

        // z[s][u] = 1 for all u
        for(int u = 0; u < nb_nodes; ++u) {
            if(u == source) continue; 
            model.addConstr(z[source][u] == 1); 
        }

        // z[u][t] = 1 for all u
        for(int u = 0; u < nb_nodes; ++u) {
            if(u == puit) continue; 
            model.addConstr(z[u][puit] == 1); 
        }

        // antisymmetry 
        for(int u = 0; u < nb_nodes-1; ++u) { 
            for(int v = u+1; v < nb_nodes; ++v) {
                model.addConstr(z[u][v] + z[v][u] == 1); 
            }
        }

        // transitivity
        for(int u = 1; u < puit-2; ++u) {
            for(int v = u+1; v < puit-1; ++v) {
                for(int w = v+1; w < puit; ++w) {

                    model.addConstr(z[u][v] + z[v][w] - 1 <= z[u][w]); 
                    model.addConstr(z[u][w] + z[w][v] - 1 <= z[u][v]); 

                }
            }
        }

        // definition of l 
        for(int u = 0; u < nb_nodes; ++u) {
            for(int i : all_pos) {
                GRBLinExpr cstr_def_l = 0; 
                for(int w = 0; w < nb_nodes; ++w) {
                    if(w == u) continue; 
                    cstr_def_l += z[w][u]; 
                }
                model.addConstr(nb_pos*l[u][i] >= i - cstr_def_l);
            }
        }

        // definition of r 
        for(int u = 0; u < nb_nodes; ++u) {
            for(int i : all_pos) {
                GRBLinExpr cstr_def_r = 0; 
                for(int w = 0; w < nb_nodes; ++w) {
                    if(w == u) continue; 
                    cstr_def_r += z[w][u]; 
                }
                model.addConstr(nb_pos*r[u][i] >= cstr_def_r - i + 1);
            }
        }

        // definition of auvi 
        for(auto& arc : all_arcs) {
            for(int i : all_pos) {
                int u = arc.first; 
                int v = arc.second; 
                model.addConstr(a[{u,v}][i] >= l[u][i] + r[v][i] - 1); 
            }
        }

        // definition of C 

        for(int i : all_pos) {
            GRBLinExpr cstr_def_C = 0; 
            for(auto& arc : all_arcs) {
                int u = arc.first; 
                int v = arc.second; 
                cstr_def_C += a[{u,v}][i]; 
            }
            model.addConstr(C >= cstr_def_C); 
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

SolverResults CW2::get_results() 
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