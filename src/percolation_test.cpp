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
	uint iterations = 10;
	uint counter = 0;

	size_type iterations_astar = boxsize*boxsize*boxsize;



	for (uint i = 0; i < iterations; ++i)
	{
		// generate the individual boxes
		Parallel newsimulation(2, 1, 1, boxsize);

		//generate errors with probability 
		newsimulation.gen_lattice(probab);

		vec coord({boxsize/2,boxsize/2,boxsize/2});
		size_type id = newsimulation._boxes.at(0).idFromCoordinate(coord);
		if (!newsimulation._boxes.at(0).is_proper_node(id))
			++id;

		position pos0(0,id);
		position pos1(1,id);

		//now perform path finding
		Astar path(right, pos0, pos1, newsimulation);
		if(path.runAstar(iterations_astar)){
			++counter;
		}
	}
	double result = static_cast<double>(counter) / static_cast<double>(iterations);
	std::cout << boxsize << " " << probab << " " << result << std::endl;
	return 0;
}