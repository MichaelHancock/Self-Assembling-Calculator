#pragma once

#include <array>
#include <algorithm>
#include <assert.h> 
#include <ctime>
#include <fstream>
#include <map>
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
	std::map<std::string, double> stats;
	std::string input;
	std::string target;
	std::pair<DanaLineSet, double> resultFunction;
	double targetComponentLength = 6;
	const std::string outputPath = "Resources/calculator.dn";
	const std::string compilePath = "Resources/calculator.o";

	//	Private member functions
	int getRandomNumber(int, int);
	std::string getVariableName(int lineNumber);
	std::vector<DanaLine> getAllLineVariations(int, const std::vector<DanaVariable> &);
	DanaLineSet correctVariableNames(DanaLineSet);
	std::string runSystemCommand(std::string);
	bool compileFunction(std::string);
	bool testFunction(std::string, std::string, std::string);
	void writeDataFile(std::string, std::string);
	std::string inline funcToString(DanaLineSet, std::string);
	DanaVariable findReturnVariable(DanaLineSet);

public:
	//	Constructor
	GeneticTransform(std::vector<DanaLineSet> &, std::vector<DanaFunction> &,
		std::string, std::string, std::string);	

	//	Public member functions
	DanaLineSet insertLine(DanaLineSet); 
	DanaLineSet modifyLine(DanaLineSet); 
	DanaLineSet deleteLine(DanaLineSet);  
	DanaLineSet crossover(const DanaLineSet &, const DanaLineSet &);
	DanaLineSet removeUnnecessaryLines(DanaLineSet, DanaVariable, bool);
	void test();
	void rank();
	std::string cycleGeneration();
	std::vector<std::pair<DanaLineSet, double>> getPopulation();
	std::map<std::string, double> getLastCycleStats();
	void tuneFitnessFunction(const std::string, double);
};

