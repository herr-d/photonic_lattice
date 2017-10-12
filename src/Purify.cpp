#include <Purify.hpp>

Astar::Astar(uint direction, position startid, position goalid, Parallel & parent)
	: _direction(direction), _goal(goalid), _closedSet(),
		_parent(parent), _openSet(), _cameFrom(), _gScore(),
		_beginningBox(startid.first), _endBox(goalid.first)
{

	_openSet.insert(startid);
	_gScore.insert({startid,0});

	// add already measured qubits and their neighbors to closed set
	for(auto i : _parent._boxes.at(startid.first)._ymeasurements){
		position tmp(startid.first,i);
		if (_closedSet.find(tmp) == _closedSet.end())
			_closedSet.insert(tmp);

		for(auto j : _parent._boxes.at(tmp.first)._neighbors.at(tmp.second)){
			if (_closedSet.find(j) == _closedSet.end())
				_closedSet.insert(j);
		}
	}

	for(auto i :_parent._boxes.at(goalid.first)._ymeasurements){
		position tmp(goalid.first,i);
		if (_closedSet.find(tmp) == _closedSet.end())
			_closedSet.insert(tmp);

		for(auto j : _parent._boxes.at(tmp.first)._neighbors.at(tmp.second)){
			if (_closedSet.find(j) == _closedSet.end())
				_closedSet.insert(j);
		}
	}
	return;
}


size_type Astar::getNodeCost(const position& id) const {
	auto it = _gScore.find(id);
	if (it != _gScore.end()){
		return it->second;
	}
	return LARGE_COST;
}


size_type Astar::getGoalDistanceEstimate(const position & id0, const position & id1) const{
	//manhatten distance: maybe change
	size_type dist = 0;
	vec pos0 = _parent._boxes.at(id0.first).coordFromId(id0.second);
	vec pos1 = _parent._boxes.at(id1.first).coordFromId(id1.second);

	if(id0.first == _endBox)
		pos0.at(_direction) += _parent._boxes.at(id0.first).getnodesaxis();
	if(id1.first == _endBox)
		pos1.at(_direction) += _parent._boxes.at(id1.first).getnodesaxis();

	for(int i =0; i<3; ++i){
		dist += std::abs(static_cast<int64_t>(pos0.at(i)) - static_cast<int64_t>(pos1.at(i)));
	}

	return dist;
}

size_type Astar::getfScore(const position & id) const {
	return getGoalDistanceEstimate(id,_goal) + getNodeCost(id);
}

bool Astar::isGoal(position & id) const{
	return std::get<0>(id) == std::get<0>(_goal) &&
		std::get<1>(id) == std::get<1>(_goal);
}

bool Astar::runAstar(uint iterations){
	//main loop
	for(uint i = 0; i <= iterations; ++i){
		if(_openSet.empty()){
			//std::cout<< "empty open set" << std::endl;
			return false;
		}
		//get position with the lowest distance
		position current;
		size_type dist = LARGE_COST;
		for(auto it: _openSet){
			if(getfScore(it) <= dist){
				current = it; 
				dist = getfScore(it);
			}
		}
		//move current from the open set to the closed set
		_openSet.erase(current);
		_closedSet.insert(current);


		//DEBUG output
		/*
		{
			vec tmp = _parent._boxes.at(current.first).coordFromId(current.second);
			std::cout << "current pos:" << current.first;
			for(auto i : tmp)
				std::cout << " " << i;
			std::cout << std::endl;
			tmp = _parent._boxes.at(_goal.first).coordFromId(_goal.second);
			std::cout << "goal pos:" << _goal.first;
			for(auto i : tmp)
				std::cout << " " << i;
			std::cout << std::endl;
			std::cout << "dist: " << getfScore(current) << std::endl;
			std::cout << "direction " << _direction << "\n" << std::endl;
		}*/

		//found goal?
		if (isGoal(current)){
			//std::cout<< "found goal" << std::endl;
			return true;
		}

		for(auto nb : _parent._boxes.at(current.first)._neighbors.at(current.second)){

			if(nb.first != _beginningBox && nb.first != _endBox)
				continue;

			if (_closedSet.find(nb) != _closedSet.end())
				continue;

			if (_openSet.find(nb) == _openSet.end())
				_openSet.insert(nb);

			//calculate distance
			size_type tentative_dist = getNodeCost(current) + 1;

			if (tentative_dist >= getNodeCost(nb))
				continue;	//not a better path

			//this path is the best untill now. Record it!
			if (_cameFrom.find(nb) != _cameFrom.end())
				_cameFrom.erase(nb);

			_cameFrom.insert({nb,current});

			if (_gScore.find(nb) == _gScore.end())
				_gScore.erase(nb);

			_gScore.insert({nb,tentative_dist});
		}

	}
	//std::cout<< "no iterations left" << std::endl;
	return false;
}

int Astar::reconstructPath(){
	_parent._boxes.at(_goal.first)._ymeasurements.insert(_goal.second);
	int count = 1;

	position current = _goal;
	
	for(;;)
	{
		auto it = _cameFrom.find(current);
		++count;
		if (it == _cameFrom.end())
			return count;

		current = it->second;

		//in first box
		_parent._boxes.at(current.first)._ymeasurements.insert(current.second);
	}
}