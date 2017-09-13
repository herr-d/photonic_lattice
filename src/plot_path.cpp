#include <basic_includes.hpp>
#include <Graph.hpp>
#include <Parallel.hpp>
#include <Purify.hpp>
#include <sstream>
#include <ctime>

//global rng: (Todo: needs to be threadsafe)
std::mt19937_64 rng;

int main(int argc, char *argv[])
{

	//handle input
	if (argc != 4){
		std::cerr << "Usage: " << argv[0] << " size" << " prob"<< " seed"<< std::endl;
		std::cerr << "size: distance between start and finish" << std::endl;
		std::cerr << "prob: probability for failure of fusion gate" << std::endl;
		std::cerr << "seed: seed for the random number generator" << std::endl;
		return 1;
	}

	size_type boxsize = 0;
	double probab = 0.;
	uint seed = 1;

	{
		std::stringstream str;
		str << argv[1]; 	//boxsize
		str >> boxsize;
		str.clear();
		str << argv[2];		//probab
		str >> probab;
		str.clear();
		str << argv[3];		//seed
		str >> seed;
	}

	//seed rng
	rng.seed(seed);
	size_type iterations_astar = boxsize*boxsize*boxsize;


	// generate the individual boxes
	Parallel newsimulation(2, 1, 1, boxsize);
	//generate errors with probability 
	newsimulation.gen_lattice(probab);

	//writeout
	newsimulation.writeout_graph("graph.json");

	newsimulation._boxes.at(0).find_structure(up, 0);
	newsimulation._boxes.at(1).find_structure(up, 0);

	position pos0(0,newsimulation._boxes.at(0).get_structure()+1);
	position pos1(1,newsimulation._boxes.at(1).get_structure()-1);


	//now perform path finding
	Astar path(right, pos0, pos1, newsimulation);
	if(path.runAstar(iterations_astar)){
		path.reconstructPath();
		newsimulation._boxes.at(0).measure_structure();
		newsimulation._boxes.at(1).measure_structure();
		newsimulation.writeout_ymeasure("ymeasure.json");
		std::cout << "SUCCESS" << std::endl;
	}

	return 0;
}