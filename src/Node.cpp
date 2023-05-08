#include "Node.hpp"
using namespace std;

Node::Node(const string& identifier, bool isAnchoringNode) {
	this->identifier = identifier;
	this->isAnchoringNode = isAnchoringNode;
}

void Node::addChild(Node* child, int overlapScore) {
	children.emplace_back(child, overlapScore);  //moguci error zbog vector, pair
}

