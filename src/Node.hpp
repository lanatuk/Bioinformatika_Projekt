#include <vector>

using namespace std;

class Node {
	public: 
		Node(const string& identifier, bool isAnchoringNode);
		void addChild(Node* child, int overlapScore);
		
		string identifier;
		bool isAnchoringNode;
		vector<pair<Node*, int >> children;
};