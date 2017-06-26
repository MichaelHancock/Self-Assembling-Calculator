#include "GeneticTransform.h"

/* - Constructor - */

GeneticTransform::GeneticTransform(std::vector<DanaLineSet> &initialPopulation, std::vector<DanaFunction> &newFunctions, 
	std::string requires, std::string cmdInput, std::string cmdTarget) : input(cmdInput), target(cmdTarget) {
	srand((unsigned int)time(0));
	knownFunctions = newFunctions;
	for (auto i : initialPopulation)
		populaton.push_back(std::make_pair(i, 0));

	//	Check if initial population includes an array
	bool includedArrayAssign = false;
	for (auto func : initialPopulation) {
		std::vector<DanaVariable> declared = func.getAllVariables();

		for (auto var : declared) {
			
			//	Add array assign to knownFunctions if initial population includes an array variable
			if (var.isArray() && !includedArrayAssign) {
				includedArrayAssign = true;
				knownFunctions.push_back(DanaFunction(var));
			}
		}
	}

	//	Compose function header - including required functions
	functionHeader.push_back("component provides App requires" + requires + " {");
	functionHeader.push_back("\tint App:main(AppParam params[]) {");
}

/* - Private member functions - */

int GeneticTransform::getRandomNumber(int low, int high) {
	if (low == high)
		return low;

	if (low > high)
		std::swap(low, high);

	//	Generate random number
	high = (high - low) + 1;
	return rand() % high + low;
}

std::string GeneticTransform::getVariableName(int lineNumber) {
	assert(lineNumber >= 1 && lineNumber <= 26);
	return std::string(1, "abcdefghijklmnopqrstuvwxyz"[lineNumber - 1]);
}

std::vector<DanaLine> GeneticTransform::getAllLineVariations(int lineNumber, std::vector<DanaVariable> &knownVariables) {
	std::vector<DanaVariable> vars = knownVariables;
	std::vector<DanaLine> potentialLines = {};
	std::vector<DanaFunction> validFunctions = {};

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
			for (int i = 0; i < arr.size() - 1; i++) {
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
			std::vector<DanaVariable> filteredVariables = vars;

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

DanaLineSet GeneticTransform::correctVariableNames(DanaLineSet lnSet) {
	DanaLineSet newLineSet = lnSet;

	for (int i = 0; i < newLineSet.numberOfLines(); i++) {
		//	Store information about this current line
		DanaLine currentLine = newLineSet.getLine(i);
		DanaVariable currentLineVar = currentLine.getDeclaredVariable();
		DanaFunction currrentLineFunc = currentLine.getFunctionCalled();
		const std::string correctNameForThisLine = getVariableName(i + 1);
		const std::string currentName = currentLineVar.name;

		//	Check if var declared on this line has the right name
		if (correctNameForThisLine == currentName)
			continue;

		//	If not - replace the declaration on this line
		DanaVariable replaceLineVar = currentLineVar;
		replaceLineVar.name = correctNameForThisLine;
		newLineSet.insertLine(DanaLine(i, replaceLineVar, currrentLineFunc));
		
		//	If not - find and replace all instances
		for (int j = i; j < newLineSet.numberOfLines(); j++) {
			DanaLine checkLine = newLineSet.getLine(j);
			DanaVariable checkVar = checkLine.getDeclaredVariable();
			DanaFunction checkFunc = checkLine.getFunctionCalled();

			//	Rename variable usage instances - for array assign function calls
			if (checkFunc.isArrayAssign()) {
				DanaVariable arr = checkFunc.getFunctionObject();

				if (arr.name == currentName) {
					arr.name = correctNameForThisLine;
					DanaFunction replaceFunc = DanaFunction(arr);
					replaceFunc.setMatchedParam(checkFunc.getMatchedParameter(0), 0);
					newLineSet.insertLine(DanaLine(j, checkVar, replaceFunc));
				}
			}

			//	Rename variable usage instances - for use as parameter in a functtion call
			if (!checkFunc.isArrayAssign()) {
				std::vector<DanaVariable> params;
				DanaFunction replaceFunc = checkFunc;
				for (int k = 0; k < checkFunc.numberOfParameters(); k++)
					params.push_back(checkFunc.getMatchedParameter(k));

				for (int k = 0; k < params.size(); k++) {
					if (params.at(k).name == currentName) {
						DanaVariable replaceVar = params.at(k);
						replaceVar.name = correctNameForThisLine;
						replaceFunc.setMatchedParam(replaceVar, k);
					}
				}

				newLineSet.insertLine(DanaLine(j, checkVar, replaceFunc));
			}
		}
	}

	return newLineSet;
}

std::string GeneticTransform::runSystemCommand(std::string command) {
	//Execute system command and return result string
	const std::shared_ptr<FILE> pipe(_popen(command.c_str(), "r"), _pclose);
	std::array<char, 128> buffer;
	std::string result;

	//Check for early command fail
	if (!pipe)
		throw std::runtime_error("popen() failed!");

	//Read data from pipe
	while (!feof(pipe.get())) {
		if (fgets(buffer.data(), 128, pipe.get()) != NULL)
			result = result + buffer.data();
	}

	return result;
}

bool GeneticTransform::compileFunction(std::string path) {
	//Try to compile new function
	const std::string command = "dnc " + path;
	const int response = system(command.c_str());

	return response == 0;
}

bool GeneticTransform::testFunction(std::string path, std::string test, std::string expectedResult) {
	//Test new function and compare with target
	const std::string inputWithFormatting = "\"" + test + "\"";
	const std::string command = "dana " + path + " " + inputWithFormatting;
	const int response = system(command.c_str());

	return response == std::stoi(expectedResult);
}

void GeneticTransform::writeDataFile(std::string toWrite, std::string path) {
	//Output generated file
	std::ofstream generatedFile(path);
	generatedFile << toWrite;
	generatedFile.close();
}

std::string GeneticTransform::collpaseVector(std::vector<std::string> &lines) {
	std::string result = "";

	//	Collapse vector into string, add lines spaces
	for (int i = 0; i < lines.size(); i++) {
		result += lines.at(i) + "\n";
	}

	return result;
}

/* - Public member functions - */

DanaLineSet GeneticTransform::insertLine(DanaLineSet lnSet) {
	//	Get the variables of lnSet and select a newLineNumber for the line to be inserted
	DanaLineSet newLineSet = lnSet;
	std::vector<DanaVariable> scopedVars = newLineSet.getAllVariables();
	const int newLineNumber = newLineSet.numberOfLines() + 1;

	//	Generate all possible new lines - select one randomly to add to the lineset
	std::vector<DanaLine> possibleLines = getAllLineVariations(newLineNumber, scopedVars);
	const int randomIndex = getRandomNumber(0, (int)(possibleLines.size() - 1));
	newLineSet.insertLine(possibleLines.at(randomIndex));
	scopedVars.clear();
	possibleLines.clear();

	return newLineSet;
}

DanaLineSet GeneticTransform::modifyLine(DanaLineSet lnSet) {
	if (lnSet.numberOfLines() < 2)
		return lnSet;

	//	Select a random line to modify
	const int randomIndex = getRandomNumber(1, lnSet.numberOfLines() - 1);
	DanaLineSet newLineSet = lnSet;
	DanaLine lineToModify = newLineSet.getLine(randomIndex);
	const int lineNumber = lineToModify.getLineNumber();
	
	//	Get function called on this line
	DanaFunction functionCalled = lineToModify.getFunctionCalled();
	std::vector<std::string> paramTypes;
	for (int i = 0; i < functionCalled.numberOfParameters(); i++) {
		paramTypes.push_back(functionCalled.getParameterType(i));
	}

	//	Change index if array assign
	if (functionCalled.isArrayAssign()) {
		DanaVariable declaredVariable = lineToModify.getDeclaredVariable();
		DanaVariable arrayInCall = functionCalled.getFunctionObject();
		DanaVariable index = functionCalled.getMatchedParameter(0);
		index.setValue(std::to_string(getRandomNumber(0, arrayInCall.size() - 1)));
		functionCalled.setMatchedParam(index, 0);

		DanaLine newLine = DanaLine(lineNumber, declaredVariable, functionCalled);
		newLineSet.insertLine(newLine);
		return newLineSet;
	}

	//	Change params for other inscope variables
	std::vector<DanaVariable> inscope = newLineSet.variablesInScope(lineToModify.getLineNumber());
	for (int i = 0; i < paramTypes.size(); i++) {
		std::string param = paramTypes.at(i);
		std::vector<DanaVariable> filteredParams = inscope;
		filteredParams.erase(std::remove_if(filteredParams.begin(), filteredParams.end(),
			[param](DanaVariable v) { return v.type != param; }), filteredParams.end());

		functionCalled.setMatchedParam(filteredParams.at(getRandomNumber(0, (int)filteredParams.size() - 1)), i);
	}

	DanaLine newLine = DanaLine(lineNumber, lineToModify.getDeclaredVariable(), functionCalled);
	newLineSet.insertLine(newLine);
	return newLineSet;
}

DanaLineSet GeneticTransform::deleteLine(DanaLineSet lnSet) {
	if (lnSet.numberOfLines() == 0)
		return lnSet;
	
	//	Randomly select a line to delete
	DanaLineSet newLineSet = lnSet;
	const int deleteIndex = getRandomNumber(0, (int)(lnSet.numberOfLines() - 1));

	if (deleteIndex == 0)
		return lnSet;

	DanaLine lineToDelete = newLineSet.getLine(deleteIndex);
	DanaVariable variableToRemove = lineToDelete.getDeclaredVariable();
	std::vector<int> usagesOfVariable = newLineSet.findVariableUsages(variableToRemove);
	
	//	If var declared on the deleted line is used nowhere else 
	if (usagesOfVariable.size() == 0) {
		newLineSet.deleteLine(deleteIndex);
		newLineSet = correctVariableNames(newLineSet);
		return newLineSet;
	}

	//	If var declared on the deleted line is used elsewhere - do safe delete
	newLineSet.safeDelete({ deleteIndex });
	newLineSet = correctVariableNames(newLineSet);
	return newLineSet;
}

void GeneticTransform::crossover() {

}

void GeneticTransform::test() {
	for (int x = 0; x < populaton.size(); x++) {
		//	Store current function and set score to 0 initially 
		DanaLineSet currentFunction = populaton.at(x).first;
		double score = 0;

		if (currentFunction.numberOfLines() == 0) {
			populaton.at(x).second = 0;
			continue;
		}

		//	Does the function return an integer
		if (currentFunction.getLine(currentFunction.numberOfLines() - 1).getDeclaredVariable().type == "int") {
			score++;
		}

		//	How many lines does the function have
		const int numberOfLines = currentFunction.numberOfLines();
		const int bestNumberOfLines = 6;
		const int distance = abs(bestNumberOfLines - numberOfLines);
		score = distance == 0 ? score += 2 : score += ((double)1 / distance);

		//	How many functions does the function use
		const int numberOfFunctions = (int)knownFunctions.size();
		std::vector<std::string> usedFunctions;
		for (int i = 0; i < numberOfLines; i++) {
			DanaFunction funcUsedOnThisLine = currentFunction.getLine(i).getFunctionCalled();
			if (std::find(usedFunctions.begin(), usedFunctions.end(),
				funcUsedOnThisLine.name + std::to_string(funcUsedOnThisLine.isArrayAssign())) == usedFunctions.end()) {
				usedFunctions.push_back(funcUsedOnThisLine.name + std::to_string(funcUsedOnThisLine.isArrayAssign()));
			}
		}
		score += (double)usedFunctions.size() / (double)numberOfFunctions;

		//	Compose function for testing
		std::vector<std::string> outFunction = functionHeader;
		const int lineCount = currentFunction.numberOfLines();
		const DanaVariable varToReturn = currentFunction.getLine(lineCount - 1).getDeclaredVariable();
		for (int i = 0; i < lineCount; i++) 
			outFunction.push_back("\t\t" + currentFunction.getLine(i).composeLine());
		outFunction.push_back("\t\treturn " + varToReturn.name);
		outFunction.push_back("\t}");
		outFunction.push_back("}");

		//	Compile and test function
		const std::string function = collpaseVector(outFunction);
		writeDataFile(function, outputPath);
		if (compileFunction(outputPath)) {
			if (testFunction(compilePath, input, target)) {
				score += 1;
				resultFunction = function;
			}
		}

		//	Set score to the population func / score pair
		populaton.at(x).second = score;
	}
}

void GeneticTransform::rank() {
	//	Use selection sort to rank population
	for (int i = 0; i < populaton.size() - 1; i++) {
		for (int j = i + 1; j < populaton.size(); j++) {
			if (populaton[i].second > populaton[j].second) {
				std::swap(populaton[i], populaton[j]);
			}
		}
	}
}

std::string GeneticTransform::cycleGeneration() {
	while (resultFunction == "") {
		test();
		rank();

		//	For each member of the population - perform a random muation
		for (int i = 0; i < populaton.size(); i++) {
			const int whichMutation = getRandomNumber(0, 2);
			switch (getRandomNumber(0, 2)) {
			case 0:
				populaton.at(i).first = insertLine(populaton.at(i).first);
				break;
			case 1:
				populaton.at(i).first = deleteLine(populaton.at(i).first);
				break;
			default:
				populaton.at(i).first = modifyLine(populaton.at(i).first);
				break;
			}
		}
	}

	//	Write result function to file so it can be tested by the client 
	writeDataFile(resultFunction, outputPath);
	compileFunction(outputPath);

	return resultFunction;
}

std::vector<std::pair<DanaLineSet, double>> GeneticTransform::getPopulation() {
	return populaton;
} 

/* - Destructor - */

GeneticTransform::~GeneticTransform() {
	populaton.clear();
}