#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <algorithm>
#include "Node.hpp"

using namespace std;

bool comparePairs(const tuple<Node*, int>& a, const tuple<Node*, int>& b) {
	return get<1>(a) > get<1>(b);
}

/*
//with recursion
vector <vector<Node*> > constructPaths(Node* node) {
	vector<vector<Node*> > paths;
	
	//
	if(node->children.empty()) {
		paths.emplace_back(vector<Node*>{node}); //creates new path vector and adds it to the 'paths' vector of vectors
		//initializes a new vector with single element 'node'
		return paths;
		
	}
	
	vector<pair<Node*, int> > sortedChildren = node->children;
	sort(sortedChildren.begin(), sortedChildren.end(), comparePairs);  //moguci error zbog arg funkcije
	
	Node* bestChild = nullptr;
	for(const auto& child : sortedChildren) {
		if(!child.first->isAnchoringNode) {
			bestChild = child.first;
			break;
		}
	}
	
	//if it reached anchoring node -> finish
	if(bestChild == nullptr) {
		paths.emplace_back(vector<Node*>{node});
		return paths;
	}
	
	vector<vector<Node*>> childPaths = constructPaths(bestChild);
	
	for(auto& path : childPaths) {
		path.insert(path.begin(), node);
		// tu neki error javlja
		//path.push_back(path);
	}
	
	return paths;
	
}
*/



//without recursion
vector<vector<Node*> > constructPaths(Node* startNode) {
    vector<vector<Node*> > paths;
    vector<Node*> currentPath;
    currentPath.push_back(startNode);

    while (!currentPath.empty()) {
        Node* currentNode = currentPath.back();
        const vector<tuple<Node*, int> >& children = currentNode->children;

        if (children.empty()) {
            paths.push_back(currentPath);
            currentPath.pop_back();
			if(!currentPath.empty()){
				Node* oldCurrentNode = currentPath.back();
        		const vector<tuple<Node*, int> >& oldChildren = oldCurrentNode->children;
				vector<tuple<Node*, int> > oldSortedChildren(oldChildren);
            	sort(oldSortedChildren.begin(), oldSortedChildren.end(), comparePairs);
				if (!oldSortedChildren.empty()) {
					Node* bestChildE = get<0>(oldSortedChildren[0]);
            		
					int index = -1;
            		for (int i = 0; i < oldChildren.size(); ++i) {
						if (get<0>(oldChildren[i]) == bestChildE) {
							index = i;
							break;
						}
            		}
					if (index != -1) {
						oldCurrentNode->deleteChild(index);
					}
					/*
					Node* bestChildE = oldSortedChildren[0].first;
					cout << "ovdje" <<endl;
					cout << bestChildE->identifier<<endl;
					//std::pair<Node*, int> toErase = oldSortedChildren[0];
					oldCurrentNode->deleteChild(oldSortedChildren.begin());
					//oldChildren.erase(oldSortedChildren.begin());
					*/
				}
			}

        } else {
            vector<tuple<Node*, int> > sortedChildren(children);
            sort(sortedChildren.begin(), sortedChildren.end(), comparePairs);
				
			Node* bestChild = get<0>(sortedChildren[0]); // Initialize with the first child

			while (!sortedChildren.empty()) {
				bestChild = get<0>(sortedChildren[0]);
				// Check if the best child is already in the current path
				bool childExistsInPath = false;
				for (Node* node : currentPath) {
					if (node == bestChild) {
						childExistsInPath = true;
						sortedChildren.erase(sortedChildren.begin());
						break;
					}
				
				}
				if (childExistsInPath == false) {
					break;
				}
			
			}
			// Check if the best child is already in the current path
            /*bool childExistsInPath = false;
            for (Node* node : currentPath) {
                if (node == bestChild) {
                    childExistsInPath = true;
                    break;
                }
            }*/

            /*if (childExistsInPath) {
                paths.push_back(currentPath);
                currentPath.pop_back();
            } else {
            currentPath.push_back(bestChild);
            }*/
			
			if (bestChild->isAnchoringNode) {
				currentPath.push_back(bestChild);
				paths.push_back(currentPath);
				return paths;
			}
			
			if (sortedChildren.empty()) {
				paths.push_back(currentPath);
				currentPath.pop_back();
			} else {
				currentPath.push_back(bestChild);
            }
        }
    }

    return paths;
}

//Read PAF file
vector<tuple<string, string, int, string, string>> readPAF(const string& filePath) {
    vector<tuple<string, string, int, string, string>> overlaps;
    ifstream file(filePath);
    string line;
    while (getline(file, line)) {
        stringstream ss(line);
        string field;
        vector<string> fields;
        while (getline(ss, field, '\t')) {
            fields.push_back(field);
        }
        string queryId = fields[0];
        string targetId = fields[5];
        int overlapScore = stoi(fields[10]);
		string sign = fields[4];
		
		int queryStart = stoi(fields[2]);
		
		int targetStart = stoi(fields[7]);
		
		string direction;
		//check overlaps: right or left		
		if(sign == "+") {
			if(queryStart <= targetStart) {
				direction = "left";
			} else {
				direction = "right";
			}
		} else if (sign == "-") {
			//dodati za reverzni komplement
			//samo znak >= za reverzni
		}
		
        overlaps.emplace_back(queryId, targetId, overlapScore, sign, direction);
    }
    return overlaps;
}

int main(int argc, char* argv[]){
	
	string fileContigs;
	string fileReads;
	
	if (argc > 2) {
		fileContigs = argv[1];
		fileReads = argv[2];
	} else {
		cout << "Number of file paths is not right!" << endl;
		exit(1);
	}	
	
	
	vector<tuple<string, string, int, string, string>> contigReadOverlaps = readPAF(fileContigs);
	vector<tuple<string, string, int, string, string>> readReadOverlaps = readPAF(fileReads);
		
	unordered_map<string, Node*> nodes;
	

	cout << "before contig-read" << endl;
	//Going over contig-read overlaps
	for (const auto& overlap : contigReadOverlaps) {
        string queryId = get<0>(overlap);
        string targetId = get<1>(overlap);
        int overlapScore = get<2>(overlap);
        string sign = get<3>(overlap);
		string direction = get<4>(overlap);

		//Set nodes
		//dodati i za komplementarne cvorove jos 2 cvora
        if (nodes.find(queryId) == nodes.end()) {
            nodes[queryId] = new Node(queryId, false);
        }
        if (nodes.find(targetId) == nodes.end()) {
            nodes[targetId] = new Node(targetId, true);
        }

        //Add child based on sign and direction
		if (sign == "+") {
			if (direction == "left") {
				nodes[targetId]->addChild(nodes[queryId], overlapScore);
			} else {
				nodes[queryId]->addChild(nodes[targetId], overlapScore);
			}
		}
        //dodati za reverzni komplement
		
		
    }
	
	cout << "before read-read" << endl;
	//Going over read-read overlaps
	for (const auto& overlap : readReadOverlaps) {
        string queryId = get<0>(overlap);
        string targetId = get<1>(overlap);
        int overlapScore = get<2>(overlap);
        string sign = get<3>(overlap);
		string direction = get<4>(overlap);
		
		//Set nodes
		//dodati i za komplementarne cvorove jos 2 cvora
        if (nodes.find(queryId) == nodes.end()) {
            nodes[queryId] = new Node(queryId, false);
        }
        if (nodes.find(targetId) == nodes.end()) {
            nodes[targetId] = new Node(targetId, false);
        }

        //Add child
        //Add child based on sign and direction
		if (sign == "+") {
			if (direction == "left") {
				nodes[targetId]->addChild(nodes[queryId], overlapScore);
			} else {
				nodes[queryId]->addChild(nodes[targetId], overlapScore);
			}
		}
		//dodati za reverzni komplement
		
		
    }
	
	
	/*
	nodes["ctg1"] = new Node("ctg1", true);
	nodes["read1"] = new Node("read1", false);
	nodes["read2"] = new Node("read2", false);
	
	nodes["ctg1"]->addChild(nodes["read1"], 9);
	nodes["ctg1"]->addChild(nodes["read2"], 8);
	
	nodes["ctg2"] = new Node("ctg2", true);
	nodes["read1"] = new Node("read1", false);
	nodes["read3"] = new Node("read3", false);

	nodes["read3"]->addChild(nodes["ctg2"], 5);
	//nodes["read1"]->addChild(nodes["ctg2"], 7);

	nodes["read2"]->addChild(nodes["read3"], 8);
	//nodes["read1"]->addChild(nodes["read3"], 4);
	
	//nodes["ctg2"]->addChild(nodes["read1"], 6);
	//nodes["ctg2"]->addChild(nodes["read3"], 8);
	*/
	
	cout << "before paths" << endl;
	vector<vector<Node*> > paths;
	
	for (const auto& node : nodes) {
		if(node.second->isAnchoringNode) {
			//cout << node.first<<endl;
			vector<vector<Node*> > nodePaths = constructPaths(node.second);
			paths.insert(paths.end(), nodePaths.begin(), nodePaths.end());
		}
	}
	
	
	//print paths
	for(const auto& path : paths) {
		for(const auto& node : path) {
			cout << node->identifier << " ";
		}
		cout << endl;
	}
	
	for(const auto& node : nodes) {
		delete node.second;
	}
	
	return 0;
}