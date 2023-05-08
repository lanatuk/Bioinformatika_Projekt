#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include "Node.hpp"

unsing namespace std;

bool comparePairs(const pair<Node*, int>& a, const pair<Node*, int>& b) {
	return a.second > b.second;
}


//with recursion
vector <vector<Node*>> constructPaths(Node* node) {
	vector<vector<Node*>> paths;
	
	//
	if(node->children.empty()) {
		paths.emplace_back(vector<Node*>{node}); //creates new path vector and adds it to the 'paths' vector of vectors
		//initializes a new vector with single element 'node'
		return paths;
		
	}
	
	vector<pair<Node*, int>> sortedChildren = node->children;
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
		path.push_back(path);
	}
	
	return paths;
	
}

//without recursion
vector<vector<Node*>> constructPaths(Node* startNode) {
    std::vector<std::vector<Node*>> paths;
    std::vector<Node*> currentPath;
    currentPath.push_back(startNode);

    while (!currentPath.empty()) {
        Node* currentNode = currentPath.back();
        const std::vector<std::pair<Node*, int>>& children = currentNode->children;


        if (children.empty()) {
            paths.push_back(currentPath);
            currentPath.pop_back();
        } else {
            std::vector<std::pair<Node*, int>> sortedChildren(children);
            std::sort(sortedChildren.begin(), sortedChildren.end(), comparePairs);
				
			Node* bestChild = sortedChildren[0].first; // Initialize with the first child

			while (!sortedChildren.empty()) {
				bestChild = sortedChildren[0].first;
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

int main(){
	
	unordered_map<string, Node*> nodes;
	
	nodes["ctg1"] = new Node("ctg1", true);
	nodes["read1"] = new Node("read1", false);
	nodes["read2"] = new Node("read2", false);
	
	nodes["ctg1"]->addChild(nodes["read1"], 6);
	nodes["ctg1"]->addChild(nodes["read2"], 8);
	
	nodes["ctg2"] = new Node("ctg2", true);
	nodes["read1"] = new Node("read1", false);
	nodes["read3"] = new Node("read3", false);
	
	nodes["ctg1"]->addChild(nodes["read1"], 6);
	nodes["ctg1"]->addChild(nodes["read2"], 8);
	
	
	vector<vector<Node*>> paths;
	
	for (const auto& node : nodes) {
		if(node.second->isAnchoringNode) {
			vector<vector<Node*>> nodePaths = constructPaths(node.second);
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