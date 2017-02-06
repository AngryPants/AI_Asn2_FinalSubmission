#ifndef A_STAR_H
#define A_STAR_H

#include <iostream>
#include <vector>
#include <list>
#include <map>
#include <unordered_set>
#include <unordered_map>
#include "TileSystem.h"

using std::cout;
using std::endl;
using std::vector;
using std::list;
using std::map;
using std::unordered_set;
using std::unordered_map;

struct AStarNode {
public:
	AStarNode* parent;
	int row, column;
	int G; //Cost from start node to current node.
	int H; //Cost from current node to end node.
	int A; //Additional Cost.

	AStarNode() {
		parent = nullptr;
		row = 0;
		column = 0;
		G = 0;
		H = 0;
		A = 0;
	}
	AStarNode(const AStarNode& _rhs) {
		parent = _rhs.parent;
		row = _rhs.row;
		column = _rhs.column;
		G = _rhs.G;
		H = _rhs.H;
		A = _rhs.A;
	}
	~AStarNode() {}

	int GetF() const { return G+H+A; }
};

class AStarSearch {
public:
	//Constructor(s) & Destructor
	AStarSearch();
	virtual ~AStarSearch();

	//Helper Function(s)
	void AddToList(TileSystem* _tileSystem, unordered_map<int, AStarNode*>& _list, AStarNode* _node) const;
	void RemoveFromList(TileSystem* _tileSystem, unordered_map<int, AStarNode*>& _list, AStarNode* _node) const;
	AStarNode* GetFromList(TileSystem* _tileSystem, unordered_map<int, AStarNode*>& _list, int _row, int _column) const;
	
	bool IsTraversable(TileSystem* _tileSystem, int _searcherID, int _row, int _column) const;
	int ComputeH(int _row, int _column, int _endRow, int _endColumn) const;
	int ComputeA(TileSystem* _tileSystem, unordered_map<int, int>* _influenceTable, unordered_set<int>* _detectedIDs, int _row, int _column) const;

	//Public Function(s)
	list<std::pair<int, int> > Search(TileSystem* _tileSystem, unordered_map<int, int>* _influenceTable, unordered_set<int>* _detectedIDs, int _searcherID,
									  int _startRow, int _startColumn, int _endRow, int _endColumn) const;
};

#endif