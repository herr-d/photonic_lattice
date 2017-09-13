#ifndef PARALLEL_HPP
#define PARALLEL_HPP

#include <basic_includes.hpp>
#include <Graph.hpp>
#include <string>
#include <deque>

class Parallel;
class Graph;

class SquareLattice
{
	size_type _x, _y, _z;
	Parallel& _parent;
public:
	
	SquareLattice(size_type x, size_type y, size_type z, Parallel& p)
	: _x(x), _y(y), _z(z), _parent(p)
	{}
	
	inline size_type get_x(){return _x;}
	inline size_type get_y(){return _y;}
	inline size_type get_z(){return _z;}
	inline vec coordFromId(size_type id) const{
		vec v {id % _x, (id/_x) %_y , (id/_x/_y)%_z};
		return v;
	}
	inline size_type idFromCoordinate(const vec& vec) const{
		return vec.at(0) + vec.at(1)*_x + vec.at(2)*_x*_y;
	}

	inline size_type get_neighbor_id(size_type id, const direction& dir) const{
		return id + get_delta(dir);
	}

	void loc_to_global(vec& loc_coord, const vec & boxpos) const;
	bool is_proper_node(const size_type& id) const;
	size_type get_delta(const direction& dir) const;
	direction get_orientation(const size_type& pos) const;
	uint64_t id(const vec &coord) const;
	bool on_boundary(size_type pos, const direction dir);
};



class Parallel
{
public:
	SquareLattice _box_lattice;
	const size_type _box_size;
	std::deque<Graph> _boxes;
	uint64_t _failed_connections;

	Parallel(const size_type& x, const size_type& y, const size_type& z, const size_type boxsize)
		:_box_lattice(x,y,z,*this), _box_size(boxsize), _failed_connections(0)
	{
		return;
	}


	//! generate random faults on the global lattice
	void gen_lattice(const double probability);


	//! Find structure positions for every box in the algorithm
	void find_structures();

	//! Perform Astar for all boxes with structures
	void path_finding();

	//! Writeout the whole graph with faults (json format)
	void writeout_graph(const std::string output);

	//! give a list of Y measurements, can be plotted as well (json format)
	void writeout_ymeasure(const std::string output);

	void statistics_output(size_type boxsize, double orig_error_rate);
};

#endif
