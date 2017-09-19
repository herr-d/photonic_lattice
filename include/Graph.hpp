#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <basic_includes.hpp>
#include <Parallel.hpp>

class Astar;
class Parallel;

class Graph
{
private:
	//! stores the position of the structure, if not it is set to the maximum value
	size_type _structure_pos;

	//! size in single dimension of underlying square lattice
	size_type _nrVerticesOnAxis;

	//! container with all y-measurements in this part of the lattice
	faults_type _ymeasurements;

	//! maximal id: there are no nodes with ids higher than this value
	size_type _max_id;

	//! did the pathfinding algorithm already run on this lattice
	bool _isPurified;
	bool _structureExists;
	Parallel& _parent;
	// add a container to keep track of all unfinished connections.
	// Structure (next id,[(originial id, direction)])
public:
	static 	std::unordered_map<position, std::vector<std::pair<position,direction>>> _unfinished;

	//! container with all neighbors. Format: (node id, (neighbor boxid, neighbor id))
	std::unordered_map<size_type, std::vector<position>> _neighbors;

	Graph(const size_type size, Parallel& p)
		:_structure_pos(0), _isPurified(false), _nrVerticesOnAxis(size), _max_id(size*size*size), _structureExists(false), _parent(p){
		assert(size%2 == 0); //size has to be even for all boxes to properly align
		return;
	}

	Graph& operator=(const Graph& other); //needed for std::container implementation


	inline size_type get_max_id(){return _max_id;}
	const inline bool isPurified(){return _isPurified;}
	inline void markAsPurified(){_isPurified = true;}
	inline size_type getMaxId(){return _max_id;}
	inline size_type getnodesaxis(){return _nrVerticesOnAxis;}
	inline size_type get_structure(){return _structure_pos;}
	inline faults_type& get_ymeasure(){return _ymeasurements;}
	inline size_type ymeasuresize(){return _ymeasurements.size();}

	inline void loc_to_global(vec& loc_coord, const vec & boxpos) const{
		for(size_type i=0; i< boxpos.size(); ++i){
			loc_coord.at(i) += (_nrVerticesOnAxis - 1)*boxpos.at(i);
		}
	}
	
	inline size_type idFromCoordinate(const vec& vec) const{
		return vec.at(0) + vec.at(1)*_nrVerticesOnAxis + vec.at(2)*_nrVerticesOnAxis*_nrVerticesOnAxis;
	}
	inline vec coordFromId(size_type id) const{
		vec v {id % _nrVerticesOnAxis, (id/_nrVerticesOnAxis) %_nrVerticesOnAxis, (id/_nrVerticesOnAxis/_nrVerticesOnAxis)%_nrVerticesOnAxis};
		return v;
	}
	//! has the structure been found?
	inline bool structure_exist(){
		return _structureExists;
	}

	void add_to_unfinished(position key, position pos, direction dir);
	void erase_unfinished(position id);
	void add_to_neighbors(size_type key, position value);



	void generate_connections(const size_type& box_id, const float& success_rate);
	void generate_connections_simple(const size_type& box_id, const float& success_rate);
	void generate_connections_square(const size_type& box_id, const float& success_rate);
	void finish_connection(const position& id, const direction& d);
	void next_connection(const position& id, const direction& d, const position& root);
	void continue_connection(const position& id, const direction& d);
	direction get_orientation(const size_type& pos) const;

	bool is_proper_node(size_type id) const;
	bool is_proper_node(const vec & coord) const;
	const bool on_boundary(size_type pos, const direction dir);
	const int get_delta(const direction dir);
	void find_structure(const direction& dir, const size_type box_id);
	size_type get_structure_handle(const size_type& pos, const direction& dir);
	bool handle_dir_exists(const size_type& pos, const direction& dir);
	void measure_structure();

	friend Astar;
};


#include <Purify.hpp>

#endif
