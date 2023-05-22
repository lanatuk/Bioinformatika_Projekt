#include <vector>
#include <list>
#include <fstream>
#include <sstream>
using namespace std;

class Node {
	public: 
		Node(const string& identifier, bool isAnchoringNode);
		void addChild(Node* child, int overlapScore);
		//void deleteChild(std::vector<std::pair<Node*, int> >::iterator it);
		void deleteChild(int index);
		
		string identifier;
		bool isAnchoringNode;
		vector<pair<Node*, int > > children;
};