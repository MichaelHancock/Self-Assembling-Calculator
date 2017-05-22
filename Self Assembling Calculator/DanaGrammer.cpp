#include "DanaGrammer.h"

DanaGrammer::DanaGrammer(std::string required, ListOfVariables variables) {
	//	Initialise local memory
	knownFunctions = ListOfFunctions();
	knownVariables = ListOfVariables();
	generatedFunctions = ListOfStrings();
	functionHeader = ListOfStrings();

	//	Add passed variables into local list of known variables
	knownVariables = variables;

	//	Add functions into a local list of known functions
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
	store.addNewListOfLines({ "char a[] = params[0].string" });
}

ListOfStrings DanaGrammer::generateAllFunctonInstances(int numberOfLines) {
	ListOfStrings lines = functionHeader;
	ListOfStrings potentialLines = {};
	int currentLineNumber = 2;

	//	Return only hardcoded elements if less than 2 lines
	if (numberOfLines < 2) {
		lines.push_back("\t\treturn 0");
		lines.push_back("\t}");
		lines.push_back("}");

		return { { collpaseVector(lines) } };
	}

	//	Iterate through available functions
	for (int i = 0; i < knownFunctions.size(); i++) {
		DanaFunction currentFunction = knownFunctions.at(i);
		ListOfVariables matchedParams = {};
		
		//	Find variables which match the currentFunction parameters  
		for (int j = 0; j < currentFunction.numberOfParameters(); j++) {
			const std::string currentParam = currentFunction.getParameter(j);

			for (int k = 0; k < knownVariables.size(); k++) {
				DanaVariable currentVariable = knownVariables.at(k);

				if (!currentFunction.isArrayAssign && currentVariable.type == currentParam) {
					//	If currentVariable matches one of the currentFunction parameters, add it to matched params
					if (!(std::find(matchedParams.begin(), matchedParams.end(),
						currentVariable) != matchedParams.end())) {
						matchedParams.push_back(currentVariable);
					}
				}
				else if (currentFunction.isArrayAssign && currentVariable.size > 0) {
					//	If current function is an array assign. Add all valid array indexes to matched params
					for (int l = 0; l < currentVariable.size; l++) {
						DanaVariable a = DanaVariable(currentVariable.name, "int");
						a.value = l;
						matchedParams.push_back(a);
					}
				}
			}
		}

		if (matchedParams.size() > 0) {
			if (currentFunction.numberOfParameters() == 1) {
				//	If variables have been found which match this functions parameter
				for (int j = 0; j < matchedParams.size(); j++) {
					std::string outputLine = currentFunction.returnType + " ";
					outputLine += getVariableName(currentLineNumber);
					outputLine += " = ";
					if (!currentFunction.isArrayAssign)
						outputLine += currentFunction.name + " ( " + matchedParams.at(j).name + " )";
					else
						outputLine += matchedParams.at(j).name + " [ " + std::to_string(matchedParams.at(j).value) + " ]";

					potentialLines.push_back(outputLine);
				}
			}
			else if (currentFunction.numberOfParameters() == 2 && currentFunction.getParameter(0) == currentFunction.getParameter(1)) {
				//	If variables have been found which match one of the two parameters of this function
				const std::string currentParam = currentFunction.getParameter(0);
				ListOfVariables filteredParams = matchedParams;

				//	Filter variables which cannot be used to populate this functions parameters
				filteredParams.erase(std::remove_if(filteredParams.begin(), filteredParams.end(),
					[currentParam](DanaVariable v) { return v.type != currentParam; }), filteredParams.end());

				//	Generate all pair combinations of variables for this function
				for (int j = 0; j < filteredParams.size(); j++) {
					for (int k = 0; k < filteredParams.size(); k++) {
						std::string outputLine = currentFunction.returnType + " ";
						outputLine += getVariableName(currentLineNumber);
						outputLine += " = ";
						outputLine += currentFunction.name + " ( " + filteredParams.at(j).name + ", " + filteredParams.at(k).name + " )";
						potentialLines.push_back(outputLine);
					}
				}
			}
		}
	}

	//	Add generated lines to store and output generated functions
	store.addNewListOfLines(potentialLines);
	std::string variableToReturn = getVariableName(currentLineNumber);
	currentLineNumber++;
	for (int i = 0; i < store.getList(0).size(); i++) {
		for (int j = 0; j < store.getList(1).size(); j++) {
			ListOfStrings output = functionHeader;
			output.push_back("\t\t" + store.getList(0).at(i));
			output.push_back("\t\t" + store.getList(1).at(j));
			output.push_back("\t\treturn " + variableToReturn);
			output.push_back("\t}");
			output.push_back("}");

			generatedFunctions.push_back(collpaseVector(output));
		}
	}

	return generatedFunctions;
}

std::string DanaGrammer::getVariableName(int lineNumber) {
	assert(lineNumber >= 1 && lineNumber <= 26);
		return std::string(1, "abcdefghijklmnopqrstuvwxyz"[lineNumber - 1]);
}

std::string DanaGrammer::collpaseVector(ListOfStrings lines) {
	std::string result = "";

	//	Collapse vector into string, add lines spaces
	for (int i = 0; i < lines.size(); i++) {
		result += lines.at(i) + "\n";
	}

	return result;
}

