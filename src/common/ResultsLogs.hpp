#pragma once 

#include "Common.hpp"

struct ResultsLogs 
{
    void display_FPT(const SolverResults& res, bool display_order);
    void display_Milp(const SolverResults& res, bool display_order); 
    void display_FPT_pre_traitement(const SolverResults& res, bool display_order); 
    void write_FPT_results(const SolverResults& res, const std::string& critere); 
    void write_MILP_results(const SolverResults& res, const std::string& model); 
    void write_FPT_pre_traitement_results(const SolverResults& res, const std::string& critere); 
};