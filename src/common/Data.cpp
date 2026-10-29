#include "Data.hpp"

std::string getFileName(const std::string& path) {

    size_t lastSlash = path.find_last_of("/\\"); // mac/linux compatible
    return (lastSlash == std::string::npos) ? path : path.substr(lastSlash + 1);

}

/* BEGIN CANDIDATES KEYHASH SECTION */

bool keyHash::operator==(const keyHash& other) const {
    return l == other.l && r == other.r;
}

keyHash keyHash::operator^(const keyHash& other) const {
    keyHash nouveau;
    nouveau.l = l ^ other.l; 
    nouveau.r = r ^ other.r; 
    return nouveau;
}

std::ostream& keyHash::operator<<(std::ostream& os) const {
    os << l << " " << r; 
    return os; 
}

keyHash randomGenerator(std::mt19937_64& moteur) {

    // to generate randomly between 0 and the max encoded on 64 bits 
    std::uniform_int_distribution<std::uint64_t> dist(0, 
        std::numeric_limits<std::uint64_t>::max()); 

    keyHash nouveau; 
    nouveau.l = dist(moteur); 
    nouveau.r = dist(moteur); 

    return nouveau; 
}

keyHash compute_cand_hash(const std::vector<int>& cand, const std::vector<keyHash>& node_to_hash) {

    keyHash base; 
    base.l = 0; // we create a hash with value 0 at the beginning
    base.r = 0; 

    for(const int c : cand) { // we XOR all the candidates together 
        base = base ^ node_to_hash[c]; 
    }

    return base; // at the end, base is the XOR of all the candidates 
}

/* END CANDIDATES KEYHASH SECTION */

/* BEGIN DATA SECTION */

Data::Data(const std::string& file) {

    instance_name = getFileName(file); 

    std::ifstream f(file); 
    
    if(!f.is_open()) 
        throw std::runtime_error("Couldn't open file " + file); 
    
    f >> dag_size >> nb_arcs >> degenerascy; 
    
    dag.resize(dag_size);  
    reverse_dag.resize(dag_size); 

    for(int i = 0; i < nb_arcs; ++i) {

        int from, to; 
        f >> from >> to; 
        dag[from].push_back(to);
        reverse_dag[to].push_back(from); 

    }

    this->s = 0; 
    this->t = this->dag_size-1; 

    compute_node_to_hash(); // compute hashes 
    compute_transitive_closure(); 

}

Data::Data(const std::vector<std::vector<int>>& _dag): dag(_dag) 
{
    this->dag_size = (int)_dag.size(); 
    int nb_arcs = 0; 
    for(int i = 0; i < dag_size; ++i) 
        nb_arcs += (int)_dag[i].size();
    this-> nb_arcs = nb_arcs; 
    
    // compute reverse dag 
    this->reverse_dag.resize(dag_size); 
    for(int i = 0; i < dag_size; ++i) {
        for(int u : _dag[i])
            this->reverse_dag[u].push_back(i); 
    }

    this->s = 0; 
    this->t = this->dag_size-1; 
        
    compute_node_to_hash(); 
    compute_transitive_closure(); 
}

void Data::compute_transitive_closure() { 

    std::vector<std::vector<bool>> trans_closure(dag_size, 
        std::vector<bool>(dag_size,0)); // init trans closure all to 0
    
    // we copy the graph relations 
    for(int i = 0; i < dag_size; ++i) {
        for(int j : dag[i]) {
            trans_closure[i][j] = true; 
        }
    }
    
    for(int i = 0; i < dag_size; ++i) 
        trans_closure[i][i] = true; // the diagonal is 1

    
    for(int k = 0; k < dag_size; ++k) {
        for(int i = 0; i < dag_size; ++i) {
            for(int j = 0; j < dag_size; ++j) {

                // if i->k & k->j then i->j
                if(trans_closure[i][k] && trans_closure[k][j])
                    trans_closure[i][j] = true; 

            }
        }
    }

    this->TC = trans_closure; 
}

void Data::compute_node_to_hash() {

    std::vector<keyHash> nodeToHash(dag_size); 

    std::random_device rd; // random seed
    std::mt19937_64 moteur(rd()); // random generator: quickly produces a 64-bit encoded number 

    for(int i = 0; i < dag_size; ++i) { // for each vertex 
        nodeToHash[i] = randomGenerator(moteur); // randomly generate its hash 
    }

    node_to_hash = nodeToHash; // store as a class attribute 

}

void Data::display_dag() const {

    for(int i = 0; i < (int)dag.size(); ++i) {
        std::cout << i << " -> "; 
        for(int j = 0; j < (int)dag[i].size(); ++j) {
            std::cout << dag[i][j] << " "; 
        }
        std::cout << std::endl;
    }

}

void Data::display_reverse_dag() const {

    for(int i = 0; i < (int)reverse_dag.size(); ++i) {
        std::cout << i << " -> "; 
        for(int j = 0; j < (int)reverse_dag[i].size(); ++j) {
            std::cout << reverse_dag[i][j] << " "; 
        }
        std::cout << std::endl;
    }

}