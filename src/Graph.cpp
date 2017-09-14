#include <basic_includes.hpp>
#include <Graph.hpp>
#include <random>
#include <iterator>
#include <algorithm>

extern std::mt19937_64 rng;

std::unordered_map<position, std::vector<std::pair<position,direction>>> Graph::_unfinished = std::unordered_map<position, std::vector<std::pair<position,direction>>> ();

void Graph::add_to_unfinished(position key, position pos, direction dir){
	if (_unfinished.find(key) == _unfinished.end())
		_unfinished.insert({key, std::vector<std::pair<position,direction>>()});
	_unfinished.at(key).push_back(std::make_pair(pos,dir));
}

void Graph::erase_unfinished(const position id){
	if(_unfinished.find(id) != _unfinished.end())
		Graph::_unfinished.erase(id);
}

void Graph::add_to_neighbors(size_type key, position value){
	if (_neighbors.find(key) == _neighbors.end())
		_neighbors.insert({key, std::vector<position>()});
	if(std::find(_neighbors.at(key).begin(), _neighbors.at(key).end(), 
		value) == _neighbors.at(key).end())
		_neighbors.at(key).push_back(value);
}



direction Graph::get_orientation(const size_type& pos) const{
	vec v = coordFromId(pos);
	v.at(0) = v.at(0)%2;
	v.at(1) = v.at(1)%2;
	v.at(2) = v.at(2)%2;

	assert(v.at(0) != v.at(1) || v.at(1)!=v.at(2));

	if(v.at(0) == v.at(1)){
		return back;
	}
	if(v.at(1) == v.at(2)){
		return right;
	}
	return up;
}


Graph& Graph::operator=(const Graph& other){
	_structure_pos = other._structure_pos;
	_ymeasurements = other._ymeasurements;
	_nrVerticesOnAxis = other._nrVerticesOnAxis;
	_max_id = other._max_id;
	_isPurified = other._isPurified;
	_structureExists = other._structureExists;
	_neighbors = other._neighbors;
	return *this;
}


void Graph::generate_connections(const size_type& box_id, const float& success_rate){
	//set up random number generation
	//std::mt19937 mersenne_engine(seed);
	assert(_neighbors.size() == 0); //only allow the creation of the lattice once!
	std::uniform_real_distribution<double> dist(0,1);
	
	//fill up neighbors
	for(size_type i=0; i < _max_id; ++i){
		_neighbors.insert({i, std::vector<position>()});
	}

	//start from square lattice and build up connections using rules from paper
	for (position id(box_id, 0); id.second < _max_id; id.second=id.second+1){
		//determine info on lattice position
		if (is_proper_node(id.second) == false)
			continue;

		direction dir = get_orientation(id.second);

		//entanglement between microclusters
		for (auto d : { right, up, back}){
			if (dir != d){
				if(dist(rng) < success_rate){//fusion gate between microclusters
					if(dist(rng) < success_rate){//fusion gate inside this microcluster
						finish_connection(id, d);
						next_connection(id, d, id);
					}else{//continue long distance connection
						continue_connection(id, d);
					}
				}else{
					if(dist(rng) < success_rate) //fusion gate inside this microcluster
						finish_connection(id, d);
				}
			}
		}
		//cleanup: all connections goning towards this node need to be deleted
		erase_unfinished(id);
	}
}


void Graph::generate_connections_simple(const size_type& box_id, const float& success_rate){
	//set up random number generation
	//std::mt19937 mersenne_engine(seed);
	assert(_nrVerticesOnAxis%2 == 0);
	std::uniform_real_distribution<double> dist(0,1);
	
	//fill up neighbors
	for(size_type i=0; i < _max_id; ++i){
		_neighbors.insert({i, std::vector<position>()});
	}

	//start from square lattice and build up connections using rules from paper
	for (position id(box_id, 0); id.second < _max_id; id.second=id.second+1){
		//determine info on lattice position
		if (is_proper_node(id.second) == false)
			continue;

		direction dir = get_orientation(id.second);

		//entanglement between microclusters
		for (auto d : { right, up, back}){
			if (dir != d){
				if(dist(rng) < success_rate){//fusion gate between microclusters
					if(on_boundary(id.second,d)){
						if(!_parent._box_lattice.on_boundary(id.first,d)){
							position target(id);
							target.first += _parent._box_lattice.get_delta(d);
							target.second += (d < 3) ? (-1)*std::pow(_nrVerticesOnAxis,d%3)*(_nrVerticesOnAxis-1) : 
								std::pow(_nrVerticesOnAxis,d%3)*(_nrVerticesOnAxis-1);
							add_to_neighbors(id.second,target);
							_parent._boxes.at(target.first).add_to_neighbors(target.second,id);

						}
					}
					else{
						position target(id.first, id.second + get_delta(d));
						add_to_neighbors(id.second, target);
						add_to_neighbors(target.second, id);
					}
				}
			}
		}
		//cleanup: all connections goning towards this node need to be deleted
		erase_unfinished(id);
	}
}

void Graph::generate_connections_square(const size_type& box_id, const float& success_rate){
	//set up random number generation
	//std::mt19937 mersenne_engine(seed);
	assert(_nrVerticesOnAxis%2 == 0);
	std::uniform_real_distribution<double> dist(0,1);
	
	//fill up neighbors
	for(size_type i=0; i < _max_id; ++i){
		_neighbors.insert({i, std::vector<position>()});
	}

	//start from square lattice and build up connections using rules from paper
	for (position id(box_id, 0); id.second < _max_id; id.second=id.second+1){

		//entanglement between microclusters
		for (auto d : { right, up, back}){
			if(dist(rng) < success_rate){
				if(on_boundary(id.second,d)){
					if(!_parent._box_lattice.on_boundary(id.first,d)){
						position target(id);
						target.first += _parent._box_lattice.get_delta(d);
						target.second += (d < 3) ? (-1)*std::pow(_nrVerticesOnAxis,d%3)*(_nrVerticesOnAxis-1) : 
							std::pow(_nrVerticesOnAxis,d%3)*(_nrVerticesOnAxis-1);
						add_to_neighbors(id.second,target);
						_parent._boxes.at(target.first).add_to_neighbors(target.second,id);
					}
				}
				else{
					position target(id.first, id.second + get_delta(d));
					add_to_neighbors(id.second, target);
					add_to_neighbors(target.second, id);
				}
			}
		}
		//cleanup: all connections goning towards this node need to be deleted
		erase_unfinished(id);
	}
}

void Graph::finish_connection(const position& id, const direction& d){
	if(_unfinished.find(id) == _unfinished.end())
		return;
	for(auto it : _unfinished.at(id)){
		if (it.second==d){
			//add connection
			//std::cout << id.second << " " << it.first.second << std::endl;
			add_to_neighbors(id.second, it.first);
			size_type id2 = it.first.second;
			size_type boxid = it.first.first;
			_parent._boxes.at(boxid).add_to_neighbors(id2,id);
		}
	}
}

void Graph::next_connection(const position& id, const direction& d, const position& root){
	//add new connection that originates at root and goes to the node after id
	position target(id);
	if(on_boundary(id.second,d)){
		if(!_parent._box_lattice.on_boundary(id.first,d)){
			target.first += _parent._box_lattice.get_delta(d);

			target.second += (d < 3) ? (-1)*std::pow(_nrVerticesOnAxis,d%3)*(_nrVerticesOnAxis-1) : 
						std::pow(_nrVerticesOnAxis,d%3)*(_nrVerticesOnAxis-1);

			add_to_unfinished(target,root,d);
		}
	}
	else{
		target.second += get_delta(d);
		add_to_unfinished(target,root,d);
	}
	return;
}

void Graph::continue_connection(const position& id, const direction& d){
	if(_unfinished.find(id) == _unfinished.end())
		return;
	auto it = _unfinished[id].begin();
	for(auto it : _unfinished[id]){
		if(it.second==d){
			next_connection(id, d, it.first);
		}
	}
}

void Graph::measure_structure(){
	faults_type nodes;
	if(structure_exist()){
		//in all three dimensions
		_ymeasurements.insert(_structure_pos);
	}
	return;
}


bool Graph::find_structure(const direction& normal_dir, const size_type no_handles){
	vec pos = {_nrVerticesOnAxis/2,_nrVerticesOnAxis/2,_nrVerticesOnAxis/2};
	assert (_nrVerticesOnAxis >=6);

	size_type winner_so_far= 0 ;
	uint max_so_far = 0;
	bool found = false;
	//begin 3D loop
	for (int z=1; z <= _nrVerticesOnAxis-4; ++z){
	int dz = (z%2)? z/2 : -z/2;
	for (int y=1; y <= _nrVerticesOnAxis-4; ++y){
	int dy = (y%2)? y/2 : -y/2;
	for (int x=1; x <= _nrVerticesOnAxis-4; ++x){
	int dx = (x%2)? x/2 : -x/2;
		//current proposed position
		pos.at(0) = _nrVerticesOnAxis/2 + dx;
		pos.at(1) = _nrVerticesOnAxis/2 + dy;
		pos.at(2) = _nrVerticesOnAxis/2 + dz;
		size_type id = idFromCoordinate(pos);

		if(is_proper_node(pos) && get_orientation(id) == normal_dir)
		{
			if(_neighbors[id].size() < 4)
				continue;
			
			uint cur_score = 0; //count all connections one
			for(auto nb : _neighbors.at(id)){
				cur_score += _parent._boxes.at(nb.first)._neighbors[nb.second].size();
			}
			if (cur_score > max_so_far){
				found = true;
				winner_so_far = id;
				max_so_far = cur_score;
			}
		}


	}}}// end 3D loop
	if (found){
		_structure_pos = winner_so_far;
		_structureExists=true;
	}
	return found;
}


const bool Graph::on_boundary(size_type pos, const direction dir){
	switch(dir){
		case right:{return pos%_nrVerticesOnAxis == (_nrVerticesOnAxis-1);}
		case up:{return (pos/_nrVerticesOnAxis)%_nrVerticesOnAxis == (_nrVerticesOnAxis-1);}
		case back:{return (pos/_nrVerticesOnAxis/_nrVerticesOnAxis) %_nrVerticesOnAxis == _nrVerticesOnAxis-1;}
		case left:{return pos%_nrVerticesOnAxis == 0;}
		case down:{return (pos/_nrVerticesOnAxis)%_nrVerticesOnAxis == 0;}
		case front:{return (pos/_nrVerticesOnAxis/_nrVerticesOnAxis)%_nrVerticesOnAxis == 0;}
	}
	return false;
}


bool Graph::handle_dir_exists(const size_type& pos, const direction& dir){
	assert(dir != get_orientation(_structure_pos));
	// TODO get next
	vec pos1 = coordFromId(pos);
	for(auto h : _neighbors.at(pos)){
		vec coord = coordFromId(h.second);
		if (coord.at(dir%3) > pos1.at(dir%3) && dir <3)
			return true;
		if (coord.at(dir%3) < pos1.at(dir%3) && dir >=3)
			return true;
	}
	return false;
}


size_type Graph::get_structure_handle(const size_type& pos, const direction& dir){
	assert(dir != get_orientation(_structure_pos));
	// TODO get next
	vec pos1 = coordFromId(pos);
	for(auto h : _neighbors.at(pos)){
		vec coord = coordFromId(h.second);
		if (coord.at(dir%3) > pos1.at(dir%3) && dir <3)
			return h.second;

		if (coord.at(dir%3) < pos1.at(dir%3) && dir >=3)
			return h.second;
	}
}


bool Graph::is_proper_node(size_type id) const{
	vec coord = coordFromId(id);
	if (1 == (coord.at(2) % 2)){
		if( 0 == (coord.at(0) % 2) or 0 == (coord.at(1) % 2) )
			return true;
		return false;
	}
	else{
		if( 1 == (coord.at(0) % 2) or 1 == (coord.at(1) % 2) )
			return true;
		return false;
	}
}

bool Graph::is_proper_node(const vec & coord) const {
	if (1 == (coord.at(2) % 2)){
		if( 0 == (coord.at(0) % 2) or 0 == (coord.at(1) % 2) )
			return true;
		return false;
	}
	else{
		if( 1 == (coord.at(0) % 2) or 1 == (coord.at(1) % 2) )
			return true;
		return false;
	}
}

const int Graph::get_delta(const direction dir){
	switch(dir){
		case front : return -1*_nrVerticesOnAxis*_nrVerticesOnAxis;
		case back : return 1*_nrVerticesOnAxis*_nrVerticesOnAxis;
		case down : return -1*_nrVerticesOnAxis;
		case up : return 1*_nrVerticesOnAxis;
		case left : return -1;
		case right : return +1;
	}
}