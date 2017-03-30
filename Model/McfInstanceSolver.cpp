//
// Created by gastonrqn on 28/03/17.
//

#include "McfInstanceSolver.h"
#include <fstream>

using namespace std;

McfInstanceSolver::McfInstanceSolver(McfInstance instance) {
    this->_instance = instance;
}

void McfInstanceSolver::Solve() {
    int n = _instance.VariableSet.VariablesCount(); // cantidad de variables

    // Genero el problema de cplex.
    int status;
    CPXENVptr env; // Puntero al entorno.
    CPXLPptr lp; // Puntero al LP

    // Creo el entorno.
    env = CPXopenCPLEX(&status);

    if (env == NULL) {
        throw string("Error creando el entorno");
    }

    // Creo el LP.
    lp = CPXcreateprob(env, &status, "instancia multicomm-flow");


    if (lp == NULL) {
        throw string("Error creando el LP");
    }

    // Definimos las variables. No es obligatorio pasar los nombres de las variables, pero facilita el debug. La info es la siguiente:
    double *ub, *lb, *objfun; // Cota superior, cota inferior, coeficiente de la funcion objetivo.
    char *xctype, **colnames; // tipo de la variable (por ahora son siempre continuas), string con el nombre de la variable.
    ub = new double[n];
    lb = new double[n];
    objfun = new double[n];
    xctype = new char[n];
    colnames = new char*[n];

    for (int i = 0; i < n; i++) {
        ub[i] = CPX_INFBOUND;
        lb[i] = 0.0;
        objfun[i] = (i == n-1) ? 1.0 : 0.0;
        xctype[i] = (i == n-1) ? 'C' : 'B'; // 'C' es continua, 'B' binaria, 'I' Entera. Para LP (no enteros), este parametro tiene que pasarse como NULL. No lo vamos a usar por ahora.
        colnames[i] = new char[15];
        sprintf(colnames[i], _instance.VariableSet.Name(i).c_str());
    }

    // Agrego las columnas.
    status = CPXnewcols(env, lp, n, objfun, lb, ub, NULL, colnames);

    if (status) {
        throw string("Problema agregando las variables CPXnewcols");
    }

    // Libero las estructuras.
    for (int i = 0; i < n; i++) {
        delete[] colnames[i];
    }

    delete[] ub;
    delete[] lb;
    delete[] objfun;
    delete[] xctype;
    delete[] colnames;


    // CPLEX por defecto minimiza. Le cambiamos el sentido a la funcion objetivo si se quiere maximizar.
    // CPXchgobjsen(env, lp, CPX_MAX);

    GenerateLinkCapacityConstraints(env, lp);

    GenerateFlowConservTransitNodesConstraints(env, lp);

    GenerateFlowExitsSourcesConstraints(env, lp);

    GenerateFlowEntersSinksConstraints(env, lp);

    // Seteo de algunos parametros.
    // Para desactivar la salida poern CPX_OFF.
    status = CPXsetintparam(env, CPX_PARAM_SCRIND, CPX_ON);
    if (status) {
        throw string("Problema seteando SCRIND");
    }

    // Setea el tiempo limite de ejecucion.
    status = CPXsetdblparam(env, CPX_PARAM_TILIM, CPLEX_EXECUTION_TIMELIMIT);
    if (status) {
        throw string("Problema seteando el tiempo limite");
    }

    // Escribimos el problema a un archivo .lp.
    status = CPXwriteprob(env, lp, OUT_LP_FILE, NULL);
    if (status) {
        throw string("Problema escribiendo modelo");
    }

    // Tomamos el tiempo de resolucion utilizando CPXgettime.
    double inittime, endtime;
    CPXgettime(env, &inittime);

    // Optimizamos el problema.
    status = CPXlpopt(env, lp);

    CPXgettime(env, &endtime);

    if (status) {
        throw string("Problema optimizando CPLEX");
    }

    // Chequeamos el estado de la solucion.
    int solstat;
    char statstring[510];
    CPXCHARptr p;
    solstat = CPXgetstat(env, lp);
    p = CPXgetstatstring(env, solstat, statstring);
    string statstr(statstring);

    cout << endl << "Resultado de la optimizacion: " << statstring << endl;
    if(solstat!=CPX_STAT_OPTIMAL){
        return;
    }

    // valor funcion objetivo
    double objval;
    status = CPXgetobjval(env, lp, &objval);
    if (status) {
        throw string("Problema obteniendo valor de mejor solucion.");
    }
    cout << "Datos de la resolucion: " << "\t Obj=" << objval << "\t Time=" << (endtime - inittime) << endl;

    // Tomamos los valores de la solucion y los escribimos a un archivo.
    std::string outputfile = OUT_SOL_FILE;
    ofstream solfile(outputfile.c_str());

    // Tomamos los valores de todas las variables. Estan numeradas de 0 a n-1.
    double *sol = new double[n];
    status = CPXgetx(env, lp, sol, 0, n - 1);
    if (status) {
        throw string("Problema obteniendo la solucion del LP.");
    }

    // Solo escribimos las variables distintas de cero (tolerancia, 1E-05).
    solfile << "Status de la solucion: " << statstr << endl;
    for (int i = 0; i < n; i++) {
        if (sol[i] > TOL) {
            solfile << _instance.VariableSet.Name(i) << " = " << sol[i] << endl;
        }
    }

    delete [] sol;
    solfile.close();
}


void McfInstanceSolver::GenerateLinkCapacityConstraints(CPXENVptr &env, CPXLPptr &lp){
    // ccnt = numero nuevo de columnas en las restricciones.
    // rcnt = cuantas restricciones se estan agregando.
    int ccnt = 0;
    int rcnt = 1; // siempre agrego de a una
    char *sense = new char[rcnt]; // 1 restriccion
    double *rhs = new double[rcnt]; // Termino independiente de las restricciones.
    int *matbeg = new int[rcnt]; //Posicion en la que comienza cada restriccion en matind y matval.
    matbeg[0] = 0; // Como agrego de a 1 restriccion, se toma desde matbeg[0] hasta nzcnt-1 (como índices en matind y matval).
    char *rowname = new char[20];

    // # de coeficientes != 0 a ser agregados a la matriz. Solo se pasan los valores que no son cero.
    int nzcnt = _instance.FlowsCapacities.size();
    // Array con los indices de las variables con coeficientes != 0 en la desigualdad.
    int *matind = new int[nzcnt];
    // Array que en la posicion i tiene coeficiente ( != 0) de la variable matind[i] en la restriccion.
    double *matval = new double[nzcnt];
    for(int i=0; i<_instance.Links.size(); i++)
    {
        rhs[0] = _instance.Links[i].Capacity;
        for(int j=0; j<_instance.FlowsCapacities.size(); j++)
        {
            matind[j] = _instance.VariableSet.Index(i, j);
            matval[j] = _instance.FlowsCapacities[j];
        }
        sense[0] = 'L'; // <=
        sprintf(rowname, ("LinkCap-("+to_string(_instance.Links[i].Origin)+","+to_string(_instance.Links[i].Destination)+")").c_str());

        int status = CPXaddrows(env, lp, ccnt, rcnt, nzcnt, rhs, sense, matbeg, matind, matval, NULL, &rowname);

        if (status) {
            throw string("Problema agregando restricciones.");
        }
    }
    delete[] matind;
    delete[] matval;
    delete[] rhs;
    delete[] matbeg;
    delete[] sense;
    delete[] rowname;
}

void McfInstanceSolver::GenerateFlowConservTransitNodesConstraints(CPXENVptr &env, CPXLPptr &lp) {
    // ccnt = numero nuevo de columnas en las restricciones.
    // rcnt = cuantas restricciones se estan agregando.
    int ccnt = 0;
    int rcnt = 1; // siempre agrego de a una
    char *sense = new char[rcnt]; // 1 restriccion
    double *rhs = new double[rcnt]; // Termino independiente de las restricciones.
    int *matbeg = new int[rcnt]; //Posicion en la que comienza cada restriccion en matind y matval.
    matbeg[0] = 0; // Como agrego de a 1 restriccion, se toma desde matbeg[0] hasta nzcnt-1 (como índices en matind y matval).
    char *rowname = new char[20];

    // nodos 0..k-1 son sink/source. Nodos k..(n+k-1) son transit nodes
    for(int i=_instance.FlowsCapacities.size(); i<_instance.NodesCount+_instance.FlowsCapacities.size(); i++)
    {
        // # de coeficientes != 0 a ser agregados a la matriz. Solo se pasan los valores que no son cero.
        int nzcnt = _instance.InLinksForNode[i].size()+_instance.OutLinksForNode[i].size();
        // Array con los indices de las variables con coeficientes != 0 en la desigualdad.
        int *matind = new int[nzcnt];
        // Array que en la posicion i tiene coeficiente ( != 0) de la variable matind[i] en la restriccion.
        double *matval = new double[nzcnt];

        rhs[0] = 0;

        // Por cada flujo y por cada nodo interno hay una restricción
        for(int j=0; j<_instance.FlowsCapacities.size(); j++)
        {
            int index = 0;
            for(int w=0; w<_instance.InLinksForNode[i].size(); w++)
            {
                int linkNumber = _instance.LinkIndexes[_instance.InLinksForNode[i][w]][i];
                matind[index] = _instance.VariableSet.Index(linkNumber, j);
                matval[index] = 1;
                index++;
            }
            for(int w=0; w<_instance.OutLinksForNode[i].size(); w++)
            {
                int linkNumber = _instance.LinkIndexes[i][_instance.OutLinksForNode[i][w]];
                matind[index] = _instance.VariableSet.Index(linkNumber, j);
                matval[index] = -1;
                index++;
            }

            sense[0] = 'E'; // =
            sprintf(rowname, ("FlowCons-N"+to_string(i)+"F"+to_string(j)).c_str());

            int status = CPXaddrows(env, lp, ccnt, rcnt, nzcnt, rhs, sense, matbeg, matind, matval, NULL, &rowname);

            if (status) {
                throw string("Problema agregando restricciones.");
            }
        }
        delete[] matind;
        delete[] matval;
    }
    delete[] rhs;
    delete[] matbeg;
    delete[] sense;
    delete[] rowname;
}

void McfInstanceSolver::GenerateFlowExitsSourcesConstraints(CPXENVptr &env, CPXLPptr &lp) {
    // ccnt = numero nuevo de columnas en las restricciones.
    // rcnt = cuantas restricciones se estan agregando.
    int ccnt = 0;
    int rcnt = 1; // siempre agrego de a una
    char *sense = new char[rcnt]; // 1 restriccion
    double *rhs = new double[rcnt]; // Termino independiente de las restricciones.
    int *matbeg = new int[rcnt]; //Posicion en la que comienza cada restriccion en matind y matval.
    matbeg[0] = 0; // Como agrego de a 1 restriccion, se toma desde matbeg[0] hasta nzcnt-1 (como índices en matind y matval).
    char *rowname = new char[20];

    // nodos 0..k-1 son sink/source para el correspondiente flujo 0..k-1
    for(int i=0; i<_instance.FlowsCapacities.size(); i++)
    {
        // # de coeficientes != 0 a ser agregados a la matriz. Solo se pasan los valores que no son cero.
        int nzcnt = _instance.OutLinksForNode[i].size();
        // Array con los indices de las variables con coeficientes != 0 en la desigualdad.
        int *matind = new int[nzcnt];
        // Array que en la posicion i tiene coeficiente ( != 0) de la variable matind[i] en la restriccion.
        double *matval = new double[nzcnt];

        rhs[0] = 1;

        for(int w=0; w<_instance.OutLinksForNode[i].size(); w++)
        {
            int linkNumber = _instance.LinkIndexes[i][_instance.OutLinksForNode[i][w]];
            matind[w] = _instance.VariableSet.Index(linkNumber, i);
            matval[w] = 1;
        }
        sense[0] = 'E'; // =
        sprintf(rowname, ("ExitSour-F"+to_string(i)).c_str());

        int status = CPXaddrows(env, lp, ccnt, rcnt, nzcnt, rhs, sense, matbeg, matind, matval, NULL, &rowname);

        if (status) {
            throw string("Problema agregando restricciones.");
        }
        delete[] matind;
        delete[] matval;
    }
    delete[] rhs;
    delete[] matbeg;
    delete[] sense;
    delete[] rowname;
}

void McfInstanceSolver::GenerateFlowEntersSinksConstraints(CPXENVptr &env, CPXLPptr &lp) {
    // ccnt = numero nuevo de columnas en las restricciones.
    // rcnt = cuantas restricciones se estan agregando.
    int ccnt = 0;
    int rcnt = 1; // siempre agrego de a una
    char *sense = new char[rcnt]; // 1 restriccion
    double *rhs = new double[rcnt]; // Termino independiente de las restricciones.
    int *matbeg = new int[rcnt]; //Posicion en la que comienza cada restriccion en matind y matval.
    matbeg[0] = 0; // Como agrego de a 1 restriccion, se toma desde matbeg[0] hasta nzcnt-1 (como índices en matind y matval).
    char *rowname = new char[20];

    // nodos 0..k-1 son sink/source para el correspondiente flujo 0..k-1
    for(int i=0; i<_instance.FlowsCapacities.size(); i++)
    {
        // # de coeficientes != 0 a ser agregados a la matriz. Solo se pasan los valores que no son cero.
        int nzcnt = _instance.InLinksForNode[i].size();
        // Array con los indices de las variables con coeficientes != 0 en la desigualdad.
        int *matind = new int[nzcnt];
        // Array que en la posicion i tiene coeficiente ( != 0) de la variable matind[i] en la restriccion.
        double *matval = new double[nzcnt];

        rhs[0] = 1;

        for(int w=0; w<_instance.InLinksForNode[i].size(); w++)
        {
            int linkNumber = _instance.LinkIndexes[_instance.InLinksForNode[i][w]][i];
            matind[w] = _instance.VariableSet.Index(linkNumber, i);
            matval[w] = 1;
        }
        sense[0] = 'E'; // =
        sprintf(rowname, ("EnterSink-F"+to_string(i)).c_str());

        int status = CPXaddrows(env, lp, ccnt, rcnt, nzcnt, rhs, sense, matbeg, matind, matval, NULL, &rowname);

        if (status) {
            throw string("Problema agregando restricciones.");
        }
        delete[] matind;
        delete[] matval;
    }
    delete[] rhs;
    delete[] matbeg;
    delete[] sense;
    delete[] rowname;
}
