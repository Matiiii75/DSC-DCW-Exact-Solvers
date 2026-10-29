#pragma once 

#include "Data.hpp"

// custom keyHasher for the unordered_map of the "state_graph" struct
// allows it to hash keys
struct KeyHasher { 

    std::size_t operator()(const keyHash& k) const {
        return static_cast<std::size_t>(k.l); 
    }

}; 

// custom comparison operator between two keys for the um of struct "state_graph" 
struct KeyEqual {

    bool operator()(const keyHash& k1, const keyHash& k2) const {
        return k1.l == k2.l && k1.r == k2.r; 
    }

}; 

struct State_graph 
{

    const Data& data; 
    std::vector<std::vector<int>> ID_to_cands; 
    std::unordered_map<keyHash, std::vector<int>, KeyHasher, KeyEqual> hash_to_ID; 
    std::vector<int> weights; 
    int curr_layer_size; 
    std::vector<int> curr_layer_IDs;    

    State_graph(const Data& _data); 

    /**
     * @brief computes the first candidate set
     * @note we must find all direct successors of s that
     *  have only s as a direct predecessor
     * @warning handles the sorting of first_cand internally 
     */
    std::vector<int> compute_first_cand() const; 

    /**
     * @brief checks if a candidate set is already in SG 
     * @param cand candidate set to check 
     * @param cand_hash the hash of the candidate set (pre-computed)
     * @return the ID if cand is already in SG, -1 otherwise 
     * @warning cand must be sorted in ascending order
     */
    int is_cand_in_SG(const std::vector<int>& cand, const keyHash& cand_hash) const ; 

    /**
     * @brief adds a candidate set to the state graph 
     * @param cand the candidate set to add 
     * @param cand_hash the pre-computed hash of cand 
     * @warning we must enter this function ONLY IF cand is not already in SG 
     * @note the function 
     */
    void add_cand_to_SG(const std::vector<int>& cand, const keyHash& cand_hash);  

    /**
     * @brief getter that returns the candidate set associated with an ID 
     * @param ID the desired ID
     * @warning shouldn't happen but if the index ID didn't exist, will throw an error 
     * @note const ref on the return type to avoid copying since we'll just iterate over it 
     */
    const std::vector<int>& get_cand(int ID) const; 

    /**
     * @brief sets the weight of an ID 
     * @param ID 
     * @param w the weight of ID 
     * @note throws an error if ID >= size(weights)
     */
    void set_weight(int ID, int w); 

    void purge_layer(); 

};