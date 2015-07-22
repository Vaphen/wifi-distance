#include "graph.h"

struct Graph::Node {
	int x = 0;
	int y = 0;
	std::string essid = "";
};
	
struct Graph::Edge {
	Node startNode;
	Node endNode;
	int weight = 0;
};
	
Graph::Node Graph::getNewNode(int x, int y, std::string essid) {
	Node newNode;
	newNode.x = x;
	newNode.y = y;
	newNode.essid = essid;
	return newNode;
}

void Graph::createEdge(Node startNode, Node endNode) {
	Edge newEdge;
	newEdge.startNode = startNode;
	newEdge.endNode = endNode;
	newEdge.weight = 0;
	edges.push_back(newEdge);
}

void Graph::deleteEdge(Edge removeEdge) {
	edges.erase(std::find(edges.begin(), edges.end(), removeEdge));
}
