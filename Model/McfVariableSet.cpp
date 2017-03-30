//
// Created by gastonrqn on 28/03/17.
//

#include "McfVariableSet.h"

using namespace std;

McfVariableSet::McfVariableSet(){}

McfVariableSet::McfVariableSet(vector<McfNetworkLink> links, int flowsCount) {
    this->_linksCount = links.size();
    this->_links = links;
    this->_flowsCount = flowsCount;
    this->_variablesCount = links.size()*flowsCount + 1;
}

string McfVariableSet::Name(int index) const {
    if(index != _variablesCount - 1)
    {
        int flowNumber = index / this->_linksCount;
        int linkIndex = index % this->_linksCount;

        McfNetworkLink l = this->_links[linkIndex];
        return "x_" + to_string(flowNumber) + "_(" + to_string(l.Origin) + "," + to_string(l.Destination) + ")";
    }
    else
    {
        return "U_max"; // variable de minimización (por ahora no tiene restricciones, o sea que siempre vale 0)
    }
}

int McfVariableSet::Index(int linkNumber, int flowNumber) const {
    // [Links flow 0] [Links flow 1] ... [Links flow k-1]
    return (flowNumber * this->_linksCount) + linkNumber;
}

int McfVariableSet::VariablesCount() const {
    return _variablesCount;
}
