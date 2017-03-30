cmake_minimum_required(VERSION 3.6)
project(TestCplexMCF)

## Compilador y parámetros
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_COMPILER g++)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m64 -O2 -fPIC -fexceptions -DNDEBUG -DIL_STD")


## Librerias de CPLEX
include_directories($ENV{CPLEX_HOME}/cplex/include/)
include_directories($ENV{CPLEX_HOME}/concert/include)

add_library(libcplex STATIC IMPORTED)
set_property(TARGET libcplex PROPERTY IMPORTED_LOCATION "$ENV{CPLEX_HOME}/cplex/lib/x86-64_linux/static_pic/libcplex.a")

add_library(libilocplex STATIC IMPORTED)
set_property(TARGET libilocplex PROPERTY IMPORTED_LOCATION "$ENV{CPLEX_HOME}/cplex/lib/x86-64_linux/static_pic/libilocplex.a")


## Definición de Ejecutable
set(SOURCE_FILES main.cpp Model/McfInstance.cpp Model/McfInstance.h Model/McfNetworkLink.h Model/McfVariableSet.cpp Model/McfVariableSet.h Model/McfInstanceSolver.cpp Model/McfInstanceSolver.h)
add_executable(TestCplexMCF ${SOURCE_FILES})

## Linker
target_link_libraries(TestCplexMCF libcplex libilocplex c m pthread nsl)