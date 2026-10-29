#include <iostream>
#include <string>
#include <exception>

#include "Data.hpp"
#include "Master.hpp"
#include "State_graph.hpp"
#include "DagSumCut.hpp"
#include "DagCutwidth.hpp"
#include "FPT_solver.hpp"
#include "ResultsLogs.hpp"
#include "DSC1.hpp"
#include "DSC2.hpp"
#include "DSC3.hpp"
#include "CW2.hpp"
#include "CW3.hpp"


/*
Main of the problem. 
Takes 3 arguments: 
1 -> problem instance
2 -> algorithm choice 
3 -> choice to write the results to a .txt file
To run the code: 
./prog [instances/inst.txt] [algo_choice] [write_choice]
Parameter details: 
0 -> DSC FPT 
1 -> CW FPT 
2 -> LP DSC1 
3 -> LP DSC2 
4 -> LP DSC3
5 -> LP CW2 
6 -> LP CW3
7 -> FPT DSC + Preprocessing
8 -> FPT DCW + Preprocessing
*/

int main(int argc, char* argv[]) 
{   
    if(argc != 4) std::cout << "erreur nb args" << std::endl;

    std::string instance_file = argv[1]; 
    int choice_algo = atoi(argv[2]); 
    int write_results = atoi(argv[3]); 
    double time_limit = 600.00; 
    Data data(instance_file);

    if(choice_algo == 0) // DSC FPT
    {
        DagSumCut critere_DSC(data); 
        Master master(data, time_limit, write_results, &critere_DSC); 
        master.solve_FPT(); 
    }
    if(choice_algo == 1) // CW FPT 
    {
        DagCutwidth critere_CW(data); 
        Master master(data, time_limit, write_results, &critere_CW); 
        master.solve_FPT(); 
    }
    if(choice_algo == 2) // LP DSC1
    {
        Master master(data, time_limit, write_results); 
        master.solve_DSC1(); 
    }
    if(choice_algo == 3) // LP DSC2
    {
        Master master(data, time_limit, write_results); 
        master.solve_DSC2();
    }
    if(choice_algo == 4) // LP DSC3
    {
        Master master(data, time_limit, write_results); 
        master.solve_DSC3();  
    }
    if(choice_algo == 5) // LP CW2
    {
        Master master(data, time_limit, write_results); 
        master.solve_CW2();
    }
    if(choice_algo == 6) // LP CW3
    {
        Master master(data, time_limit, write_results); 
        master.solve_CW3(); 
    }
    if(choice_algo == 7) // DSC preprocessing
    {
        DagSumCut critere_DSC(data); 
        Master master(data, time_limit, write_results, &critere_DSC); 
        master.solve_FPT_pre_traitement(); 
    }
    if(choice_algo == 8) // DCW preprocessing
    {
        DagCutwidth critere_CW(data); 
        Master master(data, time_limit, write_results, &critere_CW); 
        master.solve_FPT_pre_traitement(); 
    }

    return 0; 
}