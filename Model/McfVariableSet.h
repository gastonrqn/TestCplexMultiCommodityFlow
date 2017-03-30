//
// Created by gastonrqn on 28/03/17.
//

#ifndef TESTCPLEXMCF_MCFVARIABLE_H
#define TESTCPLEXMCF_MCFVARIABLE_H

#include <string>
#include "McfNetworkLink.h"
#include <vector>

using namespace std;

class McfVariableSet {
public:
    McfVariableSet();

    McfVariableSet(vector<McfNetworkLink> links, int flowsCount);

    string Name(int index) const;

    int Index(int linkNumber, int flowNumber) const;

    int VariablesCount() const;

private:
    int _linksCount;
    int _flowsCount;
    int _variablesCount;
    vector<McfNetworkLink> _links;
};


#endif //TESTCPLEXMCF_MCFVARIABLE_H
