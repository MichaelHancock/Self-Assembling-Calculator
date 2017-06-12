#include "DanaGrammer.h"

/* --- Public Member Functions --- */

DanaGrammer::DanaGrammer(std::string required, ListOfFunctions newFunctions, DanaVariable functionParameter, int usfulParamIndex) {
	//	Set initial line to pull data from function param args 
	DanaFunction primaryFunctionCall = DanaFunction(functionParameter, "string");
	primaryFunctionCall.setMatchedParam(DanaVariable("x", "int", std::to_string(usfulParamIndex)), 0);
	DanaLine primaryLine = DanaLine(0, DanaVariable("a", "char", 5), primaryFunctionCall);
	store = LineStore({ primaryLine });

	//	Initialise known functions
	knownFunctions = newFunctions;
	if (primaryLine.getDeclaredVariable().isArray()) {
		knownFunctions.push_back(DanaFunction(primaryLine.getDeclaredVariable()));
	}
	
	//	Compose function header - including required functions
	functionHeader.push_back("component provides App requires" + required + " {");
	functionHeader.push_back("\tint App:main(AppParam params[]) {");
}

ListOfStrings DanaGrammer::generateAllFunctonInstances() {
	int currentLineNumber = store.size() + 1;
	ListOfLines potentialLines = {};
	Assembler asmb1 = Assembler(store);
	std::vector<DanaLineSet> lineVariations = asmb1.makeOnlyLines();
	std::vector<std::string> attempts = {};

	knownVariables.clear();
	for (auto lineSet : lineVariations) {
		for (int line = 0; line < lineSet.numberOfLines(); line++) {
			knownVariables.push_back(lineSet.getLine(line).getDeclaredVariable());
		}

		std::string newAttempt = "";
		for (auto var : knownVariables) 
			newAttempt += var.composeVariable() + "\n";

		if (!(std::find(attempts.begin(), attempts.end(), newAttempt) != attempts.end())) {
			attempts.push_back(newAttempt);
			ListOfLines newLines = getAllLineVariations(currentLineNumber, knownVariables);
			potentialLines.insert(potentialLines.end(), newLines.begin(), newLines.end());
			newLines.clear();
		}

		knownVariables.clear();
	}

	//	Add the new lines to the store 
	store.addLines(potentialLines);
	potentialLines.clear();

	//	Assemble all possible variations of the store
	Assembler asmb = Assembler(store, functionHeader);
	return asmb.makeFunctions();
}

/* --- Private Member Functions --- */

ListOfLines DanaGrammer::getAllLineVariations(int lineNumber, ListOfVariables vars) {
	std::vector<DanaLine> potentialLines = {};
	ListOfFunctions validFunctions = {};

	//	Look for functions whos parameters we can match
	for (auto thisFunc : knownFunctions) {

		//	If array in availableVariables, add array assign function to validFunctions
		if (thisFunc.isArrayAssign()) {
			bool arrayFound = false;

			for (auto thisVar : vars) {
				if (thisVar.isArray())
					arrayFound = true;
			}

			if (arrayFound) {
				validFunctions.push_back(thisFunc);
				continue;
			}
		}

		//	Iterate through function params - searching for matches in vars 
		int numberOfParamsMatched = 0;
		for (int i = 0; i < thisFunc.numberOfParameters(); i++) {
			const std::string currentParamType = thisFunc.getParameterType(i);

			for (auto thisVar : vars) {
				if (thisVar.type == currentParamType && !thisVar.isArray()) {
					numberOfParamsMatched++;
					break;
				}
			}
		}

		//	If matching vars are found, add to validFunctions
		if (numberOfParamsMatched == thisFunc.numberOfParameters())
			validFunctions.push_back(thisFunc);
	}

	//	Find all parameter variations for validFunctions 
	for (auto thisFunc : validFunctions) {
		if (thisFunc.isArrayAssign()) {
			DanaVariable arr;

			//	Get array from vars
			for (auto thisVar : vars) {
				if (thisVar.isArray())
					arr = thisVar;
			}

			//	Generate a new potential line foreach possible array index
			for (int i = 0; i < arr.size(); i++) {
				thisFunc.setMatchedParam(DanaVariable("x", "int", std::to_string(i)), 0);
				DanaLine newLine = DanaLine(lineNumber - 1,
					DanaVariable(getVariableName(lineNumber), arr.type), thisFunc);
				potentialLines.push_back(newLine);
			}
		}
		else if (thisFunc.numberOfParameters() == 1) {
			const std::string param = thisFunc.getParameterType(0);
			std::vector<DanaVariable> filteredVariables = vars;

			//	Filter variables which cannot be used to populate this functions parameters
			filteredVariables.erase(std::remove_if(filteredVariables.begin(), filteredVariables.end(),
				[param](DanaVariable v) { return v.type != param || v.isArray(); }), filteredVariables.end());

			for (auto var : filteredVariables) {
				thisFunc.setMatchedParam(var, 0);
				DanaLine newLine = DanaLine(lineNumber - 1,
					DanaVariable(getVariableName(lineNumber), thisFunc.returnType), thisFunc);
				potentialLines.push_back(newLine);
			}
		}
		else if (thisFunc.numberOfParameters() == 2 && thisFunc.getParameterType(0) == thisFunc.getParameterType(1)) {
			const std::string param = thisFunc.getParameterType(0);
			ListOfVariables filteredVariables = vars;

			//	Filter variables which cannot be used to populate this functions parameters
			filteredVariables.erase(std::remove_if(filteredVariables.begin(), filteredVariables.end(),
				[param](DanaVariable v) { return v.type != param; }), filteredVariables.end());

			for (auto var1 : filteredVariables) {
				for (auto var2 : filteredVariables) {
					thisFunc.setMatchedParam(var1, 0);
					thisFunc.setMatchedParam(var2, 1);
					DanaLine newLine = DanaLine(lineNumber - 1,
						DanaVariable(getVariableName(lineNumber), thisFunc.returnType), thisFunc);
					potentialLines.push_back(newLine);
				}
			}
		}
	}

	validFunctions.clear();
	return potentialLines;
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