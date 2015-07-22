#ifndef _GRAPH_H_
#define _GRAPH_H_

#include <vector>
#include <string>
#include <algorithm>

class Graph {
private:
	struct Node;
	
	struct Edge;
	
	std::vector<Edge> edges;
public:
	Node getNewNode(int, int, std::string);
	void createEdge(Node, Node);
	void deleteEdge(Edge);
};

#endif
