/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   GraphWithTarjanAlgorithmWithTarjanAlgorithm.h
 * Author: alessandro
 *
 * Created on March 12, 2017, 6:44 PM
 */

#ifndef GRAPHWITHTARJANALGORITHM_H
#define GRAPHWITHTARJANALGORITHM_H

// A C++ program to find strongly connected components in a given
// directed graph using Tarjan's algorithm (single DFS)
#include <iostream>
#include <list>
#include <stack>
#include <vector>
#include <string>
#define NIL -1
using namespace std;
 
struct Vertex
{
    int id;
    string name;
    vector <unsigned int> rules;
    Vertex():id(-1),name("Invalid vertex"){};
    Vertex(int _id,string name_):id(_id),name(name_){};
    void addRule (int ruleID){
        rules.push_back(ruleID);
    }
};

// A class that represents an directed graph
class GraphWithTarjanAlgorithm
{
    vector< list<int>  > adj;  
 
    // A Recursive DFS based function used by SCC()
    void SCCUtil(int u, int disc[], int low[],
                 stack<int>& st, bool stackMember[], vector< vector<int> >& scc);
public:
    GraphWithTarjanAlgorithm(){}
    void addEdge(unsigned int v, unsigned int w);   // function to add an edge to graph
    const vector< vector <int> > SCC() ;    // prints strongly connected components
};



#endif /* GRAPHWITHTARJANALGORITHM_H */

