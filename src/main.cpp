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

/*
Methods for sorting/comparing
*/
bool compareNodes(const Node* node1, const Node* node2) {
    return node1->identifier < node2->identifier;
}
bool compareNodesReversed(const Node* node1, const Node* node2) {
    return node1->identifier > node2->identifier;
}
bool comparePairs(const tuple<Node*, int, tuple<int, int, int>, tuple<int, int, int>>& a, const tuple<Node*, int, tuple<int, int, int>, tuple<int, int, int>>& b) {
	return get<1>(a) > get<1>(b);
}


/**
 * @brief This function performs Monte Carlo method.
 *
 * @param weights
 * @return Random index
 *
 * @author Ema Vlainić
 */
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
/**
 * @brief This function constructs paths between contigs.
 *
 * @param startNode First node in the path.
 * @param flagMonteCarlo Boolean value that determines whether we use Monte Carlo method
 * @param contigs Vector of contig names
 * @return Vector of paths between two contigs
 *
 * @author Lana Tuković
 */
vector<vector<Node*> > constructPaths(Node* startNode, bool flagMonteCarlo, vector<Node*> contigs) {
    vector<vector<Node*> > paths;
    vector<Node*> currentPath;
	
	unordered_map<string, vector<Node*>> forRemove;
    currentPath.push_back(startNode);

    while (!currentPath.empty()) {
        Node* currentNode = currentPath.back();
        const vector<tuple<Node*, int, tuple<int, int, int>, tuple<int, int, int>> >& children = currentNode->children;

        if (children.empty()) {
            currentPath.pop_back();
			if(!currentPath.empty()){
				Node* oldCurrentNode = currentPath.back();
        		const vector<tuple<Node*, int, tuple<int, int, int>, tuple<int, int, int>> >& oldChildren = oldCurrentNode->children;
				vector<tuple<Node*, int, tuple<int, int, int>, tuple<int, int, int>> > oldSortedChildren(oldChildren);
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
			vector<tuple<Node*, int, tuple<int, int, int>, tuple<int, int, int>>> sortedChildren;
			vector<tuple<Node*, int, tuple<int, int, int>, tuple<int, int, int>>> childrenForSort = children;

			if (forRemove[currentNode->identifier].size() != 0){
					vector<tuple<Node*, int, tuple<int, int, int>, tuple<int, int, int>>> newSortedChildren;
					for (const auto& t : childrenForSort) {
						if (std::find_if(forRemove[currentNode->identifier].begin(), forRemove[currentNode->identifier].end(), [&](Node* node) {
        								return std::get<0>(t)->identifier == node->identifier;
    									}) == forRemove[currentNode->identifier].end()) {
							newSortedChildren.push_back(t);
						}
					}
					childrenForSort = newSortedChildren;
					
			}
			
			if (flagMonteCarlo) {
				if(childrenForSort.size() != 0){
					vector<double> weights;
					for (const auto& child : childrenForSort) {
						weights.push_back(get<1>(child));
					}
					
					for (int i = 0; i < childrenForSort.size(); i++) {
						size_t selectedIndex = monteCarloSelection(weights);
						sortedChildren.push_back(children[selectedIndex]);
						weights.erase(weights.begin() + int(selectedIndex));
					}
				}
				
			} else {
				sortedChildren = childrenForSort;
				if(sortedChildren.size() != 0){
					std::sort(sortedChildren.begin(), sortedChildren.end(), comparePairs);
				}
				
			}
			Node* emptyNode = new Node("", false, false, make_tuple(0,0,0), make_tuple(0,0,0));
			Node* bestChild = emptyNode;
			tuple<int, int, int> overlap;
			tuple<int, int, int> overlapParent;
			if(sortedChildren.size() != 0){
				bestChild = get<0>(sortedChildren[0]); // Initialize with the first child
				overlap = get<2>(sortedChildren[0]);
				overlapParent =  get<3>(sortedChildren[0]);
				while (!sortedChildren.empty()) {
					
					bestChild = get<0>(sortedChildren[0]);
					overlap = get<2>(sortedChildren[0]);
					overlapParent =  get<3>(sortedChildren[0]);
					
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
				
			}
			
			
			if (bestChild->isAnchoringNode && (contigs[0]->identifier == bestChild->identifier || contigs[0]->identifier + "*" == bestChild->identifier)) {
				
				Node* parent = currentPath.back();
				currentPath.push_back(bestChild);
				bestChild->seqTuple = overlap;
				
				parent->seqTuple2 = overlapParent;

				paths.push_back(currentPath);
				
				contigs.erase(contigs.begin());
				if (contigs.size() == 0){
					return paths;
				}
				currentPath.clear();
				currentPath.push_back(bestChild); 
				forRemove.clear();
				
			}else if(bestChild->isAnchoringNode && contigs[0]->identifier != bestChild->identifier && contigs[0]->identifier + "*" != bestChild->identifier){
				forRemove[currentNode->identifier].push_back(bestChild);	

			}else if (sortedChildren.empty()) {
				Node* r = currentPath.back();
				currentPath.pop_back();
				Node* curr = currentPath.back();
				forRemove[curr->identifier].push_back(r);
				
			} else {
				Node* parent = currentPath.back();
				currentPath.push_back(bestChild);
				bestChild->seqTuple = overlap; 
				parent->seqTuple2 = overlapParent;
            }
        }
    }
	
    return paths;
}

/**
 * @brief This function reads PAF files and calculates overlap information.
 *
 * @param filePath PAF file path
 * @param scoreType Type of score we want to use in program
 * @return Vector that contains overlap information
 *
 * @author Ema Vlainić
 */
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
		
		string sign = fields[4];
		
		int queryLength = stoi(fields[1]);
		int queryStart = stoi(fields[2]);
		int queryEnd = stoi(fields[3]);
		
		int targetLength = stoi(fields[6]);
		int targetStart = stoi(fields[7]);
		int targetEnd = stoi(fields[8]);

	
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

/**
 * @brief This function reads from fasta file.
 *
 * @param fileContigs
 * @return Map that contains sequences from fasta file
 *
 * @author Ema Vlainić
 */
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
/**
 * @brief This function connects the sequences from constructed path and places it in fasta file.
 *
 * @param paths Vector of constructed paths
 * @param contigsMap Map that contains contig sequences
 * @param readsMap Map that contains read sequences
 * @return Map that contains sequences from fasta file
 *
 * @author Ema Vlainić, Lana Tuković
 */
string makeSequenceFromPath(vector<vector<Node*>> paths, map<string,string> contigsMap, map<string,string> readsMap) {
	bool flagFirst = true;
	string finalSequence = "";
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
					usedContigs.push_back(nodeId);
				}
				sequence = contigsMap[nodeId];
			} else {
				sequence = readsMap[nodeId];
			}
			
			int start = get<0>(node->seqTuple);
			int end = get<1>(node->seqTuple)-1;
			int length = get<2>(node->seqTuple);
			
			std::cout << nodeId << " ";
			std::cout << start << " " << end << " " << length << endl;

			if (!node->isReverse) {
				if(node->isAnchoringNode){
					if (flagFirst){
						finalSequence += sequence;
						flagFirst = false;
					}else{
						finalSequence.erase(finalSequence.length() - end);
						finalSequence += sequence;
					}
				}else{
					finalSequence += sequence.substr(end, (get<1>(node->seqTuple2)-1)-end);
					
				}
			}else{
				//za reverzni
				int new_end = length-1 - start;
				if(node->isAnchoringNode){
					
					if (flagFirst){
						finalSequence += reverseComplement(sequence);
						flagFirst = false;
					}else{
						finalSequence.erase(finalSequence.length() - new_end);
						finalSequence += reverseComplement(sequence);
					}
				}else {
					string reversedSequence = reverseComplement(sequence);
					int new_end2 = length-1-(get<1>(node->seqTuple2)-1);
					
					finalSequence += reversedSequence.substr(new_end, new_end2-new_end);
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
			nodes[queryId] = new Node(queryId, false, false, make_tuple(0,0,0),  make_tuple(0,0,0));
			nodes[queryIdR] = new Node(queryIdR, false, true, make_tuple(0,0,0),  make_tuple(0,0,0)); 			
		}
		if (nodes.find(targetId) == nodes.end()) {
			nodes[targetId] = new Node(targetId, true, false, make_tuple(0,0,0),  make_tuple(0,0,0));
			nodes[targetIdR] = new Node(targetIdR, true, true, make_tuple(0,0,0),  make_tuple(0,0,0));
			contigs.push_back(nodes[targetId]);

			contigsReversed.push_back(nodes[targetIdR]);
		}
		
        //Add child based on sign and direction
		if (sign == "+") {
			if (direction == "left") {
				nodes[queryId]->addChild(nodes[targetId], overlapScore, targetTuple, queryTuple);
				nodes[targetIdR]->addChild(nodes[queryIdR], overlapScore, queryTuple, targetTuple);
			} else {
				nodes[targetId]->addChild(nodes[queryId], overlapScore, queryTuple, targetTuple);
				nodes[queryIdR]->addChild(nodes[targetIdR], overlapScore, targetTuple, queryTuple);
			}
		}else{
			if (direction == "left") {
				nodes[targetId]->addChild(nodes[queryIdR], overlapScore, queryTuple, targetTuple);
				nodes[queryId]->addChild(nodes[targetIdR], overlapScore, targetTuple, queryTuple);
			} else {
				nodes[targetIdR]->addChild(nodes[queryId], overlapScore, queryTuple, targetTuple);
				nodes[queryIdR]->addChild(nodes[targetId], overlapScore, targetTuple, queryTuple);
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
			nodes[queryId] = new Node(queryId, false, false, make_tuple(0,0,0),  make_tuple(0,0,0));
			nodes[queryIdR] = new Node(queryIdR, false, true, make_tuple(0,0,0),  make_tuple(0,0,0)); 
		}
		if (nodes.find(targetId) == nodes.end()) {
			nodes[targetId] = new Node(targetId, false, false, make_tuple(0,0,0),  make_tuple(0,0,0));
			nodes[targetIdR] = new Node(targetIdR, false, true, make_tuple(0,0,0),  make_tuple(0,0,0));
		}

	
        //Add child
        //Add child based on sign and direction
		if (sign == "+") {
			if (direction == "left") {
				nodes[queryId]->addChild(nodes[targetId], overlapScore, targetTuple, queryTuple);
				nodes[targetIdR]->addChild(nodes[queryIdR], overlapScore, queryTuple, targetTuple);
			} else {
				nodes[targetId]->addChild(nodes[queryId], overlapScore, queryTuple, targetTuple);
				nodes[queryIdR]->addChild(nodes[targetIdR], overlapScore, targetTuple, queryTuple);
			}
		}else{
			if (direction == "left") {
				nodes[targetId]->addChild(nodes[queryIdR], overlapScore, queryTuple, targetTuple);
				nodes[queryId]->addChild(nodes[targetIdR], overlapScore, targetTuple, queryTuple);
			} else {
				nodes[targetIdR]->addChild(nodes[queryId], overlapScore, queryTuple, targetTuple);
				nodes[queryIdR]->addChild(nodes[targetId], overlapScore, targetTuple, queryTuple);
			}
		}
		//dodati za reverzni komplement
		
		
    }
	
	std::cout << "before paths" << endl;
	vector<vector<Node*> > paths;
	
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