#include "DanaGrammer.h"

DanaGrammer::DanaGrammer(std::string required, ListOfVariables variables) {
	//	Initialise local memory
	knownFunctions = ListOfFunctions();
	knownVariables = ListOfVariables();
	generatedFunctions = ListOfStrings();
	functionHeader = ListOfStrings();

	//	Add passed variables into local list of known variables
	knownVariables = variables;

	//	Add functions  into local list of known functions
	knownFunctions.push_back(DanaFunction("uInt.intFromString", "int", { "char" }));
	knownFunctions.push_back(DanaFunction("adder.add", "int", { "int", "int" }));

	//	Add array variables into list of knownFunctions - for array assignments
	for (int i = 0; i < knownVariables.size(); i++) {
		if (knownVariables.at(i).size > 0) {
			knownFunctions.push_back(knownVariables.at(i));
		}
	}

	//	Add hardcoded function elements
	functionHeader.push_back("component provides App requires" + required + " {");
	functionHeader.push_back("\tint App:main(AppParam params[]) {");
	functionHeader.push_back("\t\tchar a[] = params[0].string");
}

ListOfStrings DanaGrammer::generateAllFunctonInstances(int numberOfLines) {
	ListOfStrings lines = functionHeader;

	//	Return only hardcoded elements if less than 2 lines
	if (numberOfLines < 2) {
		lines.push_back("\t}");
		lines.push_back("}");

		return { collpaseVector(lines) };
	}

	//	Iterate through available functions
	for (int i = 0; i < knownFunctions.size(); i++) {

	}

	return generatedFunctions;
}

std::string DanaGrammer::collpaseVector(ListOfStrings lines) {
	std::string result = "";

	//	Collapse vector into string, add lines spaces
	for (int i = 0; i < lines.size(); i++) {
		result += lines.at(i) + "\n";
	}

	return result;
}

