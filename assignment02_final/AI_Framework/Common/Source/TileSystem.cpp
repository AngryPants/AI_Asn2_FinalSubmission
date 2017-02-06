#include "TileSystem.h"
#include <fstream>
#include <sstream>
#include <iostream>

//Constructor(s) & Destructor
TileSystem::TileSystem() {
	numRows = 0;
	numColumns = 0;
	tileMap = nullptr;
}

TileSystem::~TileSystem() {
	Clear();
}

void TileSystem::Clear() {
	//Empty our current tiles.
	if (tileMap != nullptr) {
		delete[] tileMap;
		tileMap = nullptr;
	}
	numRows = 0;
	numColumns = 0;
}

bool TileSystem::LoadFile(const string& _filePath, bool _reverse) {
	//Open the file.
	ifstream file(_filePath.c_str());
	//If file is successfully opened.
	if (!file.is_open()) {
		cout << "Error: Cannot open file " << _filePath << "." << endl;
		return false;
	}

	//Empty our current tiles.
	if (tileMap != nullptr) {
		delete[] tileMap;
	}

	numRows = 0;
	numColumns = 0;	

	vector<vector<unsigned int>> tileVector;
	string currentLine = "";
	while(getline(file, currentLine)) {
		// If this line is an empty line, then ignore it.
		if (currentLine == "") {
			continue;
		}
		//A new row of tiles.
		++numRows;
		vector<unsigned int> currentRow;
		currentRow.resize(numColumns, 0); //Make it the size of the largest row.

		//Create a istringstream object with currentLine as it's content.
		istringstream iss(currentLine);
		string token;

		int columnCounter = 0;
		//Each time this loop is done, iss is shortened and token is replaced by whatever was removed from iss.
		while(getline(iss, token, ',')) {
			// Count the number of columns
			++columnCounter;
			//Is the current column we are in right now larger than the size of our current row? Our current row size is also the
			//size of all the rows.
			if (columnCounter > static_cast<int>(currentRow.size())) {
				numColumns = columnCounter; //Update the number of columns.
				currentRow.resize(numColumns, 0); //Resize the current row.
				//Resize all the previous rows.
				for (vector<vector<unsigned int> >::iterator iter = tileVector.begin(); iter != tileVector.end(); ++iter) {
					(*iter).resize(numColumns, 0);
				}
			}
			currentRow[columnCounter - 1] = atoi(token.c_str());
		}
		//Add the current row to our tiles.
		if (_reverse) {
			tileVector.insert(tileVector.begin(), currentRow);
		} else {
			tileVector.push_back(currentRow);
		}		
	}

	//Close the file after we're done.
	file.close();
	
	tileMap = new Tile[numRows * numColumns];
	for (int row = 0; row < numRows; ++row) {
		for (int column = 0; column < numColumns; ++column) {
			tileMap[row * numColumns + column].value = tileVector[row][column];
		}
	}
	return true;
}

int TileSystem::GetNumColumns() const {
	return numColumns;
}

int TileSystem::GetNumRows() const {
	return numRows;
}

Tile& TileSystem::GetTile(int _row, int _column) {
	return tileMap[_row * numColumns + _column];
}

Tile* TileSystem::GetTileMap() {
	return tileMap;
}

//Sets the bit mask.
/*void TileSystem::SetMask(int _row, int _column, unsigned int _mask) {
	tileMap[_row * numColumns + _column] |= _mask;
}

//Unsets the bit mask.
void TileSystem::ClearMask(int _row, int _column, unsigned int _mask) {
	tileMap[_row * numColumns + _column] &= (~_mask);
}

bool TileSystem::CheckMask(int _row, int _column, unsigned int _mask) {
	return (tileMap[_row * numColumns + _column] & _mask) == _mask;
}*/