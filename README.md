# Exact Algorithms for Directed Sum Cut and Directed Cutwidth

Source code of the implementation associated with the paper submitted to the MISTA 2027 conference by Matilin Periat, Alix Munier-Kordon, and Claire Hanen.

## 1. Introduction

This repository hosts the source code for our research project focused on optimizing topological orders in Directed Acyclic Graphs (DAGs). The main objective is to solve the **Directed Sum Cut (DSC)** and **Directed Cutwidth (DCW)** problems. We provide a comprehensive benchmark comparing Integer Linear Programming (ILP) models solved with Gurobi against a dynamic programming Fixed-Parameter Tractable (FPT) algorithm.

## 2. Formal Problem Definitions

Let $G = (V, A)$ be a Directed Acyclic Graph (DAG) with $n = |V|$ vertices. 
A **topological order** $\varphi$ defines a total order on the set of vertices $V$ such that for any arc $(u, v) \in A$, we have $\varphi(u) < \varphi(v)$.

For a given topological order $\varphi$, we define the **cut** at order $i$ (with $1 \le i \le n$) as the set of vertices placed at or before position $i$ that have at least one successor strictly after $i$:

$$ \Delta(\varphi, i) = \{u \in V : \varphi(u) \le i \wedge (\exists v \in V : \varphi(v) > i \wedge (u,v) \in A)\} $$
$$ \Theta(\varphi, i) = \{(u,v)\in A : \varphi(u) \le i \wedge \varphi(v) > i\} $$

-   The **Directed Sum Cut (DSC)** problem consists of finding a topological order $\varphi^*$ that minimizes the sum of these cuts over all positions:
    $$ \min_{\varphi} \sum_{i \in [|V|]} |\Delta(\varphi, i)| $$

-   The **Directed Cutwidth (DCW)** problem consists of finding a topological order $\varphi^*$ that minimizes the maximum cut size over all positions:
    $$ \min_{\varphi} \max_{i \in [|V|]} |\Theta(\varphi,i)| $$

**Degeneracy Parameter ($k$):**
The complexity of our exact dynamic programming approach is expressed as a function of the degeneracy $k$ of the co-comparability graph $\mathcal{H}_G$ associated with $G$. Two vertices are connected in $\mathcal{H}_G$ if they are incomparable in the transitive closure of $G$. The number of candidate sets is bounded by $\mathcal{O}(n2^k)$, which guarantees the FPT complexity of the algorithm.

## 3. Algorithms and Complexity

This project contrasts two exact resolution paradigms:

-   **FPT Approach (Dynamic Programming):** The algorithm explores a state graph ($\mathcal{SG}_G$) where each node represents a valid candidate set of vertices. The implementation features a high-performance 128-bit Zobrist-inspired hashing mechanism to store and identify states while preventing collisions. We also utilize a preprocessing phase based on weakly connected components decomposition, which massively reduces the search space for sparse graphs.
-   **ILP Approach (Gurobi):** Several mathematical models have been implemented for both DSC and DCW. These include models using absolute positions (e.g., DSC1, CW2) and relative precedence variables (e.g., DSC2). 

## 4. Project Architecture

The C++ source code is structured as follows:

    📂 Project Root
    ├── 📄 Makefile                  # Automated build script configured for Gurobi
    ├── 📄 Stats.py                  # Python script (Matplotlib/Seaborn) for generating publication-ready charts
    ├── 📂 src/
    │   ├── 📂 common/
    │   │   ├── 📄 Common.hpp/.cpp   # Shared tools, hashing logic, and helper functions
    │   │   ├── 📄 Data.hpp/.cpp     # Instance loading, transitive closure computation, and hash initialization
    │   │   ├── 📄 Timer.hpp/.cpp    # Precise execution time measurement
    │   │   ├── 📄 ResultsLogs.hpp/.cpp # Logging and formatting output results
    │   │   ├── 📄 Criteres.hpp      # Abstract base class for optimization criteria
    │   │   ├── 📄 DagSumCut.hpp/.cpp # Evaluation criteria for the DSC problem
    │   │   ├── 📄 DagCutwidth.hpp/.cpp # Evaluation criteria for the DCW problem
    │   │   ├── 📄 PreTraitement.hpp/.cpp # Weakly connected components decomposition for FPT
    │   │   └── 📄 Master.hpp/.cpp   # Main orchestrator executing the selected algorithm
    │   ├── 📂 fpt/
    │   │   ├── 📄 State_graph.hpp/.cpp # State graph modeling, 128-bit hashing, ID and collision management
    │   │   └── 📄 FPT_solver.hpp/.cpp # Core dynamic programming algorithm generating the state graph
    │   ├── 📂 milp/                 # Gurobi ILP formulations
    │   │   ├── 📄 Milp.hpp          # Abstract base class for ILP models
    │   │   ├── 📄 DSC1.hpp/.cpp     # ILP Model 1 for DSC (Absolute positions)
    │   │   ├── 📄 DSC2.hpp/.cpp     # ILP Model 2 for DSC (Relative precedence)
    │   │   ├── 📄 DSC3.hpp/.cpp     # ILP Model 3 for DSC 
    │   │   ├── 📄 CW2.hpp/.cpp      # ILP Model 2 for DCW
    │   │   └── 📄 CW3.hpp/.cpp      # ILP Model 3 for DCW
    │   └── 📄 main.cpp              # Unified entry point for all algorithms
    └── 📂 results/                  # Directory where raw .txt results are written (must be created before running with write enabled)

## 5. Build and Execution

### Prerequisites
-   **C++17** compiler (`g++` or `clang++`).
-   **Gurobi Optimizer** (Version 13.0.2 or later). Ensure environment variables (e.g., `GUROBI_HOME`) are correctly configured in the `Makefile`.
-   **Python 3** with `pandas`, `seaborn`, and `matplotlib` (if you wish to use `Stats.py` to generate charts).

### Compilation
A unified `Makefile` is provided. Run the following command at the root of the project:

    make

This will generate a single executable named `prog`.

### Running the Tool

The main program requires exactly three command-line arguments:

    ./prog <path_to_instance> <algorithm_id> <write_results>

-   **`path_to_instance`**: The relative path to the text file encoding the input DAG (e.g., `instances/100_n_5_k_1_ID.txt`).
-   **`algorithm_id`**: The choice of algorithm and problem to evaluate:
    -   `0`: Standard FPT for DSC
    -   `1`: Standard FPT for DCW
    -   `2`: ILP Model DSC1
    -   `3`: ILP Model DSC2
    -   `4`: ILP Model DSC3
    -   `5`: ILP Model CW2
    -   `6`: ILP Model CW3
    -   `7`: FPT with Preprocessing for DSC
    -   `8`: FPT with Preprocessing for DCW
-   **`write_results`**: A boolean integer managing the export of metrics:
    -   `0`: Disabled (terminal output only).
    -   `1`: Enabled. Appends a line of data to the corresponding text file in the `results/` directory. *(Note: The `results/` directory must exist at the root before execution).*

**Execution Example:**
    ./prog instances/100_n_5_k_1_ID.txt 7 1
*(Runs the FPT solver with preprocessing on the DSC problem, and writes the results to `results/results_FPT_PT_DSC.txt`)*.

### Output Format

When writing is enabled (`1`), results are appended as raw text inside the `results/` directory. For standard FPT solvers without preprocessing, the format is:

`[Instance_Name] [Size_n] [Degeneracy_k] [Optimal_Value] [State_Graph_Nodes_Generated] [Time_Seconds]`

*Note: An Optimal Value of `-1` indicates that a complete solution could not be found within the internal time limit (set to 600 seconds)*.