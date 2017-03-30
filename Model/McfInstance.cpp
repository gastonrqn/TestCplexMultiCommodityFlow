//
// Created by gastonrqn on 28/03/17.
//

#include "McfInstance.h"

using namespace std;

McfInstance::McfInstance() {
    this->Links = vector<McfNetworkLink>(0);
    this->FlowsCapacities = vector<int>(0);
    this->LinkIndexes = vector<vector<int>>(0);
    this->InLinksForNode = vector<vector<int>>(0);
    this->OutLinksForNode = vector<vector<int>>(0);
    this->VariableSet = McfVariableSet();
}


// File Format: (Nota: Aristas 0 a k-1, corresponden a nodos de "sink"/"source")
// [Cant nodos n]
// [Cant flujos k]
// [Capacidades de los flujos (enteras) Ej: "1 3 4 5"]
// [Cant aristas m]
// [Arista 0, Ej: "0 1" es arista de nodo 0 a nodo 1] [Capacidad]
//  ...
// [Arista m-1] [Capacidad]
istream& operator>>(istream& str, McfInstance& instance){
    int amountOfLinks;
    int amountOfFlows;

    str >> instance.NodesCount;
    str >> amountOfFlows;

    instance.FlowsCapacities.resize(amountOfFlows, 0);
    for(int i=0; i<amountOfFlows; i++){
        str >> instance.FlowsCapacities[i];
    }

    int realAmountOfNodes = instance.NodesCount + amountOfFlows;
    instance.LinkIndexes.resize(realAmountOfNodes);
    instance.InLinksForNode.resize(realAmountOfNodes);
    instance.OutLinksForNode.resize(realAmountOfNodes);

    for(int i=0; i<instance.LinkIndexes.size(); i++){
        instance.LinkIndexes[i].resize(instance.LinkIndexes.size(), 0);
    }

    str >> amountOfLinks;
    for(int i=0; i<amountOfLinks; i++){
        McfNetworkLink l;
        str >> l.Origin;
        str >> l.Destination;
        str >> l.Capacity;
        instance.Links.push_back(l);

        instance.LinkIndexes[l.Origin][l.Destination] = instance.Links.size()-1;

        instance.InLinksForNode[l.Destination].push_back(l.Origin);
        instance.OutLinksForNode[l.Origin].push_back(l.Destination);
    }

    instance.VariableSet = McfVariableSet(instance.Links, amountOfFlows);

    /*std::cout <<  instance.FlowsCapacities[0] << std::endl;
    std::cout << instance.Links[0].Origin << " " << instance.Links[0].Destination << " " << instance.LinkIndexes[instance.Links[0].Origin][instance.Links[0].Destination] << std::endl;
    std::cout << instance.Links[1].Origin << " " << instance.Links[1].Destination << " " << instance.LinkIndexes[instance.Links[1].Origin][instance.Links[1].Destination] << std::endl;
    */
}