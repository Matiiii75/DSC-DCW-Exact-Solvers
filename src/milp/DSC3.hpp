#pragma once 

#include "Milp.hpp"

/*
implementation of a MILP model solving DagSumCut. 
This model is based on [David Coudert, "A note on Integer Linear Programming formulations for linear ordering problems on graphs"]
It uses the following variables: 
- PHI(u) >= 0 -> captures the number of positions where u \in V participates
- xui = 1 if phi(u) <= i
*/

struct DSC3 : Milp
{   
    DSC3(const Data& _data, double _time_limit)
        : Milp(_data, _time_limit) {} // calls the constructor of the parent class
    
    virtual void solve(bool verbose = false) override; 

    virtual SolverResults get_results() override;  
};