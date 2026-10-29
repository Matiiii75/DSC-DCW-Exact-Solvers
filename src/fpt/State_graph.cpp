#include "State_graph.hpp" 

State_graph::State_graph(const Data& _data): data(_data)
{
    std::vector<int> first_cand; // adding the 1st vertex to SG 
    first_cand = compute_first_cand(); 
    keyHash first_cand_hash = compute_cand_hash(first_cand, data.node_to_hash); 

    // by calling "add_cand_to_SG", we automatically do the following things: 
    // - add relation ID -> first_cand
    // - add relation hash_first_cand -> ID 
    // - add space to store the succs of first_cand in SG 
    // - add space at the index "ID" in weights (-1 by default)

    add_cand_to_SG(first_cand, first_cand_hash); 
    // must not forget to add it to its own layer 
    curr_layer_IDs.push_back(0); 
    curr_layer_size = 1; // the size of the associated cut_set is 1 (contains only s)
    weights[0] = 0; 
}

std::vector<int> State_graph::compute_first_cand() const {

    std::vector<int> first_cand; 
    for(int succ_s : data.dag[data.s]) // we add all successors of s 
        first_cand.push_back(succ_s); 
    sort(first_cand.begin(), first_cand.end()); // sort in ascending order

    return first_cand; 
}

int State_graph::is_cand_in_SG(const std::vector<int>& cand, const keyHash& cand_hash) const {

    auto it = hash_to_ID.find(cand_hash); // more efficient to store the location via iterator beforehand 

    if(it == hash_to_ID.end()) // if the key does not exist 
        return -1; 
    
    for(const int id : it->second) { // for each ID associated with cand_hash
        if(cand == ID_to_cands[id]) // if the clique associated with the current id == cand: we found it -> returns true 
            return id; 
    }

    return -1; 
}

void State_graph::add_cand_to_SG(const std::vector<int>& cand, const keyHash& cand_hash) {

    ID_to_cands.push_back(cand); // adds the relation ID to cand 
    int cand_ID = (int)ID_to_cands.size() - 1; // we retrieve its ID 
    hash_to_ID[cand_hash].push_back(cand_ID); // associate its ID with the hash 
    weights.push_back(-1); // we add the default weight -1 to the ID 

    assert(ID_to_cands.size() == weights.size()); 

}

const std::vector<int>& State_graph::get_cand(int ID) const {
    return ID_to_cands.at(ID); 
}

void State_graph::set_weight(int ID, int w) {

    if(ID >= (int)weights.size()) { // error handling (unlikely case)
        throw std::out_of_range("State_graph : ID : " + std::to_string(ID) + 
        " does not exist as an index in weights"); 
    }

    weights[ID] = w; 

}

void State_graph::purge_layer() 
{
    for(int id : curr_layer_IDs) {
        if(ID_to_cands[id].empty()) continue; // safety 
        keyHash cand_hash = compute_cand_hash(ID_to_cands[id], data.node_to_hash); // we find its hash 
        hash_to_ID.erase(cand_hash);  // remove the key from the hash table 
        std::vector<int>().swap(ID_to_cands[id]); // free the candidate's ram 
    }
    curr_layer_IDs.clear(); // we empty the layer 
}