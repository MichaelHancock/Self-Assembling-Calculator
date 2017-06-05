#include "DanaGrammer.h"

/* --- Public Member Functions --- */

DanaGrammer::DanaGrammer(std::string required, ListOfVariables variables) {

}

ListOfStrings DanaGrammer::generateAllFunctonInstances() {

	return {};
}

/* --- Private Member Functions --- */

std::string DanaGrammer::getVariableName(int lineNumber) {
	assert(lineNumber >= 1 && lineNumber <= 26);
	return std::string(1, "abcdefghijklmnopqrstuvwxyz"[lineNumber - 1]);
}

ListOfStrings DanaGrammer::split(std::string str) {
	std::string buffer; 
	std::stringstream ss(str); 
	ListOfStrings tokens; 

	while (ss >> buffer)
		tokens.push_back(buffer);

	return tokens;
}