#ifndef TILE_H
#define TILE_H

#include <list>
#include <unordered_map>

using std::list;
using std::unordered_map;

struct Tile {
public:
	unsigned int value; //What is on the tile.
	list<std::pair<int, int> > ids; //First: ID, Second: Type
	int occupant; //Is something already standing on this thingy?

	Tile() {
		value = 0;
		occupant = -1;
	}
	Tile(const Tile& _rhs) {
		value = _rhs.value;
		ids = _rhs.ids;
		occupant = _rhs.occupant;
	}
	~Tile() {}

	Tile& operator=(const Tile& _rhs) {
		value = _rhs.value;
		ids = _rhs.ids;
		occupant = _rhs.occupant;
		return *this;
	}
};

#endif