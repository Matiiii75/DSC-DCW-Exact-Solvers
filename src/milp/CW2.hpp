#pragma once 

#include "Milp.hpp"

struct CW2 : Milp
{   
    CW2(const Data& _data, double _time_limit)
        : Milp(_data, _time_limit) {} // calls the constructor of the parent class
    
    virtual void solve(bool verbose = false) override; 

    virtual SolverResults get_results() override;  
};