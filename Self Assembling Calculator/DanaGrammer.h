#pragma once
#include <vector>
#include "DanaVariable.h"
#include "DanaFunction.h"

typedef std::vector<DanaFunction> ListOfFunctions;
typedef std::vector<DanaVariable> ListOfVariables;
typedef std::vector<std::string> ListOfStrings;

class DanaGrammer {
private: 
	ListOfFunctions knownFunctions;
	ListOfVariables knownVariables;
	ListOfStrings generatedFunctions;
	ListOfStrings functionHeader;
	std::string collpaseVector(ListOfStrings lines);

public:
	DanaGrammer(std::string required, ListOfVariables knownVariables);
	ListOfStrings generateAllFunctonInstances(int numberOfLines);
};