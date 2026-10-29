# 1. Configuration variables
CXX = g++
CXXFLAGS = -O3 -g -std=c++17 -Wall -Wextra 

# --- CRUCIAL ADDITION: Indicate the folders containing the .hpp files ---
CXXFLAGS += -Isrc/common -Isrc/fpt -Isrc/milp -Isrc/meta 

# --- SYSTEM AND GUROBI MODULE DETECTION ---
UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S), Darwin)
    GUROBI_DIR = /Library/gurobi1302/macos_universal2
    GUROBI_INC = -I$(GUROBI_DIR)/include
    GUROBI_LIB = -L$(GUROBI_DIR)/lib -lgurobi_c++ -lgurobi130 -Wl,-rpath,$(GUROBI_DIR)/lib
else
    GUROBI_DIR = $(GUROBI_HOME)
    GUROBI_INC = -I$(GUROBI_DIR)/include
    GUROBI_LIB = -L$(GUROBI_DIR)/lib -lgurobi_c++ -lgurobi130 -Wl,-rpath,$(GUROBI_DIR)/lib
endif

CXXFLAGS += $(GUROBI_INC)
# --- END GUROBI CONFIGURATION ---

# A single unified executable
EXEC_CPP = prog

# 2. Strict separation with full paths
SRC_COMMON = src/common/Common.cpp \
             src/common/Data.cpp \
             src/common/Timer.cpp \
             src/common/ResultsLogs.cpp \
             src/common/DagSumCut.cpp \
             src/common/DagCutwidth.cpp \
             src/fpt/State_graph.cpp \
             src/fpt/FPT_solver.cpp \
             src/common/PreTraitement.cpp \
             src/common/Master.cpp \
             src/milp/DSC1.cpp \
             src/milp/DSC2.cpp \
             src/milp/DSC3.cpp \
             src/milp/CW2.cpp \
             src/milp/CW3.cpp

SRC_MAIN_CPP = src/main.cpp

# 3. Transformation of .cpp into .o
OBJ_COMMON   = $(SRC_COMMON:.cpp=.o)
OBJ_MAIN_CPP = $(SRC_MAIN_CPP:.cpp=.o)

ALL_OBJ = $(OBJ_COMMON) $(OBJ_MAIN_CPP)

# 4. Build rules
all: $(EXEC_CPP)

# Unified main program (FPT + Gurobi)
$(EXEC_CPP): $(OBJ_MAIN_CPP) $(OBJ_COMMON)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(GUROBI_LIB)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# 5. Cleanup
clean:
	rm -f $(ALL_OBJ) $(EXEC_CPP) gurobi.log

fclean: clean
	rm -f $(EXEC_CPP) gurobi.log

re: fclean all

.PHONY: all clean fclean re