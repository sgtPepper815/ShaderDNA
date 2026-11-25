#include <glm/glm.hpp>
#include <random>

#include "Genome.h"


void Genome::generateDNAPopulation(int populationSize, std::vector<DNA>* population)
{
	std::srand(static_cast<unsigned int>(std::time(nullptr)));

	for(int i = 0; i <= populationSize; i++)
	{ 
		DNA dna;
		dna.frequency = 6.0f + static_cast<float>(rand()) / RAND_MAX * 12.0f;
		dna.distortion = 0.5f + static_cast<float>(rand()) / RAND_MAX * 3.0f;
		dna.speed = static_cast<float>(rand()) / RAND_MAX * 7.0f;
		dna.colorA = glm::vec3(static_cast<float>(rand()) / static_cast<float>(RAND_MAX),
			static_cast<float>(rand()) / static_cast<float>(RAND_MAX),
			static_cast<float>(rand()) / static_cast<float>(RAND_MAX));
		dna.colorB = glm::vec3(static_cast<float>(rand()) / static_cast<float>(RAND_MAX),
			static_cast<float>(rand()) / static_cast<float>(RAND_MAX),
			static_cast<float>(rand()) / static_cast<float>(RAND_MAX));

		population->push_back(dna);
	}


}

DNA Genome::createChild(std::vector<DNA>* population, float mutationRate, float mutationStrength)
{
	constexpr int num_values = 5;
	int values[num_values];
	DNA child;

	// crossover: randomly select attributes from the population to create a child DNA
	// choose for each attribute a random parent attribute from the population
	for (int i = 0; i < 5; i++)
	{
		values[i] = rand() % population->size();
	}

	child.frequency = population->at(values[0]).frequency;
	child.distortion = population->at(values[1]).distortion;
	child.speed = population->at(values[2]).speed;
	child.colorA = population->at(values[3]).colorA;
	child.colorB = population->at(values[4]).colorB;

	// mutation: randomly change some attributes of the child DNA
	for (int i = 0; i < num_values; i++)
	{
		if (static_cast<float>(rand()) / (static_cast<double>(RAND_MAX) + 1.0) < mutationRate)
		{
			values[i] += (static_cast<float>(rand()) / RAND_MAX * 2.0f - 1.0f)
				* mutationStrength;
		}
	}
	

	return child;

}
