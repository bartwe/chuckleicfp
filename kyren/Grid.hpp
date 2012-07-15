#pragma once

#include <vector>
#include <cstdint>
#include <cassert>

struct Position {
	int x, y;
};

// posidx_t = uint16_t -> up to 64k tiles (e.g. 256x256)
// posidx_t = uint32_t -> up to 4G tiles
template<typename content_t, typename posidx_t = uint32_t>
class Grid {
public:
	Grid() {
		xx = yy = 0;
	}

	Grid(posidx_t width, posidx_t height, content_t initial) {
		reset(width, height, initial);
	}

	void reset(posidx_t width, posidx_t height, content_t initial) {
		assert( (posidx_t)(xx*yy) == xx*yy );
		xx=width;
		yy=height;
		data.clear();
		data.resize(xx*yy, initial);
	}

	posidx_t pos2idx(int x, int y) const {
		assert( x >= 0 && x<xx );
		assert( y >= 0 && y<yy );
		return y*xx+x;
	}

	Position idx2pos(posidx_t idx) const {
	  Position res = {(int)idx % xx, (int)idx/xx};
	  assert( pos2idx(res.x, res.y) == idx /* +1 */);
	  return res;
	}

	content_t& atidx(posidx_t pos) {
		assert( pos < xx*yy );
		return data[pos];
	}

	content_t const& atidx(posidx_t pos) const {
		assert( pos < xx*yy );
		return data[pos];
	}

	content_t& at(int x, int y) {
		return atidx(pos2idx(x,y));
	}

	content_t& at(Position p) {
		return at(p.x, p.y);
	}

	content_t const& at(int x, int y) const {
		return atidx(pos2idx(x,y));
	}

	content_t const& at(Position p) const {
		return at(p.x, p.y);
	}

	content_t* getGrid() {
		return &data[0];
	}

	content_t const* getGrid() const {
		return &data[0];
	}

	content_t* getGridRow(int y) {
		return &at(0, y);
	}

	int gridSize() const {
		return xx*yy;
	}

private:
	int xx, yy;
	std::vector<content_t> data;
};
