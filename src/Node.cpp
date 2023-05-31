#include "Node.hpp"
#include <tuple>
using namespace std;

Node::Node(const string& identifier, bool isAnchoringNode, bool isReverse, tuple<int,int,int> seqTuple) {
	this->identifier = identifier;
	this->isAnchoringNode = isAnchoringNode;
	this->isReverse = isReverse;
	this->seqTuple = seqTuple;
}

void Node::addChild(Node* child, int overlapScore, tuple<int, int, int> overlap) {
	children.push_back(std::make_tuple(child, overlapScore, overlap));
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

