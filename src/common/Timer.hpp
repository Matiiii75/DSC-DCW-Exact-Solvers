#pragma once 

#include <iostream>
#include <chrono>

struct Timer {

    std::chrono::steady_clock::time_point start; 

    // start a timer 
    void start_timer(); 

    // retrieve the elapsed time since the beginning 
    double get_temps_passe(); 

};