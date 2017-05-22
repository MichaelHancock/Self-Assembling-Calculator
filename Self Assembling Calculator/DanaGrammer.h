#pragma once
#include <vector>
#include <assert.h>   
#include <string>
#include <algorithm>
#include "DanaVariable.h"
#include "DanaFunction.h"
#include "LineStore.h"

typedef std::vector<DanaFunction> ListOfFunctions;
typedef std::vector<DanaVariable> ListOfVariables;
typedef std::vector<std::string> ListOfStrings;

class DanaGrammer {
private: 
	ListOfFunctions knownFunctions;
	ListOfVariables knownVariables;
	ListOfStrings generatedFunctions;
	ListOfStrings functionHeader;
	LineStore store;
	std::string collpaseVector(ListOfStrings lines);
	std::string getVariableName(int lineNumber);

public:
	DanaGrammer(std::string required, ListOfVariables knownVariables);
	ListOfStrings generateAllFunctonInstances(int numberOfLines);
};