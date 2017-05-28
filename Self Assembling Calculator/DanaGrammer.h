#pragma once
#include <algorithm>
#include <assert.h>   
#include <string>
#include <sstream>
#include <vector>
#include "Assembler.h"
#include "LineStore.h"
#include "DanaVariable.h"
#include "DanaFunction.h"

typedef std::vector<DanaFunction> ListOfFunctions;
typedef std::vector<DanaVariable> ListOfVariables;
typedef std::vector<std::string> ListOfStrings;

class DanaGrammer {
private: 
	//	Private member variables
	ListOfFunctions knownFunctions;
	ListOfVariables knownVariables;
	ListOfStrings functionHeader;
	LineStore store;
	int numberOfGeneratedLines;

	//	Private member functions
	ListOfStrings getAllPossibleLines(ListOfFunctions, ListOfVariables, int);
	bool validateVariable(ListOfStrings);
	std::string getVariableName(int);
	ListOfStrings split(std::string);

public:
	//	Public member functions
	DanaGrammer(std::string, ListOfVariables);
	ListOfStrings generateAllFunctonInstances();
};