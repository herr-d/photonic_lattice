#include <basic_includes.hpp>
#include <Graph.hpp>
#include <Parallel.hpp>
#include <Purify.hpp>
#include <gtest/gtest.h>

std::mt19937_64 rng;

//TESTS: Parallell

TEST (Parallel, Initialization){
	Parallel newsimulation(3, 3, 3, 8);
	EXPECT_EQ(0,newsimulation._boxes.size());
}


//TESTS: SquareLattice

TEST (SquareLattice, Initialization){
	Parallel newsimulation(3, 3, 3, 8);
	EXPECT_EQ(3,newsimulation._box_lattice.get_x());
	EXPECT_EQ(3,newsimulation._box_lattice.get_y());
	EXPECT_EQ(3,newsimulation._box_lattice.get_z());
}

TEST (SquareLattice, idFromCoordinate){
	Parallel newsimulation(2, 3, 4, 8);
	vec pos({0,0,0});
	EXPECT_EQ(0,newsimulation._box_lattice.idFromCoordinate(pos));
	pos = {1,2,0};
	EXPECT_EQ(5,newsimulation._box_lattice.idFromCoordinate(pos));
	pos = {0,0,1};
	EXPECT_EQ(6,newsimulation._box_lattice.idFromCoordinate(pos));
}

TEST (SquareLattice, coordFromId){
	Parallel newsimulation(2, 3, 4, 8);
	vec pos({0,0,0});
	EXPECT_EQ(pos,newsimulation._box_lattice.coordFromId(0));
	pos = {1,2,0};
	EXPECT_EQ(pos,newsimulation._box_lattice.coordFromId(5));
	pos = {0,0,1};
	EXPECT_EQ(pos,newsimulation._box_lattice.coordFromId(6));
}

TEST (SquareLattice, loc_to_global_id){
	Parallel newsimulation(2, 3, 4, 2);
	vec pos({0,0,0});
	vec boxpos({0,0,0});
	newsimulation._box_lattice.loc_to_global(pos, boxpos);
	EXPECT_EQ(0,newsimulation._box_lattice.id(pos));

	pos = {1,0,0};
	boxpos = {0,1,0};
	newsimulation._box_lattice.loc_to_global(pos, boxpos);
	EXPECT_EQ(9,newsimulation._box_lattice.id(pos));

	pos = {0,0,0};
	boxpos = {0,0,1};
	newsimulation._box_lattice.loc_to_global(pos, boxpos);
	EXPECT_EQ(48,newsimulation._box_lattice.id(pos));
}

TEST (SquareLattice, is_proper_node){
	Parallel newsimulation(3, 3, 3, 8);
	EXPECT_FALSE(newsimulation._box_lattice.is_proper_node(0));
	EXPECT_TRUE(newsimulation._box_lattice.is_proper_node(1));
	EXPECT_TRUE(newsimulation._box_lattice.is_proper_node(4));
	EXPECT_TRUE(newsimulation._box_lattice.is_proper_node(9));
	EXPECT_FALSE(newsimulation._box_lattice.is_proper_node(13));
}

TEST (SquareLattice, get_delta){
	Parallel newsimulation(2, 3, 4, 8);
	EXPECT_EQ(1,newsimulation._box_lattice.get_delta(right));
	EXPECT_EQ(2,newsimulation._box_lattice.get_delta(up));
	EXPECT_EQ(6,newsimulation._box_lattice.get_delta(back));
}

TEST (SquareLattice, get_orientation){
	Parallel newsimulation(2, 3, 4, 8);
	EXPECT_EQ(right,newsimulation._box_lattice.get_orientation(1));
	EXPECT_EQ(back,newsimulation._box_lattice.get_orientation(3));
	EXPECT_EQ(up,newsimulation._box_lattice.get_orientation(7));
}


//TESTS: Graph

TEST (Graph, Initialization){
	Parallel newsimulation(1, 1, 1, 8);
	newsimulation._boxes.push_back(Graph(8,newsimulation));
	ASSERT_EQ(1,newsimulation._boxes.size());
	ASSERT_EQ(8*8*8,newsimulation._boxes.at(0).getMaxId());
	ASSERT_FALSE(newsimulation._boxes.at(0).structure_exist());
}

TEST (Graph, add_erase_unfinished){
	Parallel newsimulation(1, 1, 1, 8);
	newsimulation._boxes.push_back(Graph(8,newsimulation));
	ASSERT_EQ(0,Graph::_unfinished.size());

	newsimulation._boxes[0].add_to_unfinished({0,1},{0,2},right);
	ASSERT_EQ(1,Graph::_unfinished.size());
	
	newsimulation._boxes[0].add_to_unfinished({0,2},{0,3},right);
	ASSERT_EQ(2,Graph::_unfinished.size());

	newsimulation._boxes[0].add_to_unfinished({0,1},{0,3},right);
	ASSERT_EQ(2,Graph::_unfinished.at({0,1}).size());

	newsimulation._boxes[0].erase_unfinished({0,1});
	ASSERT_EQ(1,Graph::_unfinished.size());

	newsimulation._boxes[0].erase_unfinished({0,432});
	ASSERT_EQ(1,Graph::_unfinished.size());

	newsimulation._boxes[0].erase_unfinished({0,2});
	ASSERT_EQ(0,Graph::_unfinished.size());
}

TEST (Graph, add_to_neighbors){
	Parallel newsimulation(1, 1, 1, 8);
	newsimulation._boxes.push_back(Graph(8,newsimulation));
	EXPECT_EQ(0,newsimulation._boxes.at(0)._neighbors.size());

	newsimulation._boxes.at(0).add_to_neighbors(0,{0,1});
	EXPECT_EQ(1,newsimulation._boxes.at(0)._neighbors.size());
	
	newsimulation._boxes.at(0).add_to_neighbors(0,{0,1});
	EXPECT_EQ(1,newsimulation._boxes.at(0)._neighbors.size());
	EXPECT_EQ(1,newsimulation._boxes.at(0)._neighbors.at(0).size());
	
	newsimulation._boxes.at(0).add_to_neighbors(1,{0,1});
	EXPECT_EQ(2,newsimulation._boxes.at(0)._neighbors.size());
	EXPECT_EQ(1,newsimulation._boxes.at(0)._neighbors.at(0).size());
	
}

TEST (Graph, next_connection){
	Parallel newsimulation(2, 1, 1, 8);
	newsimulation._boxes.push_back(Graph(8,newsimulation));
	newsimulation._boxes.push_back(Graph(8,newsimulation));
	position pos({0,0});
	
	newsimulation._boxes.at(0).next_connection({0,0},right,{0,0});
	EXPECT_EQ(1,Graph::_unfinished.size());
	EXPECT_EQ(pos,Graph::_unfinished.at({0,1}).at(0).first);
	EXPECT_EQ(right,Graph::_unfinished.at({0,1}).at(0).second);

	newsimulation._boxes.at(0).next_connection({0,0},up,{0,0});
	EXPECT_EQ(2,Graph::_unfinished.size());
	EXPECT_EQ(pos,Graph::_unfinished.at({0,8}).at(0).first);

	newsimulation._boxes.at(0).next_connection({0,0},back,{0,0});
	EXPECT_EQ(3,Graph::_unfinished.size());
	EXPECT_EQ(pos,Graph::_unfinished.at({0,64}).at(0).first);

	newsimulation._boxes.at(0).next_connection({0,7},right,{0,0});
	EXPECT_EQ(4,Graph::_unfinished.size());
	EXPECT_EQ(pos,Graph::_unfinished.at({1,0}).at(0).first);

	Graph::_unfinished.clear();
}

TEST (Graph, get_orientation){
	Parallel newsimulation(1, 1, 1, 3);
	newsimulation._boxes.push_back(Graph(3,newsimulation));
	EXPECT_EQ(back,newsimulation._boxes.at(0).get_orientation(4));
	EXPECT_EQ(right,newsimulation._boxes.at(0).get_orientation(1));
	EXPECT_EQ(up,newsimulation._boxes.at(0).get_orientation(10));

}


TEST (Graph, finish_connection){
	Parallel newsimulation(1, 1, 1, 3);
	newsimulation._boxes.push_back(Graph(3,newsimulation));
	newsimulation._boxes.at(0).next_connection({0,0},right,{0,0});
	newsimulation._boxes.at(0).finish_connection({0,1},right);
	EXPECT_EQ(2,newsimulation._boxes.at(0)._neighbors.size());

	newsimulation._boxes.at(0).next_connection({0,0},up,{0,0});
	newsimulation._boxes.at(0).finish_connection({0,3},up);
	EXPECT_EQ(3,newsimulation._boxes.at(0)._neighbors.size());
}


TEST (Graph, generate_connections){
	Graph::_unfinished.clear();
	Parallel newsimulation(1, 1, 1, 3);
	newsimulation._boxes.push_back(Graph(3,newsimulation));

	newsimulation._boxes.at(0).generate_connections(0,1);
	EXPECT_EQ(0,newsimulation._boxes.at(0)._unfinished.size());
}

TEST (Graph, find_structure){
	Graph::_unfinished.clear();
	Parallel newsimulation(1, 1, 1, 8);
	newsimulation._boxes.push_back(Graph(8,newsimulation));
	newsimulation._boxes.at(0).generate_connections(0,1);

	newsimulation._boxes.at(0).find_structure(right,4);
	ASSERT_TRUE(newsimulation._boxes.at(0).structure_exist());
}

//TESTS: Graph

TEST (Purify, getNodeCost){
	Graph::_unfinished.clear();
	Parallel newsimulation(2, 1, 1, 8);
	newsimulation.gen_lattice(0.);

	position pos0({0,228});
	position pos1({1,228});

	Astar path(right, pos0, pos1, newsimulation);

	position pos2({1,2});

	EXPECT_EQ(0,path.getNodeCost(pos0));
	EXPECT_EQ(LARGE_COST,path.getNodeCost(pos2));

}

TEST (Purify, getGoalDistanceEstimate){
	Graph::_unfinished.clear();
	Parallel newsimulation(2, 1, 1, 8);
	newsimulation.gen_lattice(0.);

	position pos0({0,228});
	position pos1({1,228});

	Astar path(right, pos0, pos1, newsimulation);

	EXPECT_EQ(8,path.getGoalDistanceEstimate(pos0,pos1));
	position pos3({0,229});
	EXPECT_EQ(1,path.getGoalDistanceEstimate(pos0,pos3));
}

TEST (Purify, isGoal){
	Graph::_unfinished.clear();
	Parallel newsimulation(2, 1, 1, 8);
	newsimulation.gen_lattice(0.);

	position pos0({0,228});
	position pos1({1,228});
	
	Astar path(right, pos0, pos1, newsimulation);
	
	EXPECT_TRUE(path.isGoal(pos1));
	EXPECT_FALSE(path.isGoal(pos0));
}


TEST (Purify, runAstar){
	Graph::_unfinished.clear();
	Parallel newsimulation(2, 1, 1, 8);
	newsimulation.gen_lattice(0.);

	position pos0({0,228});
	position pos1({1,228});
	
	Astar path(right, pos0, pos1, newsimulation);

	ASSERT_TRUE(path.runAstar(8));

	
}



int main(int argc, char* argv[])
{
	rng.seed(42);
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
