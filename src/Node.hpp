#include <vector>
#include <list>
#include <fstream>
#include <sstream>
#include <tuple>
using namespace std;

class Node {
	public: 
		Node(const string& identifier, bool isAnchoringNode, bool isReverse, tuple<int,int,int> seqTuple);
		void addChild(Node* child, int overlapScore, tuple<int, int, int> overlap);
		//void deleteChild(std::vector<std::pair<Node*, int> >::iterator it);
		void deleteChild(int index);
		
		string identifier;
		bool isAnchoringNode;
		bool isReverse;
		tuple<int,int,int> seqTuple;
		vector<tuple<Node*, int, tuple<int, int, int>> > children;
};