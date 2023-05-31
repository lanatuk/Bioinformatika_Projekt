#include <vector>
#include <list>
#include <fstream>
#include <sstream>
#include <tuple>
using namespace std;

class Node {
	public: 
		Node(const string& identifier, bool isAnchoringNode, bool isReverse, tuple<int,int,int> seqTuple, tuple<int,int,int> seqTuple2);
		void addChild(Node* child, int overlapScore, tuple<int, int, int> overlap, tuple<int, int, int> overlapParent);
		//void deleteChild(std::vector<std::pair<Node*, int> >::iterator it);
		void deleteChild(int index);
		
		string identifier;
		bool isAnchoringNode;
		bool isReverse;
		tuple<int,int,int> seqTuple;
		tuple<int,int,int> seqTuple2;
		vector<tuple<Node*, int, tuple<int, int, int>, tuple<int, int, int>> > children;
};