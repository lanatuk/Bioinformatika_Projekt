#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <algorithm>
#include "Node.hpp"
#include <stack>
#include <unordered_set>
#include <random>
#include <set>
#include <unistd.h>
#include <map>

using namespace std;

bool compareNodes(const Node* node1, const Node* node2) {
    return node1->identifier < node2->identifier;
}
bool compareNodesReversed(const Node* node1, const Node* node2) {
    return node1->identifier > node2->identifier;
}
bool comparePairs(const tuple<Node*, int>& a, const tuple<Node*, int>& b) {
	return get<1>(a) > get<1>(b);
}



size_t monteCarloSelection(const vector<double>& weights) {
    //Create a random number generator
    random_device rd;
    mt19937 gen(rd());

	
	//Create a discrete distribution based on the weights
	discrete_distribution<size_t> distance(weights.begin(), weights.end());

	//Generate a random index based on the weights
	size_t selectedIndex = distance(gen);
		
	return selectedIndex;
		

}

//without recursion
vector<vector<Node*> > constructPaths(Node* startNode, bool flagMonteCarlo, vector<Node*> contigs) {
    vector<vector<Node*> > paths;
    vector<Node*> currentPath;
	
	unordered_map<string, vector<Node*>> forRemove;
    currentPath.push_back(startNode);

    while (!currentPath.empty()) {
        Node* currentNode = currentPath.back();
        const vector<tuple<Node*, int> >& children = currentNode->children;

        if (children.empty()) {
            //paths.push_back(currentPath);
            currentPath.pop_back();
			if(!currentPath.empty()){
				Node* oldCurrentNode = currentPath.back();
        		const vector<tuple<Node*, int> >& oldChildren = oldCurrentNode->children;
				vector<tuple<Node*, int> > oldSortedChildren(oldChildren);
            	std::sort(oldSortedChildren.begin(), oldSortedChildren.end(), comparePairs);
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
					
				}
			}

        } else {
			vector<tuple<Node*, int>> sortedChildren;
			
			if (flagMonteCarlo) {
				vector<double> weights;
				for (const auto& child : children) {
					weights.push_back(get<1>(child));
				}
				
				//set<int> usedIndexes;
				for (int i = 0; i < children.size(); i++) {
					size_t selectedIndex = monteCarloSelection(weights);
					sortedChildren.push_back(children[selectedIndex]);
					weights.erase(weights.begin() + int(selectedIndex));
				}
				
			} else {
				sortedChildren = children;
				if (forRemove[currentNode->identifier].size() != 0){
					vector<tuple<Node*, int>> newSortedChildren;
					for (const auto& t : sortedChildren) {
						if (std::find_if(forRemove[currentNode->identifier].begin(), forRemove[currentNode->identifier].end(), [&](Node* node) {
        								return std::get<0>(t)->identifier == node->identifier;
    									}) == forRemove[currentNode->identifier].end()) {
							newSortedChildren.push_back(t);
						}
					}
					sortedChildren = newSortedChildren;
					
				}
				if(sortedChildren.size() != 0){
					std::sort(sortedChildren.begin(), sortedChildren.end(), comparePairs);
				}
				
			}
			Node* emptyNode = new Node("", false, false, make_tuple(0,0,0));
			Node* bestChild = emptyNode;
			if(sortedChildren.size() != 0){
				bestChild = get<0>(sortedChildren[0]); // Initialize with the first child
				while (!sortedChildren.empty()) {
					
					bestChild = get<0>(sortedChildren[0]);
					//std::cout << bestChild->identifier<< "  kod brisanja"<<endl;
					// Check if the best child is already in the current path
					bool childExistsInPath = false;
					for (Node* node : currentPath) {
						if (node == bestChild) {
							childExistsInPath = true;
							sortedChildren.erase(sortedChildren.begin());
							if (sortedChildren.empty()) {
								bestChild = emptyNode;
							}
							break;
						}
					
					}
					if (childExistsInPath == false) {
						break;
					}
				}
				//std::cout << bestChild->identifier<< "  best2"<<endl;
			}
			
			
			if (bestChild->isAnchoringNode && contigs[0]->identifier == bestChild->identifier) {
				//std::cout << bestChild->identifier << endl;
				currentPath.push_back(bestChild);
				paths.push_back(currentPath);
				
				contigs.erase(contigs.begin());
				if (contigs.size() == 0){
					return paths;
				}
				currentPath.clear();
				currentPath.push_back(bestChild); 
				forRemove.clear();
				//return paths;
			}else if(bestChild->isAnchoringNode && contigs[0]->identifier != bestChild->identifier){
				forRemove[currentNode->identifier].push_back(bestChild);
				
			}
			
			else if (sortedChildren.empty()) {
				//paths.push_back(currentPath);
				//moguci error
				Node* r = currentPath.back();
				currentPath.pop_back();
				Node* curr = currentPath.back();
				forRemove[curr->identifier].push_back(r);
				
			} else {
				currentPath.push_back(bestChild);
            }
        }
    }
	//vector<std::vector<Node*>> empty_result;
    return paths;
}

//Read PAF file
vector<tuple<string, string, int, string, string, tuple<int,int,int>, tuple<int,int,int>> > readPAF(const string& filePath, string scoreType) {
    vector<tuple<string, string, int, string, string, tuple<int,int,int>, tuple<int,int,int>> > overlaps;
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
        int overlapScore = stoi(fields[9]);
		//int alignmentLenght = stoi(fields[10]);
		string sign = fields[4];
		
		int queryLength = stoi(fields[1]);
		int queryStart = stoi(fields[2]);
		int queryEnd = stoi(fields[3]);
		
		int targetLength = stoi(fields[6]);
		int targetStart = stoi(fields[7]);
		int targetEnd = stoi(fields[8]);

		
		//int extensionScore = overlapScore + (targetLength-targetEnd)/2 - (targetStart+queryLength-queryEnd)/2;

		if(queryId == targetId){
			continue;
		}
		
		
		string direction;
		if(targetStart < (targetLength-targetEnd)){
				direction = "left";
		}else if(targetStart == (targetLength-targetEnd)){
			if(queryStart < (queryLength-queryEnd)){
					direction = "right";
			}else{
				direction = "left";
			}
		}else{
			direction = "right";
		}
		
		int extensionScore;
		if (direction == "right") {
			extensionScore = overlapScore + (targetLength-targetEnd)/2 - (targetStart+queryLength-queryEnd)/2;
		} else {
			extensionScore = overlapScore + (queryLength-queryEnd)/2 - (queryStart+targetLength-targetEnd)/2;
		}
		//check overlaps: right or left		
		/*if(sign == "+") {
			if(targetStart < (targetLenght-targetEnd)){
				direction = "left";
			}else if(targetStart == (targetLenght-targetEnd)){
				if(queryStart < (queryLenght-queryEnd)){
					direction = "right";
				}else{
					direction = "left";
				}
			}else{
				direction = "right";
			}
			
			if(queryStart <= targetStart) {
				direction = "left";
			} else {
				direction = "right";
			}
		} else if (sign == "-") {
			
			//continue;
			//dodati za reverzni komplement
			//samo znak >= za reverzni
		}*/
		
		int score;
		if(scoreType == "overlap") {
			score = overlapScore;
		} else {
			score = extensionScore;
		}
		
		tuple<int,int,int> queryTuple = make_tuple(queryStart, queryEnd, queryLength);
		tuple<int,int,int> targetTuple = make_tuple(targetStart, targetEnd, targetLength);
        overlaps.emplace_back(queryId, targetId, score, sign, direction, queryTuple, targetTuple);
    }
    return overlaps;
}


map<string, string> parseFastaToMap(const string& fileContigs) {
    ifstream inputFile(fileContigs);
    string line, sequence;
    map<string, string> sequenceMap;
    string currentHeader;
    
    if (!inputFile.is_open()) {
        std::cerr << "Failed to open the file." << std::endl;
        return sequenceMap;
    }
    
    while (getline(inputFile, line)) {
        if (line.empty()) {
            continue;  //skip empty lines
        } else if (line[0] == '>') {
            if (!sequence.empty()) {
                //process the previously read sequence
                sequenceMap[currentHeader] = sequence;
                sequence.clear();
            }
            
            //process the header line starting with '>'
            currentHeader = line.substr(1);
        } else {
            //append the sequence line to the existing sequence
            sequence += line;
        }
    }
    
    //process the last sequence in the file
    if (!sequence.empty()) {
        sequenceMap[currentHeader] = sequence;
    }
    
    inputFile.close();
    
    return sequenceMap;
}

string reverseComplement(string sequence) {
    string complement = "";
    
    for (char base : sequence) {
        switch (base) {
            case 'A':
                complement += 'T';
                break;
            case 'T':
                complement += 'A';
                break;
            case 'C':
                complement += 'G';
                break;
            case 'G':
                complement += 'C';
                break;
            default:
                break;
        }
    }
    
    reverse(complement.begin(), complement.end());
    return complement;
}

string makeSequenceFromPath(vector<vector<Node*>> paths, map<string,string> contigsMap, map<string,string> readsMap) {
	bool flagFirst = true;
	string finalSequence;
	vector<string> usedContigs;
 	for (const auto& path : paths) {
		for (Node* node : path) {
			string sequence;
			//remove * from nodeId if reverse complement
			string nodeId = node->identifier;
			if (node->isReverse) {
				nodeId.pop_back();
			}
			if(node->isAnchoringNode) {
				auto it = find(usedContigs.begin(), usedContigs.end(), nodeId);

				if (it != usedContigs.end()) {
					//if contig already in vector
					continue;
				} else {
					usedContigs.push_back(node->identifier);
				}
				sequence = contigsMap[nodeId];
			} else {
				sequence = readsMap[nodeId];
			}
			
			int start = get<0>(node->seqTuple);
			int end = get<1>(node->seqTuple);
			int length = get<2>(node->seqTuple);
			
			if (!node->isReverse) {
				//first sequence
				if(flagFirst) {
					finalSequence = sequence;
					flagFirst = false;
				} else {
					finalSequence += sequence.substr(end, length);
				}
			} else {
				//first sequence
				if(flagFirst) {
					finalSequence = reverseComplement(sequence);
					flagFirst = false;
				} else {
					string reversedSequence = reverseComplement(sequence);
					int new_end = length-1 - start;
					int new_start = length-1 - end;
					finalSequence += reversedSequence.substr(new_start, new_end);
				}
			}
		}
		
	}
	return finalSequence;
}


int main(int argc, char* argv[]){
	
	//deafult values
	bool flagMonteCarlo = 0;
	string scoreType = "overlap";
	
	string fileContigs;
	string fileReads;
	char opts[] = "h:o:e:m";

    int opt;
	while ((opt = getopt(argc, argv, opts)) != -1){
        switch (opt){
	        case 'h':
			 //help();
	         break;
	        case 'o':
	         scoreType = "overlap";
	         break;
			case 'e':
			 scoreType = "extension";
			 break;
			case 'm':
			 scoreType = "monteCarlo";
			 break;
        }
    }

    fileContigs = argv[optind];
    fileReads = argv[optind + 1];
	
	
	if (scoreType == "monteCarlo") {
		flagMonteCarlo = 1;
	}
	vector<tuple<string, string, int, string, string, tuple<int,int,int> , tuple<int,int,int>> > contigReadOverlaps = readPAF(fileContigs, scoreType);
	vector<tuple<string, string, int, string, string,tuple<int,int,int> , tuple<int,int,int>> > readReadOverlaps = readPAF(fileReads, scoreType);	
	
	unordered_map<string, Node*> nodes;
	vector<Node*> contigs;
	vector<Node*> contigsReversed;
	
	std::cout << "before contig-read" << endl;
	//Going over contig-read overlaps
	for (const auto& overlap : contigReadOverlaps) {
        string queryId = get<0>(overlap);
        string targetId = get<1>(overlap);
        int overlapScore = get<2>(overlap);
        string sign = get<3>(overlap);
		string direction = get<4>(overlap);
		tuple<int,int,int> queryTuple = get<5>(overlap);
		tuple<int,int,int> targetTuple = get<6>(overlap);

		string queryIdR = queryId + "*";
		string targetIdR = targetId + "*";

		//Set nodes
		if (nodes.find(queryId) == nodes.end()) {
			nodes[queryId] = new Node(queryId, false, false, queryTuple);
			nodes[queryIdR] = new Node(queryIdR, false, true, queryTuple); 
		}
		if (nodes.find(targetId) == nodes.end()) {
			nodes[targetId] = new Node(targetId, true, false, targetTuple);
			contigs.push_back(nodes[targetId]);

			nodes[targetIdR] = new Node(targetIdR, true, true, targetTuple);
			contigsReversed.push_back(nodes[targetIdR]);
		}
		/*
		if (sign == "-"){
			if (nodes.find(queryIdR) == nodes.end()) {
				nodes[queryIdR] = new Node(queryIdR, false, true);
			}
			if (nodes.find(targetIdR) == nodes.end()) {
				nodes[targetIdR] = new Node(targetIdR, true, true);
			}
		}*/

        //Add child based on sign and direction
		if (sign == "+") {
			if (direction == "right") {
				nodes[queryId]->addChild(nodes[targetId], overlapScore);
				nodes[targetIdR]->addChild(nodes[queryIdR], overlapScore);
			} else {
				nodes[targetId]->addChild(nodes[queryId], overlapScore);
				nodes[queryIdR]->addChild(nodes[targetIdR], overlapScore);
			}
		}else{
			if (direction == "right") {
				nodes[targetId]->addChild(nodes[queryIdR], overlapScore);
				nodes[queryId]->addChild(nodes[targetIdR], overlapScore);
			} else {
				nodes[targetIdR]->addChild(nodes[queryId], overlapScore);
				nodes[queryIdR]->addChild(nodes[targetId], overlapScore);
			}
		}
        //dodati za reverzni komplement
		
		
    }
	
	std::cout << "before read-read" << endl;
	//Going over read-read overlaps
	for (const auto& overlap : readReadOverlaps) {
        string queryId = get<0>(overlap);
        string targetId = get<1>(overlap);
        int overlapScore = get<2>(overlap);
        string sign = get<3>(overlap);
		string direction = get<4>(overlap);
		tuple<int,int,int> queryTuple = get<5>(overlap);
		tuple<int,int,int> targetTuple = get<6>(overlap);

		string queryIdR = queryId + "*";
		string targetIdR = targetId + "*";

		//Set nodes
		if (nodes.find(queryId) == nodes.end()) {
			nodes[queryId] = new Node(queryId, false, false, queryTuple);
			nodes[queryIdR] = new Node(queryIdR, false, true, queryTuple); 
		}
		if (nodes.find(targetId) == nodes.end()) {
			nodes[targetId] = new Node(targetId, false, false, targetTuple);
			nodes[targetIdR] = new Node(targetIdR, false, true, targetTuple);
		}

	
        //Add child
        //Add child based on sign and direction
		if (sign == "+") {
			if (direction == "right") {
				nodes[queryId]->addChild(nodes[targetId], overlapScore);
				nodes[targetIdR]->addChild(nodes[queryIdR], overlapScore);
			} else {
				nodes[targetId]->addChild(nodes[queryId], overlapScore);
				nodes[queryIdR]->addChild(nodes[targetIdR], overlapScore);
			}
		}else{
			if (direction == "right") {
				nodes[targetId]->addChild(nodes[queryIdR], overlapScore);
				nodes[queryId]->addChild(nodes[targetIdR], overlapScore);
			} else {
				nodes[targetIdR]->addChild(nodes[queryId], overlapScore);
				nodes[queryIdR]->addChild(nodes[targetId], overlapScore);
			}
		}
		//dodati za reverzni komplement
		
		
    }
	/*
	
	nodes["ctg1"] = new Node("ctg1", true, false);
	nodes["read1"] = new Node("read1", false, false);
	nodes["read2"] = new Node("read2", false, false);
	
	nodes["ctg1"]->addChild(nodes["read1"], 9);
	nodes["ctg1"]->addChild(nodes["read2"], 8);
	
	nodes["ctg2"] = new Node("ctg2", true, false);
	nodes["read1"] = new Node("read1", false, false);
	nodes["read3"] = new Node("read3", false, false);

	nodes["read3"]->addChild(nodes["ctg2"], 5);
	//nodes["read1"]->addChild(nodes["ctg2"], 7);

	nodes["read2"]->addChild(nodes["read3"], 8);
	//nodes["read1"]->addChild(nodes["read3"], 4);
	
	//nodes["ctg2"]->addChild(nodes["read1"], 6);
	//nodes["ctg2"]->addChild(nodes["read3"], 8);
	
	*/
	std::cout << "before paths" << endl;
	vector<vector<Node*> > paths;
	/*
	for (const auto& node : nodes) {
		if(node.second->isAnchoringNode) {
			//cout << node.first<<endl;
			vector<vector<Node*> > nodePaths = constructPaths(node.second, flagMonteCarlo);
			if (nodePaths.empty()){
				continue;
			}else{
				paths.insert(paths.end(), nodePaths.begin(), nodePaths.end());
			}
		}
	} */
	std::sort(contigsReversed.begin(), contigsReversed.end(), compareNodesReversed);
	Node* firstR = contigsReversed[0];
	std::cout << firstR->identifier << endl;
	contigsReversed.erase(contigsReversed.begin());

	Node* first = contigs[0];
	std::cout << first->identifier << endl;
	contigs.erase(contigs.begin());

	std::sort(contigs.begin(), contigs.end(), compareNodes);

	
	vector<vector<Node*> > nodePaths = constructPaths(first, flagMonteCarlo, contigs);
	//vector<vector<Node*> > nodePaths = constructPaths(firstR, flagMonteCarlo, contigsReversed);
	std::cout << "tu sam" << endl;
	paths.insert(paths.end(), nodePaths.begin(), nodePaths.end());
	
	//print paths
	for(const auto& path : paths) {
		for(const auto& node : path) {
			std::cout << node->identifier << " ";
			//dodati ako je node kompl dodati *
		}
		std::cout << endl;
		
	}
	
	//parse reads and contigs to map(name:sequence)
	map<string,string> contigsMap = parseFastaToMap("ecoli_test_contigs.fasta");
	map<string,string> readsMap = parseFastaToMap("ecoli_test_reads.fasta");
	
	string finalSequence = makeSequenceFromPath( paths, contigsMap, readsMap);
	
	string output_file = "output.fasta";
	
	ofstream outputFile(output_file);

	//write sequence in file
    if (outputFile.is_open()) {
		outputFile << ">NC_000913.3 Escherichia coli str. K-12 substr. MG1655, complete genome\n";
        outputFile << finalSequence;
        outputFile.close();
        std::cout << "String written to file." << std::endl;
    } else {
        std::cerr << "Failed to open the file." << std::endl;
    }
	
	for(const auto& node : nodes) {
		delete node.second;
	}
	
	return 0;
}