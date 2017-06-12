#pragma once
#include <algorithm>
#include <assert.h>   
#include <string>
#include <sstream>
#include <vector>
#include "Assembler.h"
#include "DanaVariable.h"
#include "DanaFunction.h"
#include "DanaLine.h"
#include "LineStore.h"

typedef std::vector<DanaLine> ListOfLines;
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

	//	Private member functions
	ListOfLines getAllLineVariations(int, ListOfVariables);
	std::string getVariableName(int);
	ListOfStrings split(std::string);
public:
	//	Public member functions
	DanaGrammer(std::string, ListOfFunctions, DanaVariable, int);
	ListOfStrings generateAllFunctonInstances();
};