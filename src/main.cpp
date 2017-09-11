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
	if (argc != 5){
		std::cerr << "Usage: " << argv[0] << " boxsize" << " Nbox" << " failure_rate"<< " seed"<< std::endl;
		std::cerr << "boxsize: edge size of the box (total no of qubits is boxsize^3)" << std::endl;
		std::cerr << "Nbox: How many boxes are in one direction (total number of boxes is Nbox^3)" << std::endl;
		std::cerr << "failure_rate: probability for a faulty connections between nodes (example: 0.25)" << std::endl;
		std::cerr << "seed: seed for the random number generator" << std::endl;
		return 1;
	}

	size_type boxsize = 0;
	size_type Nbox = 0;
	double probab = 0.;
	uint seed = 1;

	{
		std::stringstream str;
		str << argv[1]; 	//boxsize
		str >> boxsize;
		str.clear();
		str << argv[2];		//Nbox
		str >> Nbox;
		str.clear();
		str << argv[3];		//failure_rate
		str >> probab;
		str.clear();
		str << argv[4];		//seed
		str >> seed;
	}

	//seed rng
	rng.seed(seed);
	
	// generate the individual boxes
	Parallel newsimulation(Nbox, Nbox, 3, boxsize);

	//std::cout << "set up interface" << std::endl;

	//generate errors with probability 
	newsimulation.gen_lattice(probab);

	//std::cout << "generated lattice" <<std::endl;

	//now loop through boxes and find viable structure positions
	newsimulation.find_structures();

	//std::cout << "found structures" << std::endl;

	//now find paths between the boxes
	newsimulation.path_finding();

	//std::cout << "found paths" << std::endl;

	newsimulation.statistics_output(boxsize, probab);

	//writeout of complete lattice (large filesize for larger lattices)
	//newsimulation.writeout_graph("graph.json");

	//writeout of paths (also large file size)
	//newsimulation.writeout_ymeasure("ymeasure.json");

	return 0;
}
