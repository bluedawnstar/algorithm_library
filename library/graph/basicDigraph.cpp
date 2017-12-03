#include <climits>
#include <numeric>
#include <queue>
#include <algorithm>
#include <vector>

using namespace std;

#include "basicDigraph.h"
#include "cycleDigraph.h"

  /////////// For Testing ///////////////////////////////////////////////////////

#include <cassert>
#include <string>
#include <iostream>
#include "../common/iostreamhelper.h"

static BasicDigraph buildDigraph(bool isCycle) {
    BasicDigraph graph(6);

    graph.addEdge(5, 2);
    graph.addEdge(5, 0);
    graph.addEdge(4, 0);
    graph.addEdge(4, 1);
    graph.addEdge(2, 3);
    graph.addEdge(3, 1);

    if (isCycle)
        graph.addEdge(1, 2);

    return graph;
}


void testBasicDigraph() {
    //return; //TODO: if you want to test a split function, make this line a comment.

    cout << "-- Basic Digraph ---------" << endl;
    {
        BasicDigraph graph(7);

        graph.addEdge(0, 1);
        graph.addEdge(1, 2);
        graph.addEdge(1, 3);
        graph.addEdge(2, 3);
        graph.addEdge(3, 4);
        graph.addEdge(4, 6);
        graph.addEdge(5, 6);

        cout << graph.getShortestPath(0, 6) << endl;
    }
    {
        auto graph = buildDigraph(false);
        assert(graph.isCyclicGraphDFS() == false);

        graph = buildDigraph(true);
        assert(graph.isCyclicGraphDFS() == true);
    }
    cout << "OK" << endl;
}
