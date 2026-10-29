#pragma once 

#include "Data.hpp"
#include "Common.hpp"
#include "FPT_solver.hpp"
#include "ResultsLogs.hpp"
#include "PreTraitement.hpp"
#include "DSC1.hpp"
#include "DSC2.hpp"
#include "DSC3.hpp"
#include "CW2.hpp"
#include "CW3.hpp"

struct Master
{
    const Data& data; 
    double time_limit; 
    bool write_results;
    const Criteres* critere; // the criterion to optimize (if preprocessed with FPT) 
    bool display_order = false; 

    Master(
        const Data& _data,   
        double _time_limit, 
        bool _write_results,
        const Criteres* _critere = nullptr // default, null pointer
    ): data(_data), time_limit(_time_limit), write_results(_write_results), critere(_critere) {}

    Master(
        const Data& _data
    ): data(_data) {}

    void solve_FPT_pre_traitement() const; 
    void solve_FPT() const; 
    void solve_CW2() const; 
    void solve_CW3() const; 
    void solve_DSC1() const; 
    void solve_DSC2() const; 
    void solve_DSC3() const; 

    void display_execution_choice(const std::string& algo_type, const std::string& critere) const; 
};