#pragma once

#include <array>
#include <algorithm>
#include <assert.h> 
#include <ctime>
#include <fstream>
#include <memory>
#include <string>
#include <stdlib.h>
#include <vector>
#include "DanaLineSet.h"
#include "DanaLine.h"
#include "DanaFunction.h"
#include "DanaVariable.h"

class GeneticTransform {
private:
	//	Private member variables
	std::vector<std::pair<DanaLineSet, double>> populaton;
	std::vector<DanaFunction> knownFunctions;
	std::vector<std::string> functionHeader;
	std::string input;
	std::string target;
	std::string resultFunction;
	const std::string outputPath = "Resources/calculator.dn";
	const std::string compilePath = "Resources/calculator.o";

	//	Private member functions
	int getRandomNumber(int, int);
	std::string getVariableName(int lineNumber);
	std::vector<DanaLine> getAllLineVariations(int, std::vector<DanaVariable> &);
	DanaLineSet correctVariableNames(DanaLineSet);
	std::string runSystemCommand(std::string);
	bool compileFunction(std::string);
	bool testFunction(std::string, std::string, std::string);
	void writeDataFile(std::string, std::string);
	std::string collpaseVector(std::vector<std::string> &);

public:
	//	Constructor
	GeneticTransform(std::vector<DanaLineSet> &, std::vector<DanaFunction> &, 
		std::string, std::string, std::string);	

	//	Public member functions
	DanaLineSet insertLine(DanaLineSet); 
	DanaLineSet modifyLine(DanaLineSet); 
	DanaLineSet deleteLine(DanaLineSet);  
	void crossover(); //
	void test(); 
	void rank(); 
	std::string cycleGeneration();
	std::vector<std::pair<DanaLineSet, double>> getPopulation();

	// Destructor 
	~GeneticTransform();
};

