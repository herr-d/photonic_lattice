#ifndef PURIFY_HPP
#define PURIFY_HPP

#define LARGE_COST 65535


#include <basic_includes.hpp>
#include <Graph.hpp>
#include <Parallel.hpp>
#include <utility>
#include <unordered_map>

class Parallel;


//define came from map
typedef std::unordered_map<position,position> map_type;


class Astar
{
private:
	uint _direction;
	position _goal;
    size_type _beginningBox, _endBox;
	std::unordered_set<position> _closedSet;
	std::unordered_set<position> _openSet;
	map_type _cameFrom;
	std::unordered_map<position,size_type> _gScore;
	Parallel & _parent;

public:
	Astar(uint direction, position startid, position goalid, Parallel & parent);

	size_type getNodeCost(const position& id) const;
	size_type getGoalDistanceEstimate(const position & id0, const position & id1) const;
	
	bool runAstar(uint iterations);
	size_type getfScore(const position & id0) const;
	bool reconstructPath();
	bool isGoal(position & id) const;

};

#endif