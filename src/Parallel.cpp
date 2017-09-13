#include <Parallel.hpp>
#include <json.hpp>
#include <fstream>
#include <string>
#include <Purify.hpp>



bool SquareLattice::on_boundary(size_type pos, const direction dir){
	switch(dir){
		case right:{return pos%_x == (_x-1);}
		case up:{return (pos/_x)%_y == (_y-1);}
		case back:{return (pos/_x/_y) %_z == _z-1;}
		case left:{return pos%_x == 0;}
		case down:{return (pos/_x)%_y == 0;}
		case front:{return (pos/_x/_y)%_z == 0;}
	}
	return false;
}


size_type SquareLattice::get_delta(const direction& dir) const {
	switch(dir){
		case front : return -1*_x*_y;
		case back : return 1*_x*_y;
		case down : return -1*_x;
		case up : return 1*_x;
		case left : return -1;
		case right : return +1;
	}
}

bool SquareLattice::is_proper_node(const size_type& id) const{
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

void SquareLattice::loc_to_global(vec& loc_coord, const vec & boxpos) const{
	for(size_type i=0; i< boxpos.size(); ++i){
		loc_coord.at(i) += (_parent._box_size)*boxpos.at(i);
	}
}


direction SquareLattice::get_orientation(const size_type& pos) const{
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


uint64_t SquareLattice::id(const vec &coord) const{
	return coord.at(0) + coord.at(1)*_x*_parent._box_size
		+ coord.at(2)*_x*_y*_parent._box_size*_parent._box_size;
}


void Parallel::gen_lattice(const double probability){
	//first check if empty
	assert(_boxes.size()==0);
	//TODO: parallize
	std::cout << "x: " << _box_lattice.get_x() << std::endl;
	std::cout << "y: " << _box_lattice.get_y() << std::endl;
	std::cout << "z: " << _box_lattice.get_z() << std::endl;
	for(size_type z = 0; z < _box_lattice.get_z(); ++z){
	for(size_type y = 0; y < _box_lattice.get_y(); ++y){
	for(size_type x = 0; x < _box_lattice.get_x(); ++x){
		std::cout << x << " " << y << " " << z << std::endl;
		_boxes.push_back(Graph(_box_size,*this));

	}}}


	for(size_type i = 0; i < _box_lattice.get_x()*_box_lattice.get_y()*_box_lattice.get_z(); ++i){
		_boxes.at(i).generate_connections(i, 1-probability);
	}
	return;
}


void Parallel::find_structures(){
	for (size_type b = 0; b<_box_lattice.get_x()*_box_lattice.get_y()*_box_lattice.get_z(); ++b){
		if(_box_lattice.is_proper_node(b)){
			direction dir = _box_lattice.get_orientation(b);
			if (_boxes.at(b).find_structure(dir, 4))
				continue; //found very good structure
			if (_boxes.at(b).find_structure(dir, 3))
				continue; //found good structure
			if (_boxes.at(b).find_structure(dir, 2)){
				continue; // found ok structure
			}
		}
	}
	return;
}


void Parallel::path_finding(){
	const uint iterations = _boxes.at(0).get_max_id();
	//for each box find a path
	for (size_type b = 0; b < _box_lattice.get_x()*_box_lattice.get_y()*_box_lattice.get_z(); ++b){
		//only if not yet done
		if (_boxes.at(b).isPurified() || !_box_lattice.is_proper_node(b))
		{
			continue;
		}

		for (auto& dir : { right, up, back}){
			if(_box_lattice.on_boundary(b,dir)){
				continue;
			}
			if(_box_lattice.get_orientation(b)==dir){
				continue;
			}

			size_type neigh = _box_lattice.get_neighbor_id(b,dir);

			if(!_boxes.at(b).structure_exist()){
				++_failed_connections;
				continue;
			}

			if(!_boxes.at(neigh).structure_exist()){
				++_failed_connections;
				continue;
			}

			//get handles
			position pos0({b, _boxes.at(b).get_structure()});

			if (!_boxes.at(b).handle_dir_exists(pos0.second, dir)){
				++_failed_connections;
				continue;
			}

			pos0.second = _boxes.at(b).get_structure_handle(pos0.second, dir);
			//flip direction for other handle
			direction dir1 = right;
			if(dir == right)
				dir1 = left;
			else if(dir == up)
				dir1 = down;
			else if(dir == back)
				dir1 = front;
			position pos1({neigh, _boxes.at(neigh).get_structure()});

			if (!_boxes.at(neigh).handle_dir_exists(pos1.second, dir)){
				++_failed_connections;
				continue;
			}

			pos1.second = _boxes.at(neigh).get_structure_handle(pos1.second, dir1);


			//now perform path finding
			Astar path(dir, pos0, pos1, *this);
			if(path.runAstar(iterations)){
				path.reconstructPath();
			}
			else{
				++_failed_connections;
			}
		}
		_boxes.at(b).markAsPurified();
	}
	//now the structures can be added too
	for (size_type b = 0; b<_box_lattice.get_x()*_box_lattice.get_y()*_box_lattice.get_z(); ++b){
		_boxes.at(b).measure_structure();
	}
	return;
}



void Parallel::writeout_ymeasure(const std::string output){
	nlohmann::json j;
	//nodes
	nlohmann::json v;
	for(size_type b=0; b < _box_lattice.get_x()*_box_lattice.get_y()*_box_lattice.get_z(); ++b){
		for(auto n : _boxes.at(b).get_ymeasure()){
			vec loc_coord = _boxes.at(b).coordFromId(n);
			vec box_coord = _box_lattice.coordFromId(b);
			_box_lattice.loc_to_global(loc_coord,box_coord);
			v[std::to_string(_box_lattice.id(loc_coord))] = loc_coord;
		}
	}
	j["nodes"]=v;
	//edges
	std::vector<std::array<uint64_t,2>> e;
	j["edges"] = e;
	
	//now writeout
	std::ofstream file;
	try{
		file.open(output);
	}
	catch (const std::ofstream::failure& e) {
  		std::cout << "Could not output to file" << std::endl;
  		return;
	}
	file << "var graph=";
	file << j.dump(2) << std::endl;
	return;
}


void Parallel::writeout_graph(const std::string output){
	nlohmann::json j;
	//nodes
	nlohmann::json v;
	std::vector<std::array<uint64_t,2>> e;
	for(size_type b=0; b < _box_lattice.get_x()*_box_lattice.get_y()*_box_lattice.get_z(); ++b){
		for(size_type n=0; n < _boxes.at(b).getMaxId(); ++n){
			if(_boxes.at(b).is_proper_node(n)){
				//add node
				vec loc_coord = _boxes.at(b).coordFromId(n);
				vec box_coord = _box_lattice.coordFromId(b);
				_box_lattice.loc_to_global(loc_coord,box_coord);
				v[std::to_string(_box_lattice.id(loc_coord))] = loc_coord;
				//add boxinternal edges
				for(auto nb : _boxes.at(b)._neighbors[n]){
					if(nb.first > b){
						vec nb_coord = _boxes.at(nb.first).coordFromId(nb.second);
						vec box_coord = _box_lattice.coordFromId(nb.first);
						_box_lattice.loc_to_global(nb_coord,box_coord);
						e.insert(e.end(),{_box_lattice.id(loc_coord),_box_lattice.id(nb_coord)});
					}
					if(nb.first == b && nb.second > n){
						vec nb_coord = _boxes.at(nb.first).coordFromId(nb.second);
						vec box_coord = _box_lattice.coordFromId(nb.first);
						_box_lattice.loc_to_global(nb_coord,box_coord);
						e.insert(e.end(),{_box_lattice.id(loc_coord),_box_lattice.id(nb_coord)});
					}
				}
			}
		}
	}
	j["nodes"]=v;
	j["edges"] = e;

	//now writeout
	std::ofstream file;
	try{
		file.open(output);
	}
	catch (const std::ofstream::failure& e) {
  		std::cout << "Could not output to file" << std::endl;
  		return;
	}
	file << "var graph=";
	file << j.dump(2) << std::endl;
	return;
}

void Parallel::statistics_output(size_type boxsize, double orig_error_rate){
	//count connections
	size_type connections = 0;
	for (size_type i=0; i < _box_lattice.get_x()*_box_lattice.get_y()*_box_lattice.get_z(); ++i){
		if (!_box_lattice.is_proper_node(i))
			continue;
		for(auto dir : {right, up, back}){
			//boundary
			if(_box_lattice.on_boundary(i,dir))
				continue;
			//proper neighbors
			if(_box_lattice.is_proper_node(_box_lattice.get_neighbor_id(i,dir))){
				++connections;
			}
		}
	}

	double error_rate = static_cast<double>(_failed_connections) / static_cast<double>(connections);

	//format: distance original_error_rate new_error_rate elapsed time
	std::cout << boxsize << " " << orig_error_rate << " " << error_rate << std::endl;
	return;
}
