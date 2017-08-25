#pragma once

#include <ctime>
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>

#include "DanaVariable.h"

typedef std::pair<std::string, std::string> pairOfStrings;
typedef std::vector<pairOfStrings> vectorOfPairs;

class DanaGrammer
{
public:
	//	Constructors
	DanaGrammer(std::string, std::vector<DanaVariable>);

	//	Public member functions
	std::string nextLine(int, int, std::string);
private:

	//	Private member functions
	int getRandomNumber(int, int);
	vectorOfPairs splitStringIntoPairs(std::string, const char *);
	std::string generateType();
	std::string generateVariableName();
	std::string generateFunctionCall();
	std::string getMatchingParam(std::string);
	pairOfStrings assignFromArray();

	//	Private member variables
	vectorOfPairs availableFunctions;
	std::vector<DanaVariable> availableVariables;
};

