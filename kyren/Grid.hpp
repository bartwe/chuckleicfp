#pragma once

#include <vector>
#include <cstdint>
#include "Tile.hpp"
#include "Util.hpp"

struct Position
{
	int x, y;
};

// posidx_t = uint16_t -> up to 64k tiles (e.g. 256x256)
// posidx_t = uint32_t -> up to 4G tiles
template<typename content_t, typename posidx_t=uint32_t>
class Grid
{
public: // members:
	/*const*/ int xx, yy; // resp. width and height

// Stuff
	Grid()
	{
		xx = yy = 0;
	}
	Grid(posidx_t width, posidx_t height, content_t initial)
	{
		reset(width, height, initial);
	}

	void reset(posidx_t width, posidx_t height, content_t initial)
	{
		REQUIRE( (posidx_t)(xx*yy) == xx*yy );
		xx=width;
		yy=height;
		data.clear();
		data.resize(xx*yy, initial);
	}

	inline posidx_t posidx(int x, int y)
	{
		REQUIRE( x >= 0 && x<xx );
		REQUIRE( y >= 0 && y<yy );
		return y*xx+x;
	}
	inline content_t& atidx(posidx_t pos)
	{
		REQUIRE( pos < xx*yy );
		return data[pos];
	}
	inline content_t& at(int x, int y)
	{
		return atidx(posidx(x,y));
	}
	inline content_t& at(Position p)
	{
		return at(p.x, p.y);
	}

	content_t* getGrid()
	{
		return &data[0];
	}
	content_t* getGridRow(int y)
	{
		return &at(0, y);
	}
	int gridSize()
	{
		return xx*yy;
	}
private:
	std::vector<content_t> data;
};
