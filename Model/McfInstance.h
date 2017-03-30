//
// Created by gastonrqn on 28/03/17.
//

#ifndef TESTCPLEXMCF_MCFINSTANCE_H
#define TESTCPLEXMCF_MCFINSTANCE_H

#include "McfVariableSet.h"
#include "McfNetworkLink.h"
#include <vector>
#include <istream>

class McfInstance {
public:
    int NodesCount;

    vector<McfNetworkLink> Links;

    vector< vector<int> > InLinksForNode;

    vector< vector<int> > OutLinksForNode;

    vector<int> FlowsCapacities;

    vector< vector<int> > LinkIndexes;

    McfVariableSet VariableSet;

    McfInstance();

    friend istream& operator>>(istream& str, McfInstance& instance);
};


#endif //TESTCPLEXMCF_MCFINSTANCE_H
