#pragma once
#include <iostream>
#include <vector>

#include "DNA.h"

class Genome
{
public:
	~Genome();
	static void generateDNAPopulation(int populationSize, std::vector<DNA>* population);
	static DNA createChild(std::vector<DNA> *population, float mutationRate, float mutationStrength);
};