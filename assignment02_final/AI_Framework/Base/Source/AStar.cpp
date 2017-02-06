#include "AStar.h"

//Constructor(s) & Destructor
AStarSearch::AStarSearch() {
}

AStarSearch::~AStarSearch() {
}

//Private Helper Function(s)
void AStarSearch::AddToList(TileSystem* _tileSystem, unordered_map<int, AStarNode*>& _list, AStarNode* _node) const {
	int index = _node->row * _tileSystem->GetNumColumns() + _node->column;
	_list.insert(std::pair<int, AStarNode*>(index, _node));
}

void AStarSearch::RemoveFromList(TileSystem* _tileSystem, unordered_map<int, AStarNode*>& _list, AStarNode* _node) const {
	int index = _node->row * _tileSystem->GetNumColumns() + _node->column;
	_list.erase(_list.find(index));
}

AStarNode* AStarSearch::GetFromList(TileSystem* _tileSystem, unordered_map<int, AStarNode*>& _list, int _row, int _column) const {
	int index = _row * _tileSystem->GetNumColumns() + _column;
	unordered_map<int, AStarNode*>::iterator iter = _list.find(index);
	if (iter != _list.end()) {
		return iter->second;
	}
	return nullptr;
}

bool AStarSearch::IsTraversable(TileSystem* _tileSystem, int _searcherID, int _row, int _column) const {	
	Tile* tile = &_tileSystem->GetTile(_row, _column);
	//It is traversable if the tile value is not 0 and has no occupant or the occupant is the searcher.
	return tile->value == 0 && (tile->occupant < 0 || tile->occupant == _searcherID);
}

int  AStarSearch::ComputeH(int _row, int _column, int _endRow, int _endColumn) const {
	return abs(_endRow - _row) + abs(_endColumn - _column);
}

//Virtual Function(s)
int  AStarSearch::ComputeA(TileSystem* _tileSystem, unordered_map<int, int>* _influenceTable, unordered_set<int>* _detectedIDs,
						   int _row, int _column) const {
	int A = 0;
	Tile* tile = &_tileSystem->GetTile(_row, _column);
	for (list<std::pair<int, int> >::iterator listIter = tile->ids.begin(); listIter != tile->ids.end(); ++listIter) {
		//listIter->first is the id of the Entity.
		//Check if this id is something we've detected.
		if (_detectedIDs->find(listIter->first) != _detectedIDs->end()) {
			//listIter->second is the type of the enemy.
			unordered_map<int, int>::iterator umapIter = _influenceTable->find(listIter->second);
			if (umapIter != _influenceTable->end()) {
				A += umapIter->second;
			}
		}
	}

	return A;
}

list<std::pair<int, int> > AStarSearch::Search(TileSystem* _tileSystem, unordered_map<int, int>* _influenceTable, unordered_set<int>* _detectedIDs, int _searcherID,
											   int _startRow, int _startColumn, int _endRow, int _endColumn) const
{
	list<std::pair<int, int> > path;

	//Check if there's a Tile System.
	if (_tileSystem == nullptr) {
		return path;
	}
	//Check if we are already at the end location.
	if (_startRow == _endRow && _startColumn == _endColumn) {
		return path;
	}

	//Check if we are within the bounds of the tileSystem.
	if (_startRow < 0 || _startRow >= _tileSystem->GetNumRows()) {
		return path;
	} else if (_startColumn < 0 || _startColumn >= _tileSystem->GetNumColumns()) {
		return path;
	} else if (_endRow < 0 || _endRow >= _tileSystem->GetNumRows()) {
		return path;
	} else if (_endColumn < 0 || _endColumn >= _tileSystem->GetNumColumns()) {
		return path;
	}

	//Check if the end location is traversable.
	if (!IsTraversable(_tileSystem, _searcherID, _endRow, _endColumn)) {
		return path;
	}

	unordered_map<int, AStarNode*> openList; //Nodes along the border that we are checking.
	unordered_map<int, AStarNode*> closedList; //Nodes that we are done checking.
	float movementCost = 10.0f; //The cost of moving from one node to another.

	AStarNode* startNode = new AStarNode();
	startNode->row = _startRow;
	startNode->column = _startColumn;
	startNode->H = ComputeH(startNode->row, startNode->column, _endRow, _endColumn);
	AddToList(_tileSystem, openList, startNode);

	bool stopLoop = false;
	while (!openList.empty() && !stopLoop) {
		AStarNode* cheapestNode = openList.begin()->second;
		for (unordered_map<int, AStarNode*>::iterator iter = std::next(openList.begin()); iter != openList.end(); ++iter) {
			AStarNode* iterNode = iter->second;
			if (iterNode->GetF() < cheapestNode->GetF()) {
				cheapestNode = iterNode;
			}
		}

		//Move the node from the Open List to the Closed List.
		RemoveFromList(_tileSystem, openList, cheapestNode);
		AddToList(_tileSystem, closedList, cheapestNode);

		//Check the nodes around the cheapest node and see if they are the goal node.
		//Check 1 row behind, then 1 row infront, then 1 column behind, then 1 column infront.
		int rowOffset[4] = {-1, 1, 0, 0};
		int columnOffset[4] = {0, 0, -1, 1};
		for (int i = 0; i < 4 && !stopLoop; ++i) {
			int row = cheapestNode->row + rowOffset[i];
			int column = cheapestNode->column + columnOffset[i];

			//Make sure that we are still within the tile map.
			if (row < 0 || row >= _tileSystem->GetNumRows() || column < 0 || column >= _tileSystem->GetNumColumns()) {
				continue;
			}

			//Check if this path is traversable. For now let's just take 1 as intraversable.
			if (!IsTraversable(_tileSystem, _searcherID, row, column)) {
				continue;
			}

			//Skip the node if it is in the Closed List.
			AStarNode* neighbourNode = GetFromList(_tileSystem, closedList, row, column);
			if (neighbourNode != nullptr) {
				continue;
			}

			//Check if it is in the Open List.
			float G = cheapestNode->G + movementCost; //The cost of moving to the neighbour node from the current node.
			neighbourNode = GetFromList(_tileSystem, openList, row, column);
			//Check if it is in the Open List.
			if (neighbourNode != nullptr) {
				//If it is cheaper to get to the neighbour node from the current node then the neighbour node's old path, replace it.
				if (G < neighbourNode->G) {
					neighbourNode->G = G;
					neighbourNode->parent = cheapestNode;
				}
			}

			//If it is not in the Open List, then a node for this tile was never created.
			if (neighbourNode == nullptr) {
				neighbourNode = new AStarNode();
				neighbourNode->row = row;
				neighbourNode->column = column;
				neighbourNode->H = ComputeH(neighbourNode->row, neighbourNode->column, _endRow, _endColumn);
				neighbourNode->G = G;
				neighbourNode->A = ComputeA(_tileSystem, _influenceTable, _detectedIDs, neighbourNode->row, neighbourNode->column);
				neighbourNode->parent = cheapestNode;

				//Add it to the Open List.
				AddToList(_tileSystem, openList, neighbourNode);
			}

			//If this node is what we were looking for, then we can stop.
			if (neighbourNode->row == _endRow && neighbourNode->column == _endColumn) {
				stopLoop = true; //Break out of our loops.
				AStarNode* resultNode = neighbourNode;
				while (resultNode != startNode) { //Don't at our start node, since we're already there.
					//We are working backwards from the end node to the start node. So we need to push front.
					path.push_front(std::pair<int, int>(resultNode->row, resultNode->column));
					resultNode = resultNode->parent;
				}
			}
		}
	}

	//Now that we've found the path, we need to delete the nodes and clean up memory.
	for (unordered_map<int, AStarNode*>::iterator iter = openList.begin(); iter != openList.end(); ++iter) {
		delete iter->second;
	}
	openList.clear();
	for (unordered_map<int, AStarNode*>::iterator iter = closedList.begin(); iter != closedList.end(); ++iter) {
		delete iter->second;
	}
	closedList.clear();

	return path;
}