#ifndef TILE_SYSTEM_H
#define TILE_SYSTEM_H

#include <vector>
#include <string>
#include "Tile.h"

using namespace std;

class TileSystem {

private:
	//Variable(s)	
	int numRows, numColumns;
	Tile* tileMap;

public:
	//Constructor(s) & Destructor
	TileSystem();
	virtual ~TileSystem();

	//Function(s)
	bool LoadFile(const string& _filePath, bool _reverse = false);
	void Clear();
	int GetNumRows() const;
	int GetNumColumns() const;
	Tile& GetTile(int _row, int _column);
	Tile* GetTileMap();

	/*void SetMask(int _row, int _column, unsigned int _mask);
	void ClearMask(int _row, int _column, unsigned int _mask);
	bool CheckMask(int _row, int _column, unsigned int _mask);*/

	/*void SetBit(unsigned int _row, unsigned int _column, unsigned int _bit);
	void ClearBit(unsigned int _row, unsigned int _column, unsigned int _bit);
	void ToggleBit(unsigned int _row, unsigned int _column, unsigned int _bit);
	bool CheckBit(unsigned int _row, unsigned int _column, unsigned int _bit);*/

};

#endif