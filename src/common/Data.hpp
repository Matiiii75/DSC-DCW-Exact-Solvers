#pragma once 

#include <iostream>
#include <vector>
#include <unordered_map> 
#include <unordered_set>
#include <string>
#include <random> 
#include <fstream> 
#include <algorithm> 
#include <queue>
#include <map> 
#include <cassert>
#include <optional>

using Dag = std::vector<std::vector<int>>;

// takes as input a path to an instance and returns just the instance name 
std::string getFileName(const std::string& path); 

/**
 * @brief structure that represents a 128-bit integer in two 64-bit parts
 * @param l the left 64 bits of the 128-bit integer
 * @param r the right 64 bits of the 128-bit integer
 * @note it is equipped with operator (==) & (^) to be able to compare 128-bit integers
 *  and perform XOR 
 */
struct keyHash {

    // l: left part of the number 
    // r: right part of the number
    std::uint64_t l, r; 

    // definition of the == operator (comparison)
    bool operator==(const keyHash& other) const; 

    // definition of the XOR operator 
    keyHash operator^(const keyHash& other) const; 

    // display operator for a keyHash
    std::ostream& operator<<(std::ostream& os) const; 

}; 


/**
 * @brief generates a random keyhash
 * @param moteur it is the random generator that we created before
 *  and which quickly produces an integer encoded on 64 bits
 * @return randomly generated keyhash 
 */
keyHash randomGenerator(std::mt19937_64& moteur); 


/**
 * @brief computes the hash of a candidate set (node of the state graph)
 * @param cand the candidate set 
 * @param node_to_hash structure that associates a hash to each node of the dag 
 * @return hash of the candidate set
 */
keyHash compute_cand_hash(const std::vector<int>& cand, const std::vector<keyHash>& node_to_hash); 



struct Data {

    std::string instance_name; 

    Dag dag; // initial dag
    Dag reverse_dag; // reverse_dag (we invert the direction of the arcs)

    int dag_size; 
    int nb_arcs; 
    int degenerascy; 

    int s, t; // source & sink
    
    std::vector<std::vector<bool>> TC; // transitive closure of the initial dag
    std::vector<keyHash> node_to_hash; // for each node of the dag, we associate a hash 

    /**
     * @brief Data constructor: initializes the problem data, 
     * like dag, reverse_dag, transitive closure etc.. from an instance 
     * @param file instance of the initial dag
     * @warning WARNING: the order of the vertices in dag and reverse_dag must be INCREASING !! 
     */
    Data(const std::string& file); 

    /**
     * @brief Data constructor: initializes the problem data from
     * a dag encoded as an adjacency matrix 
     * @note : WE CAN IMPROVE THIS BY PROVIDING THE TC OF THE ORIGINAL DAG AND THE NEW_TO_OLD DICTIONARY
     * SO WE DON'T HAVE TO RECOMPUTE THE TC FROM THE BEGINNING 
     */
    Data(const std::vector<std::vector<int>>& _dag); 

    void compute_transitive_closure(); // computes the transitive closure (CAN BE IMPROVED)
    void compute_node_to_hash(); // computes the hashes for each node of the dag 
    
    void display_dag() const; // dag and reverse dag display 
    void display_reverse_dag() const; 

};