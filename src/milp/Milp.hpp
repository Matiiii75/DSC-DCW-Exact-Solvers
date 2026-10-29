#pragma once 

#include "Common.hpp"
#include "gurobi_c++.h"

struct Milp 
{
    const Data& data; 
    double time_limit; 
    bool found_solution;
    double total_time; 
    double gap; 
    double optimal_value; 
    double best_bound; 
    std::vector<int> optimal_order; 

    Milp(const Data& _data, double _time_limit)
        : data(_data), time_limit(_time_limit),
        found_solution(false), total_time(0.0), gap(100.0),
        optimal_value(-1.0), best_bound(-1.0) {}
    virtual ~Milp() = default; // destructor  
    virtual void solve(bool verbose = false) = 0; // virtual method for solving the model
    virtual SolverResults get_results() = 0; 
};