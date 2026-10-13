#include "gameMap.h"
#include "asserts.h"

void GameMap::create(int w, int h)
{
	*this = {}; // reset all the GameMap structure data
	mapData.resize(w * h);

	this->w = w;
	this->h = h;

	for (auto &e : mapData) { e = {}; } // clears all the Block data
}

Block &GameMap::getBlockUnsafe(int x, int y)
{
	permaAssertCommentDevelopement(mapData.size() == w * h, "Map data not initialized"); // we assert that width and height are not 0 so the map is initialized
	permaAssertCommentDevelopement(x >= 0 && y >= 0 && x < w && y < h, "getBlockUnsafe out of bounds error");

	return mapData[ x + y * w ];
}

Block *GameMap::getBlockSafe(int x, int y)
{
	permaAssertCommentDevelopement(mapData.size() == w * h, "Map data not initialized"); // we assert that width and height are not 0 so the map is initialized
	if (x < 0 || y < 0 || x >= w || y >= h) return nullptr; // if the coordinates are out of bounds, we return nullptr

	return &mapData[x + y * w];
}
