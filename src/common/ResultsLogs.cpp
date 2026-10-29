#include "ResultsLogs.hpp"

void ResultsLogs::display_FPT(const SolverResults& res, bool display_order)
{
    std::cout << "-------------------------------------------------------------------------"; 
    std::cout << std::endl;
    std::cout << "               [----- AFFICHAGES RÉSULTATS -----]" << std::endl;
    std::cout << std::endl;
    std::cout << "[Instance]              : " << res.instance_name << std::endl;
    std::cout << "[Dag size & Degeneracy] : "; 
    std::cout << res.dag_size << " / "; 
    std::cout << res.degeneracy << std::endl;
    if(display_order) {
        std::cout << "[Ordre topologique optimal] : " << std::endl;
        for(int i : res.optimal_order) std::cout << i << ", "; 
        std::cout << std::endl;
    }   
    std::cout << "[Temps total]           : "; 
    std::cout << res.total_time << " sec" << std::endl;
    std::cout << "[valeur optimale]       : " << res.optimal_value << std::endl;
    std::cout << "[nombre de candidats]   : " << res.nb_cands << std::endl;
    std::cout << std::endl;
    std::cout << "-------------------------------------------------------------------------"; 
    std::cout << std::endl;
}

void ResultsLogs::display_Milp(const SolverResults& res, bool display_order)
{
    std::cout << "----- [RESULTATS GUROBI] -----" << std::endl;    

    if(res.found_solution) { // if a solution was found
        std::cout << "--- [Solution trouvée] ---" << std::endl;
        std::cout << "[valeur] : " << res.optimal_value << std::endl;
        std::cout << "[best bound] : " << res.best_bound<< std::endl;
        std::cout << "[Gap] : " << res.gap << std::endl;
        std::cout << "[Temps] : " << res.total_time << std::endl;
        if(display_order) {
            std::cout << "[ordre optimal] : " << std::endl;
            for(int i : res.optimal_order) std::cout << i << ", "; 
            std::cout << std::endl;
        }
    } else {
        std::cout << "--- [Aucune solution trouvée dans le temps imparti] ---" << std::endl;
        std::cout << "[Temps] : " << res.total_time << std::endl;
    }
}

void ResultsLogs::display_FPT_pre_traitement(const SolverResults& res, bool display_order) 
{
    std::cout << "-------------------------------------------------------------------------"; 
    std::cout << std::endl;
    std::cout << "               [----- AFFICHAGES RÉSULTATS -----]" << std::endl;
    std::cout << std::endl;
    std::cout << "[Instance]                         : " << res.instance_name << std::endl;
    std::cout << "[Dag size & Degeneracy]            : "; 
    std::cout << res.dag_size << " / "; 
    std::cout << res.degeneracy << std::endl;
    if(display_order) {
        std::cout << "[Ordre topologique optimal]            : " << std::endl;
        for(int i : res.optimal_order) std::cout << i << ", "; 
        std::cout << std::endl;
    }   
    std::cout << "[Temps total]                      : "; 
    std::cout << res.total_time << " sec" << std::endl;
    std::cout << "[valeur optimale]                  : " << res.optimal_value << std::endl;
    std::cout << "[nombre de candidats]              : " << res.nb_cands << std::endl;
    std::cout << "[nombre composantes]               : " << res.nb_total_composantes << std::endl;
    std::cout << "[nombre composantes de taille > 2] : " << res.nb_sub_pb_solved << std::endl;
    std::cout << std::endl;
    std::cout << "-------------------------------------------------------------------------"; 
    std::cout << std::endl;
}

void ResultsLogs::write_FPT_results(const SolverResults& res, const std::string& critere)
{
    std::string path_file = "results/results_FPT_" + critere + ".txt"; 
    std::ofstream writing(path_file, std::ios::app); 
    writing << res.instance_name << " " << res.dag_size << " ";
    writing << res.degeneracy << " " << res.optimal_value << " ";
    writing << res.nb_cands << " " << res.total_time; 
    writing << std::endl;
    writing.close(); 
}

void ResultsLogs::write_MILP_results(const SolverResults& res, const std::string& model)
{   
    std::string path_file = "results/results_MILP_" + model + ".txt"; 
    std::ofstream writing(path_file, std::ios::app); 
    writing << res.instance_name << " " << res.dag_size << " "; 
    writing << res.degeneracy << " " << res.optimal_value << " "; 
    writing << res.total_time << " " << res.gap << " "; 
    writing << res.best_bound << " " << res.found_solution << std::endl;
    writing.close(); 
}

void ResultsLogs::write_FPT_pre_traitement_results(const SolverResults& res, const std::string& critere)
{
    std::string path_file = "results/results_FPT_PT_" + critere + ".txt"; 
    std::ofstream writing(path_file, std::ios::app); 
    writing << res.instance_name << " " << res.dag_size << " ";
    writing << res.degeneracy << " " << res.optimal_value << " ";
    writing << res.nb_cands << " " << res.total_time << " "; 
    writing << res.nb_total_composantes << " " << res.nb_sub_pb_solved << " "; 
    writing << std::endl;
    writing.close();
}