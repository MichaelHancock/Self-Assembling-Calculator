#include "DanaGrammer.h"

/* --- Constructor --- */

DanaGrammer::DanaGrammer(std::string requiredIterfaces, std::vector<DanaVariable> variables) {
	srand((unsigned int)time(0));

	//Add passed variables to list of availableVariables
	availableVariables = variables;

	//THIS CODE IS TEMPORARY - functions should be generated dynamically
	vectorOfPairs requires = splitStringIntoPairs(requiredIterfaces, ",");
	pairOfStrings a = std::make_pair("uInt.intFromString", "char");
	pairOfStrings b = std::make_pair("adder.add", "int int");
	availableFunctions.push_back(a);
	availableFunctions.push_back(b);
}

/* --- Public member functions --- */

std::string DanaGrammer::nextLine(int lineIndex, int maxLines) {	
	//Check if last line
	if (lineIndex == maxLines - 1) 
		return "return " + availableVariables.at(availableVariables.size() -1).name;
		

	//Check if any iterable variables exist in availableVariables
	bool chanceOfFunction = false;
	bool chanceOfArrayAssign = false;
	for (int i = 0; i < availableVariables.size(); i++) {
		if (availableVariables.at(i).size != NULL)
			chanceOfArrayAssign = true;
		else if (availableVariables.at(i).type.size() > 0 && availableVariables.at(i).size == NULL) {
			//Variable found that is not an array
			chanceOfFunction = true;
		}
	}

	//Generate line of valid dana syntax
	std::string type = generateType();
	const std::string name = generateVariableName();
	std::string action = "";
	
	//Based on variables available, decide on what to assign to action
	if (chanceOfFunction && !chanceOfArrayAssign)
		//Varibales are available, but no arrays
		action = generateFunctionCall();
	else if (!chanceOfFunction && chanceOfArrayAssign) {
		//No normal variables are available, but arrays are
		pairOfStrings arrayAssign = assignFromArray();
		type = arrayAssign.first;
		action = arrayAssign.second;
	}
	else if (chanceOfFunction && chanceOfArrayAssign)
		//Both options are available, give functionCall a 80% chance over array assign
		if (getRandomNumber(0, 100) < 80)
			action = generateFunctionCall();
		else {
			pairOfStrings arrayAssign = assignFromArray();
			type = arrayAssign.first;
			action = arrayAssign.second;
		}
	else
		//Not enough information available
		return NULL;

	DanaVariable newVar = DanaVariable(name, type);
	availableVariables.push_back(newVar);

	return type + " " + name + " = " + action;
}

/* --- Private member functions --- */

int DanaGrammer::getRandomNumber(int low, int high) {
	if (low == high)
		return low;

	if (low > high)
		std::swap(low, high);

	//Generate random number
	high = (high - low) + 1;
	return rand() % high + low;
}

vectorOfPairs DanaGrammer::splitStringIntoPairs(std::string text, const char* delimiter) {
	//Split string into pairs of strings and store in vector
	vectorOfPairs requiredInterfaces;
	std::string bufferInterface = "", bufferObject = "";

	//Remove characters from string
	for (unsigned int i = 0; i < strlen(delimiter); ++i) {
		text.erase(std::remove(text.begin(), text.end(), delimiter[i]), text.end());
	}

	std::stringstream ssin(text);

	//Buffer string into pairs 
	while (ssin >> bufferInterface >> bufferObject) {
		pairOfStrings bufferPair = std::make_pair(bufferInterface, bufferObject);
		requiredInterfaces.push_back(bufferPair);
	}

	return requiredInterfaces;
}

std::string DanaGrammer::generateType() {
	//Generate random type value 

	//MORE TYPES WILL BE ADDED
	const std::vector<std::string> availableTypes = { "int" };
	const int randomIndex = getRandomNumber(0, availableTypes.size() - 1);

	return availableTypes.at(randomIndex);
}

std::string DanaGrammer::generateVariableName() {
	//Generate variable name
	DanaVariable lastVariableDeclared = availableVariables.at(availableVariables.size() - 1);

	//Check if last variable was a generated one
	if (lastVariableDeclared.name.size() < 3)
		return "gen1";

	int lastVariableNumber = stoi(lastVariableDeclared.name.erase(0, 3));

	//Increase number by 1 and append to string 'gen'
	std::string newName = "gen" + std::to_string(lastVariableNumber + 1);
	return newName;
}

std::string DanaGrammer::generateFunctionCall() {
	//Generate random function call given the list of known functions
	pairOfStrings potentialCall = availableFunctions.at(getRandomNumber(0, availableFunctions.size() - 1));

	//Split string if there are more than one function parameters
	std::stringstream ssin(potentialCall.second);
	std::string buffer = "";
	std::vector<std::string> params;
	std::string result = "";
	int attempts = 0;
	bool foundAcceptableParam = false;

	while (ssin >> buffer)
		params.push_back(buffer);

	//Generate parameters from given variables
	for (size_t i = 0; i < params.size(); i++) {
		std::string newParam = getMatchingParam(params.at(i));

		if (newParam != "") {
			if (result.length() > 0)
				result = result + ", " + newParam;
			else
				result += newParam;

			//Max number of parameters reached
			if (params.size() - 1 == i)
				foundAcceptableParam = true;
		}
		else {
			foundAcceptableParam = false;
			break;
		}
	}

	std::string finalCall = potentialCall.first + " ( " + result + " )";

	//Return only if acceptable params can be found withen 
	return foundAcceptableParam ? finalCall : generateFunctionCall();
}

std::string DanaGrammer::getMatchingParam(std::string param) {
	//Find variable with same type as a required parameter type
	const int size = availableVariables.size();
	bool paramFound = false;
	std::string result;
	int attempts = 0;

	//Generate random list of integers
	std::vector<int> indexes;
	for (size_t i = 0; i < availableVariables.size(); i++) indexes.push_back(i);
	std::random_shuffle(indexes.begin(), indexes.end());

	//Look for variable with matching type
	for (size_t i = 0; i < indexes.size(); i++) {
		DanaVariable potentialParam = availableVariables.at(indexes.at(i));

		//Check for type match
		if (potentialParam.type == param)
			return potentialParam.name;
	}

	return "";
}

pairOfStrings DanaGrammer::assignFromArray() {
	//Generate assignment from an array	
	std::vector<DanaVariable> arrays;

	//Get all arrays from variable list
	for (size_t i = 0; i < availableVariables.size(); i++) {
		if (availableVariables.at(i).size != NULL)
			arrays.push_back(availableVariables.at(i));
	}

	DanaVariable newArrayAssign = arrays.at(getRandomNumber(0, arrays.size() - 1));

	//Return result syntax and type
	std::string type = newArrayAssign.type == "string" ? "char" : newArrayAssign.type;
	std::string result = " " + newArrayAssign.name + " [ " + std::to_string(getRandomNumber(0, newArrayAssign.size - 1)) + " ] ";
	pairOfStrings pResult = std::make_pair(type, result);
	return pResult;
}