//
// Created by gastonrqn on 28/03/17.
//

#ifndef TESTCPLEXMCF_MCFINSTANCESOLVER_H
#define TESTCPLEXMCF_MCFINSTANCESOLVER_H


#include "McfInstance.h"
#include <ilcplex/ilocplex.h>
#include <ilcplex/cplex.h>
#include <string>
#include <vector>
#include <iostream>

#define TOL 1E-05
#define CPLEX_EXECUTION_TIMELIMIT 3600
#define OUT_LP_FILE "mcf.lp"
#define OUT_SOL_FILE "mcf.sol"

class McfInstanceSolver {
public:
    McfInstanceSolver(McfInstance instance);

    void Solve();

private:
    McfInstance _instance;

    void GenerateLinkCapacityConstraints(CPXENVptr &env, CPXLPptr &lp);

    void GenerateFlowConservTransitNodesConstraints(CPXENVptr &env, CPXLPptr &lp);

    void GenerateFlowExitsSourcesConstraints(CPXENVptr &env, CPXLPptr &lp);

    void GenerateFlowEntersSinksConstraints(CPXENVptr &env, CPXLPptr &lp);
};


#endif //TESTCPLEXMCF_MCFINSTANCESOLVER_H
