#pragma once
#include <algorithm>
#include <assert.h>   
#include <string>
#include <sstream>
#include <vector>
#include "DanaVariable.h"
#include "DanaFunction.h"

typedef std::vector<DanaFunction> ListOfFunctions;
typedef std::vector<DanaVariable> ListOfVariables;
typedef std::vector<std::string> ListOfStrings;

class DanaGrammer {
private: 

	//	Private member functions
	std::string getVariableName(int);
	ListOfStrings split(std::string);

public:
	//	Public member functions
	DanaGrammer(std::string, ListOfVariables);
	ListOfStrings generateAllFunctonInstances();
};