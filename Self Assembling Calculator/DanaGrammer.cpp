#include "DanaGrammer.h"

/* --- Public Member Functions --- */

DanaGrammer::DanaGrammer(std::string required, ListOfVariables variables) {
	//	Initialise local memory
	knownFunctions = ListOfFunctions();
	knownVariables = ListOfVariables();
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
	numberOfGeneratedLines = 1;
}

ListOfStrings DanaGrammer::generateAllFunctonInstances() {
	knownVariables.clear();
	for (int i = 0; i < store.size(); i++) {
		DanaVariable var;
		ListOfStrings line = {};

		for (int j = 0; j < store.getList(i).size(); j++) {
			line = split(store.getList(i).at(j));
			if (validateVariable(line)) {
				break;
			}
		}

		if (line.at(1).size() > 1 && line.at(1).at(1) == '[') {
			line.at(1).pop_back();
			line.at(1).pop_back();
			var = DanaVariable(line.at(1), line.at(0) == "char" ? "string" : line.at(0), 5);
		}
		else {
			var = DanaVariable(line.at(1), line.at(0));
		}

		
		knownVariables.push_back(var);
	}

	ListOfStrings potentialLines = getAllPossibleLines(knownFunctions, knownVariables, numberOfGeneratedLines + 1);
	std::vector<std::pair<int, int>> iterators;

	//	Add generated lines to store and output generated functions
	store.addNewListOfLines(potentialLines);
	numberOfGeneratedLines++;
	std::string variableToReturn = getVariableName(numberOfGeneratedLines);
	
	Assembler asmb = Assembler(store, functionHeader);
	return asmb.make();
}

/* --- Private Member Functions --- */

ListOfStrings DanaGrammer::getAllPossibleLines(ListOfFunctions availableFunctions, ListOfVariables availableVariables, int lineIndex) {
	ListOfStrings lines = functionHeader;
	ListOfStrings potentialLines = {};
	int currentLineNumber = lineIndex;

	//	Iterate through available functions
	for (int i = 0; i < availableFunctions.size(); i++) {
		DanaFunction currentFunction = availableFunctions.at(i);
		ListOfVariables matchedParams = {};

		//	Find variables which match the currentFunction parameters  
		for (int j = 0; j < currentFunction.numberOfParameters(); j++) {
			const std::string currentParam = currentFunction.getParameter(j);

			for (int k = 0; k < availableVariables.size(); k++) {
				DanaVariable currentVariable = availableVariables.at(k);

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

	return potentialLines;
}

bool DanaGrammer::validateVariable(ListOfStrings line) {
	if (line.size() > 4 && line.at(4) == "(") {
		for (int j = 0; j < knownFunctions.size(); j++) {
			if (knownFunctions.at(j).name == line.at(3)) {
				int numberOfParamMatches = 0;
				DanaFunction current = knownFunctions.at(j);
				ListOfStrings params = {};
				for (int l = 4; l < line.size(); l++) {
					if (line.at(l) != "," && line.at(l) != "(" && line.at(l) != ")") {
						params.push_back(line.at(l));
					}
				}

				for (int l = 0; l < knownVariables.size(); l++) {
					for (int m = 0; m < params.size(); m++) {
						if (knownVariables.at(l).name == params.at(m)) {
							DanaVariable currentVariable = knownVariables.at(l);

							for (int n = 0; n < current.numberOfParameters(); n++) {
								if (currentVariable.type == current.getParameter(n)) {
									numberOfParamMatches++;
								}
							}
						}
					}
				}

				if (numberOfParamMatches == current.numberOfParameters()) 
					return true;
				else 
					return false;
			}
		}
	}

	else 
		return true;
}

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