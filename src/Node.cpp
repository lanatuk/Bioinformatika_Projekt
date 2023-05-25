#include "Node.hpp"
#include <tuple>
using namespace std;

Node::Node(const string& identifier, bool isAnchoringNode, bool isReverse) {
	this->identifier = identifier;
	this->isAnchoringNode = isAnchoringNode;
	this->isReverse = isReverse;
}

void Node::addChild(Node* child, int overlapScore) {
	children.push_back(std::make_tuple(child, overlapScore));
	//children.emplace_back(child, overlapScore);  //moguci error zbog vector, pair
}
void Node::deleteChild(int index) {
    if (index >= 0 && index < children.size()) {
        children.erase(children.begin() + index);
    }
}
/*
void Node::deleteChild(std::vector<std::pair<Node*, int> >::iterator it) {
    children.erase(it);
}
void Node::deleteChild(Node* child, int overlapScore){
	children.erase(std::make_pair(child, overlapScore));
}*/

